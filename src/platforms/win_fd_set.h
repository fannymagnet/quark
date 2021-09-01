#pragma once
#include "common.h"
#include "../noncopyable.h"

namespace quark
{
#if defined(WIN32) || defined(WIN64)
    class WinFdSet : NonCopyable
    {
    public:
        WinFdSet(/* args */);
        ~WinFdSet();

        void Reset()
        {
            FD_ZERO(&fd_set_);
        }

        bool Set(socket_type fd)
        {
            FD_SET(fd, &fd_set_);
            return true;
        }

        bool IsSet(socket_type fd)
        {
            return FD_ISSET(fd, &fd_set_) != 0;
        }

        void IsSet(socket_type fd)
        {
            FD_CLR(fd, &fd_set_);
        }

        fd_set* Raw()
        {
            return &fd_set_;
        }

    private:
        fd_set fd_set_;
    };

    WinFdSet::WinFdSet(/* args */)
    {
        Reset();
    }

    WinFdSet::~WinFdSet()
    {
        Reset();
    }
#endif
} // namespace quark