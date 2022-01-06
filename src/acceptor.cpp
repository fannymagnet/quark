#include "acceptor.h"
#include "log.h"
#include <cstring>

#if defined(_WIN32) || defined(_WIN64)
#else
#include <sys/ioctl.h>
#include <sys/utsname.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#endif

namespace quark
{
    Acceptor::Acceptor(IoContext *ctx, int max_conns) : ctx_(ctx), max_conns_(max_conns)
    {
    }

    Acceptor::~Acceptor()
    {
        Close();
    }

    bool Acceptor::Open(int port)
    {
#if defined(_WIN32) || defined(_WIN64)
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if (0 != WSAStartup(sockVersion, &wsaData))
    {
        return false;
    }
#endif
        sock_ = socket(PF_INET, SOCK_STREAM, 0);
        if (sock_ == -1)
        {
            return false;
        }

        if (!SetNonBlock())
        {
            Debug("SetNonBlock failed!!!");
            return false;
        }

        if (!SetReUse())
        {
            Close();
            return false;
        }

        struct sockaddr_in srv_addr;
        memset(&srv_addr, 0, sizeof(srv_addr));
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_port = htons(port);
        srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        /* We bind to a port and turn this socket into a listening
        * socket.
        * */
        int bind_result = 0;
#if defined(_WIN32) || defined(_WIN64)
        bind_result = bind(sock_,
                               (const struct sockaddr *)&srv_addr,
                               (int)sizeof(srv_addr));
#else
        bind_result = bind(sock_,
                               (const struct sockaddr *)&srv_addr,
                               sizeof(srv_addr));
#endif
        if (bind_result < 0)
        {

            Debug(LOCATION, "bind() => ", bind_result);
            return false;
        }

        return true;
    }
    bool Acceptor::Accept()
    {
        if (listen(sock_, max_conns_) < 0)
        {
            Debug("listen()");
            return false;
        }
        Debug("listen()");

        if (nullptr == ctx_)
        {
            //LOG(ERROR) << "io context is nullptr";
            return false;
        }

        channel_ = new Channel(sock_);
        channel_->SetEvent(EventRead);
        Debug("SetEvent()");
        auto read_handle = [&](){
            struct sockaddr addr;
            socklen_t addr_len;
#if defined(_WIN32) || defined(_WIN64)
            int ret = accept(sock_, &addr, &addr_len);
#else
            int ret = accept4(sock_, &addr, &addr_len, SOCK_NONBLOCK|SOCK_CLOEXEC);
#endif
            if (ret > 0)
            {
                Debug(LOCATION, "new socket: ", ret, " connected!");
                Channel *chan = new Channel(ret);
                ctx_->AddChannel(chan);
            }
            else 
            {
                Debug(LOCATION, "socket", sock_," accpet error: ", errno);
            }
        };
        Debug("create handle()");
        channel_->SetRead(std::move(read_handle));
        Debug("SetRead()");
        ctx_->AddChannel(channel_);
        Debug("AddChannel()");
        return true;
    }

    void Acceptor::AsyncAccept(AccpetHandle handle)
    {
    }

    void Acceptor::Close()
    {
        if (sock_ > 0)
        {
#if defined(_WIN32) || defined(_WIN64)
            closesocket(sock_);
            WSACleanup();
#else
            close(sock_);
#endif
            sock_ = 0;
        }
    }

    bool Acceptor::SetNonBlock(bool open)
    {
        int on = open ? 1 : 0;
        int rc = 0;
        #if defined(_WIN32) || defined(_WIN64)
        rc = ioctlsocket(sock_, FIONBIO, (unsigned long *)&on);
        #else
        rc = ioctl(sock_, FIONBIO, (char *)&on);
        #endif
        if (rc < 0)
        {
            Debug("ioctl() failed");
            return false;
        }

        return true;
    }

    bool Acceptor::SetReUse(bool open)
    {
        int enable = open ? 1 : 0;
        #if defined(_WIN32) || defined(_WIN64)
        if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char *)&enable, sizeof(int)) < 0)
        #else
        if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        #endif
        {
            return false;
        }
        return true;
    }
} // namespace quark