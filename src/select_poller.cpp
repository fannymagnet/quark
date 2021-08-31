#include "select_poller.h"
#include "log.h"
#include <algorithm>
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
            case PollerEvent::PollerEventType::Add:
                DirectAddChannle(ev.ch);
                break;
            case PollerEvent::PollerEventType::Remove:
                DirectRemoveChannle(ev.ch);
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
            bool isListenner = channels_[i]->IsListenr();
            if (ev & EventRead || isListenner)
            {
                FD_SET(fd, &rfds);
                //Debug("add read fd:" + std::to_string(fd));
            }

            if (channels_[i]->WaitingSendBytes() > 0 && !isListenner)
                FD_SET(fd, &wfds);

            if (fd > max_fd)
                max_fd = fd;
        }

        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ms % 1000 * 1000;

        int retval = select(max_fd + 1, &rfds, &wfds, 0, &tv);

        std::cout << "select result: " << retval << std::endl;
        if (retval < 0)
        {
            return;
        }
        else if (retval > 0)
        {
            actChannels.clear();
            for (size_t i = 0; i < channels_.size(); i++)
            {
                auto ch = channels_[i];
                auto fd = ch->GetSocket();
                bool event = false;
                if (FD_ISSET(fd, &rfds) )
                {
                    event = true;
                    ch->SetEvent(EventRead);
                    FD_CLR(fd, &rfds);
                }
                if (FD_ISSET(fd, &wfds))
                {
                    event = true;
                    ch->SetEvent(EventWrite);
                    FD_CLR(fd, &wfds);
                }
                if (event)
                {
                    actChannels.emplace_back(ch);
                }
            }
        }
        else
        {
            return;
        }
    }

    bool SelectPoller::AddChannel(Channel *channel)
    {
        return events_.push(PollerEvent{ PollerEvent::PollerEventType::Add, channel});
    }

    bool SelectPoller::RemoveChannel(Channel *channel)
    {
        return events_.push(PollerEvent{ PollerEvent::PollerEventType::Remove, channel});
    }

    void SelectPoller::DirectAddChannle(Channel *ch)
    {
        auto iter = std::find(channels_.begin(), channels_.end(), ch);
        if (iter == channels_.end())
        {
            channels_.emplace_back(ch);
        }
    }
    void SelectPoller::DirectRemoveChannle(Channel *ch)
    {
        auto iter = std::find(channels_.begin(), channels_.end(), ch);
        if (iter != channels_.end())
        {
            channels_.erase(iter);
        }
    }
}