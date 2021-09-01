#pragma once

#include "common.h"
#include "../noncopyable.h"
#include <sys/select.h>
#include <unistd.h>

namespace quark
{
    class PosixFdSet : NonCopyable
    {
    public:
        PosixFdSet(/* args */) { Reset(); }
        ~PosixFdSet() { Reset();}

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

        void Clear(socket_type fd)
        {
            FD_CLR(fd, &fd_set_);
        }

        fd_set* Raw(){
            return &fd_set_;
        }

    private:
        fd_set fd_set_;
    };
} // namespace quark