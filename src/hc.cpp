#include <exception>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
// #include <thread>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>

#include "hc.hpp"
#include "result.hpp"

namespace HengCore
{
Excutable::Excutable(const Config::Config &cfg):
    cfg(cfg),
    cgp("hengCore/" + std::to_string(getpid())),
    logger("hengCore/Excutable/" + std::to_string(getpid()))
{
    if(cfg.memLimit > 0)
    {
        cgp.setMemLimit(cfg.memLimit);
    }
    if(cfg.maxPid > 0)
    {
        cgp.setPidLimit(cfg.maxPid);
    }
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
            gid_t curGid = getgid();
            uid_t curUid = getuid();
            FILE *in     = nullptr;
            FILE *out    = nullptr;
            FILE *err    = nullptr;
            if(curGid != cfg.gid)
            {
                if(setgid(cfg.gid) != 0)
                {
                    logger.err("Failed to set gid from "
                               + std::to_string(curGid)
                               + " to "
                               + std::to_string(cfg.gid));
                    std::abort();
                }
                else
                {
                    logger.log("Set gid from "
                               + std::to_string(curGid)
                               + " to "
                               + std::to_string(cfg.gid));
                }
            }
            if(curUid != cfg.uid)
            {
                if(setuid(cfg.uid) != 0)
                {
                    logger.err("Failed to set uid from "
                               + std::to_string(curUid)
                               + " to "
                               + std::to_string(cfg.uid));
                    std::abort();
                }
                else
                {
                    logger.log("Set uid from "
                               + std::to_string(curUid)
                               + " to "
                               + std::to_string(cfg.uid));
                }
            }

            if(cfg.stdin == cfg.stdout
               || cfg.stdin == cfg.stderr)
            {
                logger.err(
                  "stdin can't equal to stdout nor stderr");
                std::abort();
            }

            in  = fopen(cfg.stdin.c_str(), "r");
            out = fopen(cfg.stdout.c_str(), "w+");
            if(cfg.stdout != cfg.stderr)
            {
                err = fopen(cfg.stderr.c_str(), "w+");
            }
            else
            {
                err = out;
            }

            if(in == nullptr || out == nullptr
               || err == nullptr)
            {
                if(in != nullptr)
                {
                    fclose(in);
                }
                else
                {
                    logger.err(
                      "Failed to open file"
                      "(in)");
                }
                if(out != nullptr)
                {
                    fclose(out);
                }
                else
                {
                    logger.err(
                      "Failed to open file"
                      "(out)");
                }
                if(err != nullptr && err != out)
                {
                    fclose(err);
                }
                else
                {
                    logger.err(
                      "Failed to open file"
                      "(err)");
                }
                std::abort();
            }
            logger.flush();
            if(dup2(fileno(in), fileno(stdin)) == -1
               || dup2(fileno(out), fileno(stdout)) == -1
               || dup2(fileno(err), fileno(stderr)) == -1)
            {
                std::abort();
            }

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
            // It's parent process
            cgp.attach(childPid);
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
    logger.log("WaitChild Child process");
    int                status;
    std::vector<pid_t> vec;
    do
    {
        vec = cgp.getPidInGroup();
        logger.log("WaitChild, Found "
                   + std::to_string(vec.size()));
        for(auto pid: vec)
        {
            int res;
            logger.log("WaitChild, PID "
                       + std::to_string(pid));
            res = waitpid(pid, &status, 0);
            if(res == -1)
            {
                logger.err("WaitChild,GetError "
                           + std::to_string(errno));
                kill(pid, SIGKILL);
            }
            logger.log("WaitChild, PID "
                       + std::to_string(pid) + " Stoped");
        }
        // sleep(1);
    } while(vec.size() != 0);
    return true;
    // return kill(childPid, SIGKILL) == -1;
}
bool Excutable::killChild()
{
    logger.log("KillChild Child process");
    std::vector<pid_t> vec;
    do
    {
        vec = cgp.getPidInGroup();
        logger.log("KillChild, Found "
                   + std::to_string(vec.size()));
        for(auto pid: vec)
        {
            logger.log("KillChild, PID "
                       + std::to_string(pid));
            // waitpid(pid, &status, 0);
            kill(pid, SIGKILL);
            logger.log("KillChild, PID "
                       + std::to_string(pid) + " Stoped");
        }
        // sleep(1);
    } while(vec.size() != 0);
    return true;
    // return kill(childPid, SIGKILL) == -1;
}
bool Excutable::killTimer()
{
    return kill(timerPid, SIGKILL) == -1;
}
}  // namespace HengCore