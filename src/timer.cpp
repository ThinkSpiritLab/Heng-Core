#include "timer.hpp"
#include <chrono>
namespace Timer
{

Timer &Timer::begin(void)
{
    this->start_ = Timer::Clock::now();
    return *this;
}
Timer &Timer::stop(void)
{
    this->end_ = Timer::Clock::now();
    return *this;
}
int64_t Timer::get(void)
{
    return std::chrono::duration_cast<
             std::chrono::nanoseconds>(end_ - start_)
      .count();
}

AutoTimer::AutoTimer(int64_t &output): output(output)
{
    timer.begin();
}

AutoTimer::~AutoTimer()
{
    timer.stop();
    output = timer.get();
}

}  // namespace Timer