#include "select_poller.h"
#ifdef WIN32
#else
#include <sys/select.h>
#endif

namespace quark
{
    SelectPoller::SelectPoller(/* args */)
    {
    }

    SelectPoller::~SelectPoller()
    {
    }

    void SelectPoller::RunInPoller(std::function<void()> func)
    {
        tasks_.push(func);
    }

    void SelectPoller::Update(uint32_t ms, std::list<Channel *> &actChannels)
    {
        PollerEvent ev;
        while (events_.try_pop(ev))
        {
            switch (ev.t)
            {
            case PollerEventType::Add:
                /* code */
                break;
            case PollerEventType::Remove:
                break;
            default:
                break;
            }
        }

        std::function<void()> handle = nullptr;
        while (tasks_.try_pop(handle))
        {
            handle();
        }

        fd_set rfds;
        fd_set wfds;

        FD_ZERO(&rfds);
        FD_ZERO(&wfds);

        int max_fd = 0;
        for (size_t i = 0; i < channels_.size(); i++)
        {
            int fd = channels_[i]->GetSocket();
            auto ev = channels_[i]->CurrentEvent();
            FD_SET(fd, &rfds);

            if (ev & EventWrite)
            {
                FD_SET(fd, &wfds);
            }

            if (fd > max_fd)
            {
                max_fd = fd;
            }
        }

        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ms & 1000 * 1000;

<<<<<<< HEAD
        if (retval < 0)
            perror("select()");
        else if (retval > 0) {
            printf("Data is available now.\n");
            /* FD_ISSET(0, &rfds) will be true. */
=======
        int retval = select(max_fd + 1, &rfds, &wfds, 0, &tv);
        if (retval == -1)
        {
            //perror("select()");
            return;
        }
        else if (retval)
        {
            actChannels.clear();
            //printf("Data is available now.\n");
            bool hasEvent = false;
            for (size_t i = 0; i < channels_.size(); i++)
            {
                auto ch = channels_[i];
                if (FD_ISSET(ch->GetSocket(), &rfds))
                {
                    ch->SetEvent(EventRead);
                    hasEvent = true;
                }

                if (FD_ISSET(ch->GetSocket(), &wfds))
                {
                    ch->SetEvent(EventWrite);
                    hasEvent = true;
                }
                if (hasEvent)
                {
                    actChannels.push_back(ch);
                    hasEvent = false;
                }
            }
>>>>>>> 956a4a4b606015a81c3b9f0106175e42a7f6fad7
        }
        else
        {
            //printf("No data within five seconds.\n");
            return;
        }
    }

    bool SelectPoller::AddChannel(Channel *channel)
    {
        return events_.push(PollerEvent{PollerEventType::Add, channel});
    }

    bool SelectPoller::RemoveChannel(Channel *channel)
    {
        return events_.push(PollerEvent{PollerEventType::Remove, channel});
    }

    void SelectPoller::DirectAddChannle(Channel* ch) {
    }
    void SelectPoller::DirectRemoveChannle(Channel* ch) {

    }
}