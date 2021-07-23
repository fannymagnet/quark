#pragma once

#include "liburing.h"

namespace quark
{
    class IoUring
    {
    private:
        /* data */
        ::io_uring uring;
    public:
        IoUring(/* args */) noexcept {}
        ~IoUring() {}
    };
} // namespace quark
