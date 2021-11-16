#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <functional>
#include <random>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "config.hpp"
#include "hc.hpp"
#include "result.hpp"
#include "timer.hpp"

namespace HengCore
{
std::random_device rd;
std::mt19937_64    rnd64(rd());
Excutable::Excutable(const Config::Config &cfg):
    cfg(cfg),
    cgp("hengCore/"
        + std::to_string(std::hash<Config::Config>()(cfg))
        + std::to_string(
          static_cast<unsigned long long>(rnd64()))),
    logger("hengCore/Excutable/" + std::to_string(getpid()))
{
    if(cfg.memLimit > 0)
    {
        if(!cgp.setMemLimit(cfg.memLimit))
        {
            throw std::runtime_error(
              "fail to set memory limit");
        }
    }
    if(cfg.maxPid > 0)
    {
        if(!cgp.setPidLimit(cfg.maxPid))
        {
            throw std::runtime_error(
              "fail to set pid limit");
        }
    }
    if(cfg.maxCpu > 0)
    {
        if(!cgp.setCpuLimit(cfg.maxCpu))
        {
            throw std::runtime_error(
              "fail to set cpu limit");
        }
    }
}

bool Excutable::exec()
{
    sign       = -1;
    returnCode = -1;
    // real time collector start
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
            // https://github.com/torvalds/linux/blob/a9c9a6f741cdaa2fa9ba24a790db8d07295761e3/kernel/cgroup/cgroup.c#L2816
            cgp.attach(0);
            // inChild -> exit()
            inChild();
            // unreachable code
            return false;
        }
        else
        {
            logger.log("ChildPid is "
                       + std::to_string(childPid));
            // It's parent process
            if(cfg.timeLimit > 0)
            {
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
                        // inTimer -> exit()
                        inTimer();
                        // unreachable code
                    }
                    else
                    {
                        // It's parent process
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
    logger.log("After waitChild, all child process stoped");
    timer.stop();
    killTimer();
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
    logger.log("Wait child process");
    int status;
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
    logger.log(
      std::string("main chlid process dead, kill any other "
                  "chlid process"));
    killChild();
    return true;
}
bool Excutable::killChild()
{
    logger.log("Kill child process");
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
            kill(pid, SIGKILL);
            logger.log("KillChild, PID "
                       + std::to_string(pid)
                       + " Kill Signal Send");
        }
        usleep(10000);  // 10ms
    } while(vec.size() != 0);
    return true;
}
bool Excutable::killTimer()
{
    if(timerPid != -1)
    {
        return kill(timerPid, SIGKILL) != -1;
    }
    else
    {
        return false;
    }
}
}  // namespace HengCore