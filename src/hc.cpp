#include <exception>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
// #include <thread>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>

#include "config.hpp"
#include "hc.hpp"
#include "result.hpp"
#include "timer.hpp"

namespace HengCore
{
Excutable::Excutable(const Config::Config &cfg):
    cfg(cfg),
    cgp("hengCore/"
        + std::to_string(std::hash<Config::Config>()(cfg))),
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
    sign       = -1;
    returnCode = -1;
    timer.begin();
    childPid = fork();
    if(childPid < 0)
    {
        int errcode = errno;
        logger.err("Failed to create child process because "
                   + std::string(strerror(errcode)));
        return false;
    }
    else
    {
        if(childPid == 0)
        {
            // It's child process
            inChild();
            // should not continue;
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
                if(pipe(timerPipe) != 0)
                {
                    int errcode = errno;
                    logger.err(
                      "Failed to create timer pipe "
                      "because "
                      + std::string(strerror(errcode)));
                    killChild();
                    return false;
                }
                close(timerPipe[1]);
                timerPid = fork();
                if(timerPid < 0)
                {
                    int errcode = errno;
                    logger.err(
                      "Failed to create timer process "
                      "because "
                      + std::string(strerror(errcode)));
                    killChild();
                    return false;
                }
                else
                {
                    if(timerPid == 0)
                    {
                        // It's timer process
                        close(timerPipe[0]);
                        inTimer();
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
    waitChild();
    logger.log("Child process stoped");
    timer.stop();
    if(timerPid != -1)
    {
        killTimer();
    }
    res.time.real  = timer.get();
    res.time.usr   = cgp.getTimeUsr();
    res.time.sys   = cgp.getTimeSys();
    res.memory     = cgp.getMemUsage();
    res.returnCode = returnCode;
    res.signal     = sign;
    return res;
}
bool Excutable::waitChild()
{
    logger.log("WaitChild Child process");
    int                status;
    std::vector<pid_t> vec;
    if(waitpid(childPid, &status, 0) != -1)
    {
        if(WIFEXITED(status))
        {
            returnCode = WEXITSTATUS(status);
        }
        if(WIFSIGNALED(status))
        {
            sign = WTERMSIG(status);
        }
    }
    do
    {
        vec = cgp.getPidInGroup();
        logger.log("WaitChild, Found "
                   + std::to_string(vec.size()));
        for(auto pid: vec)
        {
            if(pid != childPid)
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
                           + std::to_string(pid)
                           + " Stoped");
            }
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
    if(timerPid != -1)
    {
        return kill(timerPid, SIGKILL) == -1;
    }
    else
    {
        return false;
    }
}
}  // namespace HengCore