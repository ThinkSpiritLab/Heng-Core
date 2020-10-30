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
        long long   timeLimit = -1;
        long long   memLimit  = -1;
        uid_t       uid       = 0;
        gid_t       gid       = 0;
        long long   maxPid    = -1;
        bool        isRoot    = getuid() == 0;
        std::string stdin     = Config::defaultStdin;
        std::string stdout    = Config::defaultStdout;
        std::string stderr    = Config::defaultStderr;
        std::string cwd;
        std::vector<std::string> args;
        std::string              bin;
        static const std::string defaultStdin;
        static const std::string defaultStdout;
        static const std::string defaultStderr;
    };
    void to_json(nlohmann::json &j, const Config &cfg);
    void from_json(const nlohmann::json &j, Config &cfg);
}  // namespace Config
}  // namespace HengCore
#endif