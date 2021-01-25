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

#include "hc.child.hpp"
#include "hc.hpp"
#include "result.hpp"

namespace HengCore
{

int FileHandler::getfd() const
{
    if(fp != NULL)
    {
        return fileno(fp);
    }
    else
    {
        throw std::invalid_argument(
          "fp is NULL , maybe FILE doesn't exist");
    }
}

const FileHandler &FileHandler::setTo(FILE *lop) const
{
    if(lop != fp && lop != NULL)
    {
        if(dup2(fileno(fp), fileno(lop)) == -1)
        {
            throw std::runtime_error(strerror(errno));
        }
    }
    return *this;
}

FileHandler::FileHandler(const std::string fileName,
                         const std::string mod) noexcept
{
    if(fileName == Config::Config::defaultStdin
       && mod == "r")
    {
        fp = stdin;
    }
    else if(fileName == Config::Config::defaultStderr
            && mod == "w")
    {
        fp = stderr;
    }
    else if(fileName == Config::Config::defaultStdout
            && mod == "w")
    {
        fp = stdout;
    }
    else
    {
        fp = fopen(fileName.c_str(), mod.c_str());
    }
}

FileHandler::~FileHandler() noexcept
{
    if(fp != NULL)
    {
        fclose(fp);
    }
}

void Excutable::childExit(ChildErrcode returnCode)
{
    std::exit(static_cast<int>(returnCode));
}

void Excutable::inChild()
{
    logger.log("In child process , pid "
               + std::to_string(getpid()));
    gid_t curGid = getgid();
    uid_t curUid = getuid();
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

    logger.flush();

    {
        FileHandler in(cfg.stdin, "r");
        in.setTo(stdin);
        FileHandler out(cfg.stdout, "w");
        out.setTo(stdout);
        FileHandler err(cfg.stderr, "w");
        err.setTo(stderr);

        char *vec[cfg.args.size() + 2];
        int   vecSize = 0;
        vec[vecSize++] =
          const_cast<char *>(cfg.bin.c_str());
        for(const auto &s: cfg.args)
        {
            vec[vecSize++] = const_cast<char *>(s.c_str());
        }
        vec[vecSize++] = nullptr;
        execv(cfg.bin.c_str(), vec);
        logger.err("Failed to replace");
        // std::abort();
        childExit(ChildErrcode::EXEC);
        return;
    }
}

void Excutable::inTimer()
{
    close(timerPipe[0]);
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