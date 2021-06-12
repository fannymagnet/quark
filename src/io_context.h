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

    void run();
    void runOnce();

    void add_accept(int sock);

private:
    void add_channel_accept(channel* chan, sockaddr *client_addr, socklen_t* client_len );
    void add_channel_read(channel* chan);
    void add_channel_write(channel* chan);
private:
    /* data */
    struct io_uring m_ring; 
    std::unordered_map<uint64_t, channel*> m_channels;
};
