#ifndef RESULT_HPP
#define RESULT_HPP
#include "json.hpp"
namespace HengCore
{
namespace Result
{
    struct Result
    {
        long long mem;
        long long time;
        int       returnCode;
    };
    void to_json(nlohmann::json &j, const Result &res);
    void from_json(const nlohmann::json &j, Result &res);
}  // namespace Result
}  // namespace HengCore
#endif