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
        struct io_uring_params params;
        memset(&params, 0, sizeof(params));
        params.sq_thread_idle = 200;
        int result = io_uring_queue_init_params(1024, &m_ring, &params);
        if (result < 0)
        {
            ////LOG(ERROR) << "init uring queue failed! error: " << -result;
            return false;
        }

        // check if IORING_FEAT_FAST_POLL is supported
        if (!(params.features & IORING_FEAT_FAST_POLL))
        {
            //LOG(ERROR) << "IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n";
            return false;
        }

        return true;
    }

    io_context::~io_context()
    {
        io_uring_queue_exit(&m_ring);
    }

    void io_context::runOnce()
    {
        struct io_uring_cqe *cqe;
        unsigned head;
        unsigned count = 0;
        // go through all CQEs
        io_uring_for_each_cqe(&m_ring, head, cqe)
        {
            ++count;
            Channel *req = (Channel *)io_uring_cqe_get_data(cqe);
            ;
            switch (req->CurrentEvent())
            {
            case EventAccept:
            {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int sock_conn_fd = cqe->res;
                // only read when there is no error, >= 0
                if (sock_conn_fd >= 0)
                {
                    Channel *chan = new Channel(sock_conn_fd);
                    m_channels.insert(std::make_pair((uint64_t)chan, chan));
                    m_SocketToChannels.insert(std::make_pair(sock_conn_fd, chan));
                    add_channel_read(chan);
                    //LOG(INFO) << "ACCEPT => " << req->GetSocket() << " Read new connection: " << sock_conn_fd;
                }
                add_channel_accept(req, (sockaddr *)&client_addr, &client_len);
                //LOG(INFO) << "ACCEPT => Accpet again";
                break;
            }
            case EventRead:
            {
                int bytes_read = cqe->res;
                if (cqe->res <= 0)
                {
                    // connection closed or error
                    shutdown(req->GetSocket(), SHUT_RDWR);
                    //LOG(INFO) << "socket disconnect: " << req->GetSocket() << " error: " << errno << " res: " << cqe->res;
                    m_channels.erase((uint64_t)req);
                    m_SocketToChannels.erase(req->GetSocket());
                    delete req;
                }
                else
                {
                    // todo: bytes have been read into bufs, now add write to socket sqe
                    //LOG(INFO) << "socket: " << req->GetSocket() << " read " << bytes_read << " bytes";
                    req->GetBuffer().add(bytes_read);
                    add_channel_write(req);
                }
                break;
            }
            case EventWrite:
            {
                if (cqe->res <= 0)
                {
                    //LOG(INFO) << "socket write to client error " << cqe->res;
                }
                else
                {
                    //LOG(INFO) << "socket write to client " << req->GetSocket() << " complete " << cqe->res;
                }
                req->GetBuffer().erase(cqe->res);
                add_channel_read(req);
                break;
            }
            default:
                break;
            }
        }
        /* Mark this request as processed */
        io_uring_cq_advance(&m_ring, count);
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
        struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
        //LOG(INFO) << "channel for socket " << chan->GetSocket() << " add accept";
        io_uring_prep_accept(sqe, chan->GetSocket(), client_addr, client_len, 0);
        chan->SetEvent(EventAccept);
        io_uring_sqe_set_data(sqe, chan);
        io_uring_submit(&m_ring);
    }

    void io_context::add_channel_read(Channel *chan)
    {
        struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
        uint32_t nr_vecs = 2;
        struct iovec *iovecs = chan->get_write_vecs(nr_vecs);
        //LOG(INFO) << "io_context add_channel_read => write iovec: " << nr_vecs;
        //LOG(INFO) << iovecs[0].iov_base << " : " << iovecs[0].iov_len;
        //LOG(INFO) << iovecs[1].iov_base << " : " << iovecs[1].iov_len;
        //LOG(INFO) << "io_context add_channel_read =========== ";
        chan->SetEvent(EventRead);
        io_uring_prep_readv(sqe, chan->GetSocket(), iovecs, nr_vecs, 0);
        io_uring_sqe_set_data(sqe, chan);
        io_uring_submit(&m_ring);
    }

    void io_context::add_channel_write(Channel *chan)
    {
        struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
        chan->SetEvent(EventWrite);
        //LOG(INFO) << "prepared to write to client :" << chan->GetSocket();
        uint32_t nr_vecs = 2;
        struct iovec *iovecs = chan->get_read_vecs(nr_vecs);
        io_uring_prep_writev(sqe, chan->GetSocket(), iovecs, nr_vecs, 0);
        io_uring_sqe_set_data(sqe, chan);
        io_uring_submit(&m_ring);
    }
}