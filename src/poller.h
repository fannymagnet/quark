#pragma once

#include <functional>
#include <list>

namespace quark
{

    class Channel;
    class Poller
    {
    public:
        Poller() {}
        virtual ~Poller() {}
        virtual void RunInPoller(std::function<void()> func) = 0;
        virtual void Update(uint32_t ms, std::list<Channel*>& acts) = 0;
        virtual bool AddChannel(Channel*) = 0;
        virtual bool RemoveChannel(Channel*) = 0;
    };
}