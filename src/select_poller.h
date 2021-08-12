#pragma once

#include <vector>

#include "channel.h"
#include "poller.h"
#include "safe_queue.h"

namespace quark {
    enum PollerEventType {
        Add,
        Remove,
    }

    struct PollerEvent {
        PollerEventType t; 
        Channel* ch;
    }; 

    class SelectPoller : public Poller
    {
    public:
        SelectPoller(/* args */);
        virtual ~SelectPoller();

        virtual void RunInPoller(std::function<void()> func);
        virtual void Update(uint32_t ms, std::list<Channel*>& actChannels);

        virtual bool AddChannel(Channel*);
        virtual bool RemoveChannel(Channel*);
    private:
        void DirectAddChannle(Channel* ch);
        void DirectRemoveChannle(Channel* ch);
    private:
        SafeQueue<std::function<void()>> tasks_;
        SafeQueue<PollerEvent> events_;
        std::vector<Channel*> channels_;
    };
}