#include "select_poller.h"
#ifdef WIN32
#else
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

    void SelectPoller::Update(uint32_t ms)
    {
        std::function<void()> handle = nullptr;
        while (tasks_.try_pop(handle))
        {
            handle();
        }

        fd_set rfds;
        fd_set wfds;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
        int retval;

        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

        int max_fd = 0;
        for (size_t i = 0; i < channels_.size(); i++)
        {
            int fd = channels_[i]->GetSocket();
            auto ev = channels_[i]->CurrentEvent();
            if (ev & EventRead)
            {
                FD_SET(fd, &rfds);
            }

            if (ev & EventWrite)
            {
                FD_SET(fd, &wfds);
            }

            if (fd > max_fd) {
                max_fd = fd;
            }
        }

        retval = select(max_fd + 1, &rfds, &wfds, 0, &tv);

        if (retval < 0)
            perror("select()");
        else if (retval > 0) {
            printf("Data is available now.\n");
            /* FD_ISSET(0, &rfds) will be true. */
        }
        else
            printf("No data within five seconds.\n");
    }
}