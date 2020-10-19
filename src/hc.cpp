#include <exception>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include "hc.hpp"

namespace HengCore
{
Excutable::Excutable(const Config::Config &cfg): cfg(cfg) {}

bool Excutable::exec()
{
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
            return true;
        }
    }
}

}  // namespace HengCore