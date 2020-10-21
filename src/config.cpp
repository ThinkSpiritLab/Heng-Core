#include "json.hpp"

#include "config.hpp"
namespace HengCore
{
namespace Config
{
    void to_json(nlohmann::json &j, const Config &cfg)
    {
        j = nlohmann::json(
          { { "timeLimit", cfg.timeLimit },
            { "memLimit", cfg.memLimit },
            // { "outPutLimit", cfg.outPutLimit },
            { "uid", cfg.uid },
            { "gid", cfg.gid },
            { "maxPid", cfg.maxPid },
            { "isRoot", cfg.isRoot },
            { "stdout", cfg.stdout },
            { "stderr", cfg.stderr },
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
        j.at("isRoot").get_to(cfg.isRoot);
        j.at("stdout").get_to(cfg.stdout);
        j.at("stderr").get_to(cfg.stderr);
        j.at("args").get_to(cfg.args);
        j.at("bin").get_to(cfg.bin);
    }
}  // namespace Config
}  // namespace HengCore