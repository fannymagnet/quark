#pragma once

#include <functional>

namespace quark
{
    class Poller
    {
    public:
        Poller() {}
        virtual ~Poller() {}
        virtual void RunInPoller(std::function<void()> func) = 0;
        virtual void Update(uint32_t ms) = 0;
    };
}