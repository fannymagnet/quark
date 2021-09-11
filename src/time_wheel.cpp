#include "time_wheel.h"

namespace quark
{
    TimerWheel::TimerWheel() {}
    TimerWheel::~TimerWheel() {}

    bool TimerWheel::AddTimer(TimerNode *timer)
    {
        uint64_t delta = 0;
        uint64_t tick = timer->GetTickTime();

        if (current_time_ < tick)
        {
            delta = tick - current_time_;
        }

        int wheel_index = 0;
        int wheel_scale = 0;
        uint64_t dist = delta;
        while (dist > 63)
        {
            dist >>= 6;
            ++wheel_index;
        }

        return true;
    }

    void TimerWheel::Update()
    {
    }
} // namespace quark