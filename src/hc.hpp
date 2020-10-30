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
    enum class ChildErrcode
    {
        GOOD       = 0,
        GIDFAIL    = -1,
        UIDFAIL    = -2,
        IOCONFILCT = -3,
        OPENSTDIN  = -4,
        OPENSTDOUT = -5,
        OPENSTDERR = -6,
        SETCWD     = -7

    };
    Timer::Timer   timer;
    Config::Config cfg;
    Cgroup::Cgroup cgp;
    Logger::Logger logger;
    int            returnCode;
    int            sign;
    pid_t          childPid = -1;
    pid_t          timerPid = -1;
    void           inChild();
    void           inTimer();
    void           childExit(ChildErrcode returnCode);

public:
    Excutable(const Config::Config &cfg);
    bool           exec();
    bool           waitChild();
    bool           killChild();
    bool           killTimer();
    Result::Result getResult();
};
}  // namespace HengCore
#endif