#ifndef HC_HPP
#define HC_HPP

#include "cgroup.hpp"
#include "config.hpp"
#include "result.hpp"
#include "timer.hpp"

namespace HengCore
{

class FileHandler
{
public:
    FileHandler(const std::string fileName,
                const std::string mod) noexcept;
    ~FileHandler() noexcept;
    FILE *             fp;
    int                getfd() const;
    const FileHandler &setTo(FILE *lop) const;
};

const FileHandler
getFileHandler(const std::string fileName);

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
    SETCWD        = -11,
    CLOSEFD       = -12,
    EXEC          = -13

};
class Excutable
{
private:
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
    void           handleGid();
    void           handleUid();

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