#include <exception>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "hc.hpp"
#include "result.hpp"

namespace HengCore
{
Excutable::Excutable(const Config::Config &cfg): cfg(cfg) {}

bool Excutable::exec()
{
    timer.begin();
    childPid = fork();
    if(childPid < 0)
    {
        // throw std::runtime_error("UnableToFork");
        return false;
    }
    else
    {
        if(childPid == 0)
        {
            // It's child process
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
            // execl(cfg.bin.c_str(), cfg.args.c_str());
            return false;
        }
        else
        {
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
                        killChild();
                    }
                }
            }

            return true;
        }
    }
}

Result::Result Excutable::getResult()
{
    Result::Result res;
    int            status;
    waitpid(childPid, &status, 0);
    timer.stop();
    res.time = timer.get();
    return res;
}

bool Excutable::killChild()
{
    return kill(timerPid, SIGKILL) == -1
           && kill(childPid, SIGKILL) == -1;
}

}  // namespace HengCore