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

namespace quark
{
    class IoContext : public NonCopyable
    {
    public:
        IoContext(/* args */);
        ~IoContext();

        bool Init();
        void Run();
        void RunOnce();
        Channel* AddAccept(int sock);
        void AddChannel(Channel* chan);

    private:
        /* data */
        Poller* poller_;
        std::unordered_map<uint64_t, Channel *> channels_;
        std::unordered_map<int, Channel *> socket_to_channels_;
        SafeQueue<std::function<void()>> events_;
    };

}