#ifndef HC_HPP
#define HC_HPP

#include "config.hpp"
#include "result.hpp"
namespace HengCore
{
class Excutable
{
private:
    Config::Config cfg;
    pid_t          childPid = -1;

    bool killChild();

public:
    Excutable(const Config::Config &cfg);
    bool           exec();
    Result::Result getResult() const;
};
}  // namespace HengCore
#endif