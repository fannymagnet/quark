#pragma once

#include <functional>

#include "io_context.h"
#include "platforms/common.h"

class Channel;

namespace quark
{
    class Acceptor
    {
    public:
        typedef std::function<void(Channel*)> AccpetHandle;
        Acceptor(io_context *ctx, int max_conns);
        ~Acceptor();

        bool Open(int port);
        bool Accept();
        void AsyncAccept(AccpetHandle handle);
        void Close();

        bool SetNonBlock(bool open = true);
        bool SetReUse(bool open = true);

        socket_type GetSocket() { return sock_; }
    private:
        io_context* ctx_;
        socket_type sock_;
        int max_conns_;
    };
} // namespace quark