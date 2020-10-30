#include <exception>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
// #include <thread>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
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

void Excutable::childExit(
  Excutable::ChildErrcode returnCode)
{
    std::quick_exit(static_cast<int>(returnCode));
}

void Excutable::inChild()
{
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
            int err = errno;
            logger.err("Failed to set gid from "
                       + std::to_string(curGid) + " to "
                       + std::to_string(cfg.gid)
                       + " because " + strerror(err));
            childExit(ChildErrcode::GIDFAIL);
        }
        else
        {
            logger.log("Set gid from "
                       + std::to_string(curGid) + " to "
                       + std::to_string(cfg.gid));
        }
    }
    else
    {
        logger.log("Skip set gid");
    }
    if(curUid != cfg.uid)
    {
        if(setuid(cfg.uid) != 0)
        {
            int err = errno;
            logger.err("Failed to set uid from "
                       + std::to_string(curUid) + " to "
                       + std::to_string(cfg.uid)
                       + " because " + strerror(err));
            childExit(ChildErrcode::UIDFAIL);
        }
        else
        {
            logger.log("Set uid from "
                       + std::to_string(curUid) + " to "
                       + std::to_string(cfg.uid));
        }
    }
    else
    {
        logger.log("Skip set uid");
    }
    if(cfg.stdin == cfg.stdout || cfg.stdin == cfg.stderr)
    {
        logger.err(
          "stdin can't equal to stdout nor stderr");
        childExit(ChildErrcode::IOCONFILCT);
    }
    else
    {
        logger.log("IO Confilct checked");
    }

    if(!cfg.cwd.empty())
    {
        if(chdir(cfg.cwd.c_str()) == -1)
        {
            int err = errno;
            logger.err(
              "Failed to Set Cwd"
              " because "
              + std::string(strerror(err)));
            childExit(ChildErrcode::SETCWD);
        }
        else
        {
            logger.log("Cwd Set");
        }
    }
    else
    {
        logger.log("Cwd skip");
    }

    if(cfg.stdin != Config::Config::defaultStdin)
    {
        in = fopen(cfg.stdin.c_str(), "r");
        if(in == nullptr)
        {
            int err = errno;
            logger.err("Failed to open " + cfg.stdin
                       + "(in) because " + strerror(err));
            childExit(ChildErrcode::OPENSTDIN);
        }
    }
    else
    {
        logger.log("stdin skip");
    }
    if(cfg.stdout != Config::Config::defaultStdout)
    {
        out = fopen(cfg.stdout.c_str(), "r");
        if(out == nullptr)
        {
            int err = errno;
            logger.err("Failed to open " + cfg.stdout
                       + "(out) because " + strerror(err));
            childExit(ChildErrcode::OPENSTDOUT);
        }
    }
    else
    {
        logger.log("stdout skip");
    }
    if(cfg.stderr != Config::Config::defaultStderr)
    {
        if(cfg.stderr != cfg.stdout)
        {

            err = fopen(cfg.stderr.c_str(), "r");
            if(err == nullptr)
            {
                int errcode = errno;
                logger.err("Failed to open " + cfg.stderr
                           + "(err) because "
                           + strerror(errcode));
                childExit(ChildErrcode::OPENSTDERR);
            }
        }
        else
        {
            err = out;
            logger.log("stderr skip same as out");
        }
    }
    else
    {
        logger.log("stderr skip default");
    }

    logger.flush();
    // if(dup2(fileno(in), fileno(stdin)) == -1
    //    || dup2(fileno(out), fileno(stdout)) == -1
    //    || dup2(fileno(err), fileno(stderr)) == -1)
    // {
    //     std::abort();
    // }

    char *vec[cfg.args.size() + 2];
    int   vecSize  = 0;
    vec[vecSize++] = const_cast<char *>(cfg.bin.c_str());
    for(const auto &s: cfg.args)
    {
        vec[vecSize++] = const_cast<char *>(s.c_str());
    }
    vec[vecSize++] = nullptr;
    execv(cfg.bin.c_str(), vec);
    logger.err("Failed to replace");
    std::abort();
    return;
}

void Excutable::inTimer()
{
    if(cfg.timeLimit > 0)
    {
        sleep(cfg.timeLimit);
        logger.log("Time out");
        killChild();
    }
    childExit(ChildErrcode::GOOD);
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
    if(timerPid != -1)
    {
        killTimer();
    }
    logger.log("Child process stoped");
    timer.stop();
    res.time       = timer.get();
    res.mem        = cgp.getMemUsage();
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
    return kill(timerPid, SIGKILL) == -1;
}
}  // namespace HengCore