#include "io_context.h"
//#include "easylogging++.h"
#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <memory.h>

//using namespace el::base::debug;
//using namespace el::base::type;

namespace quark
{
    io_context::io_context(/* args */)
    {
    }

    bool io_context::init()
    {
        // initialize io_uring
        //struct io_uring_params params;
        //memset(&params, 0, sizeof(params));
        //params.sq_thread_idle = 200;
        //int result = io_uring_queue_init_params(1024, &m_ring, &params);
        //if (result < 0)
        //{
        //    ////LOG(ERROR) << "init uring queue failed! error: " << -result;
        //    return false;
        //}

        //// check if IORING_FEAT_FAST_POLL is supported
        //if (!(params.features & IORING_FEAT_FAST_POLL))
        //{
        //    //LOG(ERROR) << "IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n";
        //    return false;
        //}

        return true;
    }

    io_context::~io_context()
    {
        //io_uring_queue_exit(&m_ring);
    }

    void io_context::runOnce()
    {
        uint8_t buf[1024] = {};
        std::list<Channel *> actChannels;
        poller.Update(1, actChannels);
        for (auto ch : actChannels)
        {
            if (nullptr == ch)
                continue;
            if (ch->WaitingSendBytes() > 0) {
                // send data
                uint32_t nv = 0;
                iovec* vec = ch->get_write_vecs(nv);
                auto bytes = writev(ch->GetSocket(), vec, nv);
                ch->GetWriteBuffer().erase(bytes);
            }

            if (ch->InBfferCapcity() > 0) {
                // decode msg
                uint32_t nv = 0;
                iovec* vec = ch->get_read_vecs(nv);
                auto bytes = readv(ch->GetSocket(), vec, nv);
                ch->GetReadBuffer().add(bytes);
            }
            // echo 
            if (ch->CanRead()) {
                while (ch->WaitingSendBytes() > 0) {
                    // send data
                    uint32_t nv = 0;
                    iovec* vec = ch->get_write_vecs(nv);
                    auto bytes = writev(ch->GetSocket(), vec, nv);
                    ch->GetWriteBuffer().erase(bytes);
                }
                auto count = ch->GetReadBuffer().get(buf, 1024);
                ch->GetWriteBuffer().put(buf, count);
            }
        }
    }

    void io_context::run()
    {
        while (true)
        {
            runOnce();
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void io_context::add_accept(int sock)
    {
        Channel *chan = new Channel(sock);
        m_channels.insert(std::make_pair((uint64_t)chan, chan));
        m_SocketToChannels.insert(std::make_pair(sock, chan));
        add_channel_accept(chan, nullptr, nullptr);
    }

    void io_context::add_channel_accept(Channel *chan, sockaddr *client_addr, socklen_t *client_len)
    {
        //        struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
        //        //LOG(INFO) << "channel for socket " << chan->GetSocket() << " add accept";
        //        io_uring_prep_accept(sqe, chan->GetSocket(), client_addr, client_len, 0);
        //        chan->SetEvent(EventAccept);
        //        io_uring_sqe_set_data(sqe, chan);
        //        io_uring_submit(&m_ring);
    }

    void io_context::add_channel_read(Channel *chan)
    {
        //        struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
        //        uint32_t nr_vecs = 2;
        //        struct iovec *iovecs = chan->get_write_vecs(nr_vecs);
        //        //LOG(INFO) << "io_context add_channel_read => write iovec: " << nr_vecs;
        //        //LOG(INFO) << iovecs[0].iov_base << " : " << iovecs[0].iov_len;
        //        //LOG(INFO) << iovecs[1].iov_base << " : " << iovecs[1].iov_len;
        //        //LOG(INFO) << "io_context add_channel_read =========== ";
        //        chan->SetEvent(EventRead);
        //        io_uring_prep_readv(sqe, chan->GetSocket(), iovecs, nr_vecs, 0);
        //        io_uring_sqe_set_data(sqe, chan);
        //        io_uring_submit(&m_ring);
    }

    void io_context::add_channel_write(Channel *chan)
    {
        //struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
        //chan->SetEvent(EventWrite);
        ////LOG(INFO) << "prepared to write to client :" << chan->GetSocket();
        //uint32_t nr_vecs = 2;
        //struct iovec *iovecs = chan->get_read_vecs(nr_vecs);
        //io_uring_prep_writev(sqe, chan->GetSocket(), iovecs, nr_vecs, 0);
        //io_uring_sqe_set_data(sqe, chan);
        //io_uring_submit(&m_ring);
    }
}