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
        long long timeLimit = -1;
        long long memLimit  = -1;
        // long long                outPutLimit = -1;
        uid_t                    uid    = 1;
        gid_t                    gid    = 1;
        long long                maxPid = -1;
        bool                     isRoot = getuid() == 0;
        std::string              stdin  = "/dev/stdin";
        std::string              stdout = "/dev/stdout";
        std::string              stderr = "/dev/stderr";
        std::vector<std::string> args;
        std::string              bin = "";
    };
    void to_json(nlohmann::json &j, const Config &cfg);
    void from_json(const nlohmann::json &j, Config &cfg);
}  // namespace Config
}  // namespace HengCore
#endif