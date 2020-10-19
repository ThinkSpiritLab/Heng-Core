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
    pid_t          childPid;

    bool killChild();

public:
    void           exec();
    Result::Result getResult() const;
};
}  // namespace HengCore
#endif