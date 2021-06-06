#include "io_context.h"
#include <iostream>
#include <memory.h>

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

void io_context::add_channel_read(channel* chan) 
{
    struct io_uring_sqe *sqe = io_uring_get_sqe(&m_ring);
    io_uring_prep_read(sqe, chan->GetSocket(), chan->GetReadBuffer(), 64000, 0);
    io_uring_sqe_set_data(sqe, chan);
    io_uring_submit(&m_ring);
}