#include <exception>
#include <vector>

#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
// #include <thread>
#include <unistd.h>

#include "hc.hpp"
#include "result.hpp"

namespace HengCore
{
Excutable::Excutable(const Config::Config &cfg):
    cfg(cfg),
    cgp("hengCore/" + std::to_string(getpid())),
    logger("hengCore/Excutable/" + std::to_string(getpid()))
{
}

bool Excutable::exec()
{
    timer.begin();
    childPid = fork();
    if(childPid < 0)
    {
        // throw std::runtime_error("UnableToFork");
        logger.err("Failed to create child process");
        return false;
    }
    else
    {
        if(childPid == 0)
        {
            // It's child process
            logger.log("In child process , pid "
                       + std::to_string(getpid()));
            char *vec[cfg.args.size() + 2];
            int   vecSize = 0;
            vec[vecSize++] =
              const_cast<char *>(cfg.bin.c_str());
            for(const auto &s: cfg.args)
            {
                vec[vecSize++] =
                  const_cast<char *>(s.c_str());
            }
            vec[vecSize++] = nullptr;
            execv(cfg.bin.c_str(), vec);
            logger.err("Failed to replace");
            std::abort();
            // execl(cfg.bin.c_str(), cfg.args.c_str());
            return false;
        }
        else
        {
            logger.log("ChildPid is "
                       + std::to_string(childPid));
            cgp.attach(childPid);
            // It's parent process
            if(cfg.timeLimit > 0)
            {
                timerPid = fork();
                if(timerPid < 0)
                {
                    killChild();
                    return false;
                }
                else
                {
                    if(timerPid == 0)
                    {
                        // It's timer process
                        sleep(cfg.timeLimit);
                        logger.log("Time out");
                        killChild();
                        std::exit(0);
                    }
                    else
                    {
                        logger.log(
                          "timerPid is "
                          + std::to_string(timerPid));
                    }
                }
            }

            if(cfg.memLimit > 0)
            {
                cgp.setMemLimit(cfg.memLimit);
            }

            return true;
        }
    }
}

Result::Result Excutable::getResult()
{
    logger.log("Try getResult");
    Result::Result res;
    // int            status;
    // waitpid(childPid, &status, 0);
    waitChild();
    killTimer();
    logger.log("Child process stoped");
    timer.stop();
    res.time = timer.get();
    res.mem  = cgp.getMemUsage();
    return res;
}
bool Excutable::waitChild()
{
    logger.log("Wait Child process");
    int status;
    int pid;
    while(true)
    {
        pid = wait(&status);
        if(pid == -1 && errno == ECHILD)
        {
            break;
        }
        logger.log("Child " + std::to_string(pid)
                   + " Stoped");
    }
    return true;
    // return kill(childPid, SIGKILL) == -1;
}
bool Excutable::killChild()
{
    logger.log("Kill Child process");
    int status;
    int pid;
    while(true)
    {
        pid = waitpid(-1, &status, WNOHANG);
        if(pid == -1 && errno == ECHILD)
        {
            break;
        }
        kill(pid, SIGKILL);
        logger.log("Child " + std::to_string(pid)
                   + " Killed");
    }
    return true;
    // return kill(childPid, SIGKILL) == -1;
}
bool Excutable::killTimer()
{
    return kill(timerPid, SIGKILL) == -1;
}
}  // namespace HengCore