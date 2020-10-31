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
        TIMERGOOD     = -1,
        GIDFAIL       = -2,
        UIDFAIL       = -3,
        OPENSTDIN     = -4,
        OPENSTDOUT    = -5,
        OPENSTDERR    = -6,
        IOCONFILCT    = -7,
        REPLACESTDIN  = -8,
        REPLACESTDOUT = -9,
        REPLACESTDERR = -10,
        SETCWD        = -11

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