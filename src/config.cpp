#include "json.hpp"

#include "config.hpp"
namespace HengCore
{
namespace Config
{
    const std::string Config::defaultStdin  = "/dev/stdin";
    const std::string Config::defaultStdout = "/dev/stdout";
    const std::string Config::defaultStderr = "/dev/stderr";
    void to_json(nlohmann::json &j, const Config &cfg)
    {
        j = nlohmann::json(
          { { "timeLimit", cfg.timeLimit },
            { "memLimit", cfg.memLimit },
            // { "outPutLimit", cfg.outPutLimit },
            { "uid", cfg.uid },
            { "gid", cfg.gid },
            { "maxPid", cfg.maxPid },
            { "maxCpu", cfg.maxCpu },
            { "isRoot", cfg.isRoot },
            { "stdin", cfg.stdin },
            { "stdout", cfg.stdout },
            { "stderr", cfg.stderr },
            { "fd", cfg.outFd },
            { "cwd", cfg.cwd },
            { "args", cfg.args },
            { "bin", cfg.bin } });
    }
    void from_json(const nlohmann::json &j, Config &cfg)
    {
        j.at("timeLimit").get_to(cfg.timeLimit);
        j.at("memLimit").get_to(cfg.memLimit);
        // j.at("outPutLimit").get_to(cfg.outPutLimit);
        j.at("uid").get_to(cfg.uid);
        j.at("gid").get_to(cfg.gid);
        j.at("maxPid").get_to(cfg.maxPid);
        j.at("maxCpu").get_to(cfg.maxCpu);
        j.at("isRoot").get_to(cfg.isRoot);
        j.at("stdin").get_to(cfg.stdin);
        j.at("stdout").get_to(cfg.stdout);
        j.at("stderr").get_to(cfg.stderr);
        j.at("fd").get_to(cfg.outFd);
        j.at("cwd").get_to(cfg.cwd);
        j.at("args").get_to(cfg.args);
        j.at("bin").get_to(cfg.bin);
    }
}  // namespace Config
}  // namespace HengCore

namespace std
{
std::size_t hash<HengCore::Config::Config>::operator()(
  const HengCore::Config::Config &config) const noexcept
{
    return std::hash<std::string>()(nlohmann::json(config).dump());
}
}  // namespace std