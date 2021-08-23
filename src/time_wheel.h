#pragma once

#include <vector>
#include <list>

#include <timer.h>

class TimerWheel {
    using ScaleType = std::list<TimerNode*>;
    using WheelType = std::vector<ScaleType>;
    using WheelArray = std::vector<WheelType>;
public:
    TimerWheel(){}
    ~TimerWheel(){}

    bool AddTimer(TimerNode* timer);
    void Update();
private:
    uint32_t current_wheel_;
    uint32_t current_scale_;
    uint64_t current_time_;
    WheelArray wheels_; 
};