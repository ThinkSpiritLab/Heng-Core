#ifndef LOGGER_HPP
#define LOGGER_HPP

// #define LOGGER_SOURCE_LOCATION_ENABLE

#ifdef LOGGER_SOURCE_LOCATION_ENABLE
#    include <source_location>
#endif
#include <string>

namespace HengCore
{
namespace Logger
{

    class Logger
    {
    private:
        std::string name;

    public:
        enum class Level
        {
            log,
            out,
            err
        };
        Logger(std::string name);
        Logger &log(Level l, std::string msg);
        Logger &log(std::string msg);
        Logger &out(std::string msg);
        Logger &err(std::string msg);
    };
}  // namespace Logger
}  // namespace HengCore

#endif