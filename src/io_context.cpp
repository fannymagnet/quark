#include "io_context.h"
//#include "easylogging++.h"
//#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <memory.h>
#include "log.h"

//using namespace el::base::debug;
//using namespace el::base::type;

namespace quark
{
    io_context::io_context(/* args */)
    {
    }

    bool io_context::init()
    {
        return true;
    }

    io_context::~io_context()
    {
    }

    void io_context::runOnce()
    {
        uint8_t buf[1024] = {};
        std::list<Channel *> actChannels;
        poller.Update(1000, actChannels);
        struct sockaddr addr;
        socklen_t addr_len;
        for (auto ch : actChannels)
        {
            if (nullptr == ch)
                continue;

            //Debug(LOCATION, "socket", ch->GetSocket(), " is Listenner: ", ch->IsListenr());
            if (ch->IsListenr())
            {
                if ((ch->CurrentEvent() & EventRead) > 0)
                {
                    int ret = accept(ch->GetSocket(), &addr, &addr_len);
                    if (ret > 0)
                    {
                        Debug(LOCATION, "new socket: ", ret, " connected!");
                        Channel *chan = new Channel(ret);
                        poller.AddChannel(chan);
                    }
                }
            }
            else
            {
                if ((ch->CurrentEvent() & EventWrite) > 0 && ch->WaitingSendBytes() > 0)
                {
                    // send data
                    ch->Send();
                }

                if ((ch->CurrentEvent() & EventRead) > 0 && ch->InBfferCapcity() > 0)
                {
                    // decode msg
                    auto bytes = ch->Recieve();
                    if (bytes < 0)
                    {
                        //Debug(LOCATION, "ERROR: socket: " , ch->GetSocket() , " recv " , bytes , " bytes!  errno: " ,errno );
                        continue;
                    }
                    else if (bytes == 0)
                    {
                        poller.RemoveChannel(ch);
                        Debug(LOCATION, "INFO: socket: " , ch->GetSocket() , " disconnected!" );
                        ::close(ch->GetSocket());
                    }
                }
                // echo
                if (ch->CanRead())
                {
                    while (ch->WaitingSendBytes() > 0)
                    {
                        // send data
                        ch->Send();
                    }
                    auto count = ch->GetReadBuffer().get(buf, 1024);
                    ch->GetWriteBuffer().put(buf, count);
                }

                if (ch->WaitingSendBytes() <= 0)
                {
                    ch->RemoveEvent(EventWrite);
                }
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
        std::cout << "add accept socket: " << sock << std::endl;
        Channel *chan = new Channel(sock);
        chan->SetEvent(EventAccept);
        m_channels.insert(std::make_pair((uint64_t)chan, chan));
        m_SocketToChannels.insert(std::make_pair(sock, chan));
        poller.AddChannel(chan);
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