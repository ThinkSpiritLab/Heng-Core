#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>

namespace Timer
{

class Timer
{
    using Clock     = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint start_;
    TimePoint end_;

public:
    Timer & begin(void);
    Timer & stop(void);
    int64_t get(void);
};

class AutoTimer
{
    int64_t &output;
    Timer    timer;

public:
    AutoTimer(int64_t &output);
    ~AutoTimer();
};

}  // namespace Timer
#endif