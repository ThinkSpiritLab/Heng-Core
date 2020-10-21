#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>
// #include <thread>
// #include <source_location>

#include "colourStream.h"

#include "logger.hpp"

namespace HengCore
{
namespace Logger
{
    template <std::ostream &os>
    using costream =
      kerbal::utility::costream::costream<os>;

    namespace cos = kerbal::utility::costream;

    std::mutex loggerMutex;

    Logger::Logger(std::string name): name(name) {}

    Logger &Logger::log(Logger::Level l, std::string msg)
    {
        switch(l)
        {
            case Level::log:
                return log(msg);
                break;
            case Level::out:
                return out(msg);
                break;
            case Level::err:
                return err(msg);
                break;
            default:
                return err(msg);
                break;
        }
    }

    Logger &Logger::log(std::string msg)
    {
        std::ostringstream ss;
        if(name.length() != 0)
        {

            ss << getpid() << "[" << name << "]";
        }
        ss << "Log:" << msg << std::endl;
        loggerMutex.lock();
        costream<std::clog>(cos::GREY) << ss.str();
        loggerMutex.unlock();
        return *this;
    }
    Logger &Logger::out(std::string msg)
    {
        std::ostringstream ss;
        if(name.length() != 0)
        {
            ss << getpid() << "[" << name << "]";
        }
        ss << "Out:" << msg << std::endl;
        loggerMutex.lock();
        costream<std::cout>(cos::WHITE) << ss.str();
        loggerMutex.unlock();
        return *this;
    }
    Logger &Logger::err(std::string msg)
    {
        std::ostringstream ss;
        if(name.length() != 0)
        {
            ss << getpid() << "[" << name << "]";
        }
        ss << "Err:" << msg << std::endl;
        loggerMutex.lock();
        costream<std::cerr>(cos::RED) << ss.str();
        loggerMutex.unlock();
        return *this;
    }
    Logger& Logger::flush()
    {
        std::clog << std::flush;
        std::cout << std::flush;
        std::cerr << std::flush;
        return *this;
    }
}  // namespace Logger
}  // namespace HengCore