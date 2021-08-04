#pragma once

#include <vector>

#include "channel.h"
#include "poller.h"
#include "safe_queue.h"

namespace quark {
    class SelectPoller : public Poller
    {
    public:
        SelectPoller(/* args */);
        virtual ~SelectPoller();

        virtual void RunInPoller(std::function<void()> func);
        virtual void Update(uint32_t ms);
    private:
        SafeQueue<std::function<void()>> tasks_;
        std::vector<Channel*> channels_;
    };
}