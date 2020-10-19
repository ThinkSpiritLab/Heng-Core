#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>

#include "json.hpp"
#include <sys/types.h>
#include <unistd.h>
namespace HengCore
{
namespace Config
{
    struct Config
    {
        long long                timeLimit   = -1;
        long long                memLimit    = -1;
        long long                outPutLimit = -1;
        long long                uid         = 1;
        long long                gid         = 1;
        long long                maxPid      = -1;
        bool                     isRoot = getuid() == 0;
        std::string              stdout = "/dev/null";
        std::string              stderr = "/dev/null";
        std::vector<std::string> args;
        std::string              bin = "";
    };
    void to_json(nlohmann::json &j, const Config &cfg);
    void from_json(const nlohmann::json &j, Config &cfg);
}  // namespace Config
}  // namespace HengCore
#endif