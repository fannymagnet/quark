#include "io_context.h"
#include <iostream>
#include <memory.h>
#include <arpa/inet.h>
#include "easylogging++.h"

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
        LOG(ERROR) << "init uring queue failed! error: " << -result << std::endl;
        return false;
    }

    // check if IORING_FEAT_FAST_POLL is supported
    if (!(params.features & IORING_FEAT_FAST_POLL))
    {
        LOG(ERROR) << "IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n";
        return false;
    }
    return true;
}

io_context::~io_context()
{
}

void io_context::runOnce()
{
    struct io_uring_cqe *cqe;
    unsigned head;
    unsigned count = 0;
    // go through all CQEs
    io_uring_for_each_cqe(&m_ring, head, cqe) {
        ++count;
        channel *req = (channel *) cqe->user_data;
        switch (req->CurrentEvent()) 
        {
            case EventAccept: 
            {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int sock_conn_fd = cqe->res;
                // only read when there is no error, >= 0
                if (sock_conn_fd >= 0) {
                    channel* chan = new channel(sock_conn_fd);
                    m_channels.insert(std::make_pair((uint64_t)chan, chan));
                    add_channel_read(chan);
                    LOG(INFO) << "ACCEPT => " << req->GetSocket() << " Read new connection: " << sock_conn_fd;
                }
                add_channel_accept(req, (sockaddr *)&client_addr, &client_len);
                LOG(INFO) << "ACCEPT => Accpet again";
                break;
            }
            case EventRead: 
            {
                int bytes_read = cqe->res;
                if (cqe->res <= 0) {
                    // connection closed or error
                    shutdown(req->GetSocket(), SHUT_RDWR);
                    LOG(INFO) << "socket disconnect: " << req->GetSocket();
                    m_channels.erase((uint64_t)req);
                    delete req;
                } else {
                    // todo: bytes have been read into bufs, now add write to socket sqe
                    LOG(INFO) << "socket: " << req->GetSocket() << " read " << bytes_read << " bytes";
                    req->SetDataCount(bytes_read);
                    add_channel_write(req);
                }
                break;
            }
            case EventWrite:
            {
                req->SetDataCount(0);
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

void io_context::run() {
    while (true) {
        runOnce();
    }
}

void io_context::add_accept(int sock) {
    channel* chan = new channel(sock);
    add_channel_accept(chan, nullptr, nullptr);
}

void io_context::add_channel_accept(channel* chan, sockaddr *client_addr, socklen_t* client_len ) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
    LOG(INFO) << "channel for socket " << chan->GetSocket() << " add accept";
    io_uring_prep_accept(sqe, chan->GetSocket(), client_addr, client_len, 0);
    chan->SetEvent(EventAccept);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&m_ring);
}

void io_context::add_channel_read(channel* chan) 
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
    auto& buf = chan->GetBuffer();
    io_uring_prep_read(sqe, chan->GetSocket(), buf.data(), chan->GetDataCount(), 0);
    chan->SetEvent(EventRead);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&m_ring);
}

void io_context::add_channel_write(channel* chan) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
    chan->SetEvent(EventWrite);
    auto& buf = chan->GetBuffer();
    LOG(INFO) << "prepared to write " << chan->GetDataCount() << " to client :" << chan->GetSocket();
    io_uring_prep_write(sqe, chan->GetSocket(), buf.data(), chan->GetDataCount(), 0);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&m_ring);
}