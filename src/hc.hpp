#ifndef HC_HPP
#define HC_HPP

#include "cgroup.hpp"
#include "config.hpp"
#include "result.hpp"
#include "timer.hpp"
namespace HengCore
{
class Excutable
{
private:
    Timer::Timer      timer;
    Config::Config cfg;
    Cgroup::Cgroup cgp;
    Logger::Logger logger;
    pid_t          childPid = -1;
    pid_t          timerPid = -1;

public:
    Excutable(const Config::Config &cfg);
    bool           exec();
    bool           killChild();
    bool           killTimer();
    Result::Result getResult();
};
}  // namespace HengCore
#endif