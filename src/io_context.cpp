#include "io_context.h"
#include <iostream>
#include <memory.h>
#include <arpa/inet.h>

io_context::io_context(/* args */)
{
}

bool io_context::init()
{
    // initialize io_uring
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));
    int result = io_uring_queue_init_params(256, &m_ring, &params);
    if (result < 0)
    {
        std::cerr << "init uring queue failed! error: " << -result << std::endl;
        return false;
    }

    // check if IORING_FEAT_FAST_POLL is supported
    if (!(params.features & IORING_FEAT_FAST_POLL))
    {
        std::cerr << "IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n";
        return false;
    }

    return true;
}

io_context::~io_context()
{
}

void io_context::runOnce()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    struct io_uring_cqe *cqe;
    unsigned head;
    unsigned count = 0;
    // go through all CQEs
    socklen_t client_addr_len = sizeof(client_addr);
    io_uring_for_each_cqe(&m_ring, head, cqe) {
        ++count;
        channel *req = (channel *) cqe->user_data;
        switch (req->CurrentEvent()) 
        {
            case EventAccept: 
            {
                int sock_conn_fd = cqe->res;
                // only read when there is no error, >= 0
                if (sock_conn_fd >= 0) {
                    channel* chan = new channel(sock_conn_fd);
                    m_channels.insert(std::make_pair((uint64_t)chan, chan));
                    add_channel_read(chan);
                }
                add_channel_accpet(req, (sockaddr *)&client_addr, &client_len);
                break;
            }
            case EventRead: 
            {
                int bytes_read = cqe->res;
                if (cqe->res <= 0) {
                    // connection closed or error
                    shutdown(req->GetSocket(), SHUT_RDWR);
                } else {
                    // todo: bytes have been read into bufs, now add write to socket sqe
                }
                break;
            }
            case EventWrite:
            {
                break;
            }
            default:
                break;
        }
    }
    /* Mark this request as processed */
    io_uring_cq_advance(&m_ring, count);
}


void io_context::add_channel_accpet(channel* chan, sockaddr *client_addr, socklen_t* client_len ) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
    io_uring_prep_accept(sqe, chan->GetSocket(), client_addr, client_len, 0);
    chan->SetEvent(EventAccept);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&m_ring);
}

void io_context::add_channel_read(channel* chan) 
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
    auto& buf = chan->GetBuffer();
    io_uring_prep_read(sqe, chan->GetSocket(), buf.data(), 64000, 0);
    chan->SetEvent(EventRead);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&m_ring);
}

void io_context::add_channel_write(channel* chan) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
    chan->SetEvent(EventWrite);
    auto& buf = chan->GetBuffer();
    io_uring_prep_write(sqe, chan->GetSocket(), buf.data(), chan->GetDataCount(), 0);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&m_ring);
}