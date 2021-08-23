#pragma once

#include "common.h"
#include "noncopyable.h"
#include <sys/select.h>
#include <unistd.h>

namespace quark
{
    class PosixFdSet : NonCopyable
    {
    public:
        PosixFdSet(/* args */);
        ~PosixFdSet();

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

    private:
        fd_set fd_set_;
    };

    PosixFdSet::PosixFdSet(/* args */)
    {
        Reset();
    }

    PosixFdSet::~PosixFdSet()
    {
        Reset();
    }

} // namespace quark