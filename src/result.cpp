#include "result.hpp"

#include "json.hpp"

namespace HengCore
{
namespace Result
{
    void to_json(nlohmann::json &j, const Result &res)
    {
        j =
          nlohmann::json({ { "mem", res.mem },
                           { "time", res.time },
                           { "returnCode", res.returnCode },
                           { "signal", res.signal } });
    }
    void from_json(const nlohmann::json &j, Result &res)
    {
        j.at("mem").get_to(res.mem);
        j.at("time").get_to(res.time);
        j.at("returnCode").get_to(res.returnCode);
        j.at("signal").get_to(res.signal);
    }

    void to_json(nlohmann::json &         j,
                 const Result::TimeUsage &time)
    {
        j = nlohmann::json({

          { "sys", time.sys },
          { "usr", time.usr },
          { "real", time.real }

        });
    }
    void from_json(const nlohmann::json &j,
                   Result::TimeUsage &   time)
    {
        j.at("sys").get_to(time.sys);
        j.at("usr").get_to(time.usr);
        j.at("real").get_to(time.real);
    }
}  // namespace Result
}  // namespace HengCore