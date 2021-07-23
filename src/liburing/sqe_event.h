#pragma once

#include "liburing.h"
#include "utils.h"

namespace quark
{

class SqeEvent
{
private:
    /* data */
    ::io_uring_sqe* sqe_ = nullptr;
public:
    SqeEvent(NotNull<::io_uring_sqe*> sqe) : sqe_(sqe.get()) {}
    ~SqeEvent() {

    }
};
} // namespace quark