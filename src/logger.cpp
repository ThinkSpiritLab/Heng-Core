#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "logger.hpp"

#include "colourStream.h"

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

    const Logger &Logger::log(Logger::Level l,
                              std::string   msg) const
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

    const Logger &
    Logger::log([[maybe_unused]] std::string msg) const
    {
#ifdef LOG_LOG
        std::ostringstream ss;
        if(name.length() != 0)
        {

            ss << getpid() << "[" << name << "]";
        }
        ss << "Log:" << msg << std::endl;
        loggerMutex.lock();
        costream<std::clog>(cos::GREY) << ss.str();
        loggerMutex.unlock();
#endif
        return *this;
    }
    const Logger &
    Logger::out([[maybe_unused]] std::string msg) const
    {
#ifdef LOG_OUT
        std::ostringstream ss;
        if(name.length() != 0)
        {
            ss << getpid() << "[" << name << "]";
        }
        ss << "Out:" << msg << std::endl;
        loggerMutex.lock();
        costream<std::cout>(cos::WHITE) << ss.str();
        loggerMutex.unlock();
#endif
        return *this;
    }
    const Logger &
    Logger::err([[maybe_unused]] std::string msg) const
    {
#ifdef LOG_ERR
        std::ostringstream ss;
        if(name.length() != 0)
        {
            ss << getpid() << "[" << name << "]";
        }
        ss << "Err:" << msg << std::endl;
        loggerMutex.lock();
        costream<std::cerr>(cos::RED) << ss.str();
        loggerMutex.unlock();
#endif
        return *this;
    }
    const Logger &Logger::flush() const
    {
        std::clog << std::flush;
        std::cout << std::flush;
        std::cerr << std::flush;
        return *this;
    }
}  // namespace Logger
}  // namespace HengCore