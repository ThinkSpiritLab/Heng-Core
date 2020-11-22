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
#include "hc.child.hpp"
#include "result.hpp"

namespace HengCore
{

void Excutable::childExit(
  ChildErrcode returnCode)
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
    if(in != nullptr
       && dup2(fileno(in), fileno(stdin)) == -1)
    {
        int errcode = errno;
        logger.err("Failed to replace " + cfg.stdin
                   + "(in) because " + strerror(errcode));
        childExit(ChildErrcode::REPLACESTDIN);
    }
    if(out != nullptr
       && dup2(fileno(out), fileno(stdout)) == -1)
    {
        int errcode = errno;
        logger.err("Failed to replace " + cfg.stdout
                   + "(out) because " + strerror(errcode));
        childExit(ChildErrcode::REPLACESTDOUT);
    }
    if(err != nullptr
       && dup2(fileno(err), fileno(stderr)) == -1)
    {
        int errcode = errno;
        logger.err("Failed to replace " + cfg.stderr
                   + "(err) because " + strerror(errcode));
        childExit(ChildErrcode::REPLACESTDERR);
    }

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
        write(timerPipe[1], "\1\0", 2);
        logger.log("Time out");
        killChild();
    }
    childExit(ChildErrcode::TIMERGOOD);
}


}  // namespace HengCore