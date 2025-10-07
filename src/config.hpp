#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <functional>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "json.hpp"

namespace HengCore
{
namespace Config
{
    struct Config
    {
        long long   timeLimit  = -1;
        long long   memLimit   = -1;
        uid_t       uid        = 0;
        gid_t       gid        = 0;
        long long   maxPid     = -1;
        long long   maxCpu     = -1;
        bool        isRoot     = getuid() == 0;
        std::string stdinPath  = Config::defaultStdin;
        std::string stdoutPath = Config::defaultStdout;
        std::string stderrPath = Config::defaultStderr;
        int         outFd      = -1;
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
namespace std
{
template <>
struct hash<HengCore::Config::Config>
{
public:
    std::size_t operator()(const HengCore::Config::Config
                             &config) const noexcept;
};
}  // namespace std
#endif
