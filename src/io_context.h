#pragma once
#include "channel.h"

#include <map>
#include <list>
#include <functional>
#include <unordered_map>

#ifdef WIN32
#include<ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#include "noncopyable.h"
#include "safe_queue.h"
#include "poller.h"
#include "select_poller.h"

namespace quark
{
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
        void add_channel_accept(Channel *chan, sockaddr *client_addr, socklen_t *client_len);
        void add_channel_read(Channel *chan);
        void add_channel_write(Channel *chan);

    private:
        /* data */
        SelectPoller poller;
        std::unordered_map<uint64_t, Channel *> m_channels;
        std::unordered_map<int, Channel *> m_SocketToChannels;

        SafeQueue<std::function<void()>> m_events;
    };

}