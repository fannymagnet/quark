#pragma once
#include "channel.h"

#include <map>
#include <unordered_map>
#include "liburing.h"
#include "noncopyable.h"

class io_context : public NonCopyable
{
public:
    io_context(/* args */);
    ~io_context();

    bool init();

private:
    /* data */
    struct io_uring m_ring; 
    std::unordered_map<uint64_t, channel*> m_channels;
};
