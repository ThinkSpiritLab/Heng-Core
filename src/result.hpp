#ifndef RESULT_HPP
#define RESULT_HPP
#include "json.hpp"

namespace HengCore
{
namespace Result
{
    struct Result
    {
        long long memory;
        struct TimeUsage
        {
            long long sys;
            long long usr;
            long long real;
        } time;
        int returnCode;
        int signal;
    };
    void to_json(nlohmann::json &j, const Result &res);
    void from_json(const nlohmann::json &j, Result &res);
    void to_json(nlohmann::json &         j,
                 const Result::TimeUsage &time);
    void from_json(const nlohmann::json &j,
                   Result::TimeUsage &   time);
}  // namespace Result
}  // namespace HengCore
#endif