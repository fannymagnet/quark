#include "io_context.h"
//#include "easylogging++.h"
//#include <arpa/inet.h>
#include <chrono>
#include <iostream>
#include <memory.h>
#include "log.h"
#include "select_poller.h"

//using namespace el::base::debug;
//using namespace el::base::type;

namespace quark
{
    IoContext::IoContext(/* args */)
    {
        poller_ = new SelectPoller();
    }

    bool IoContext::Init()
    {
        return true;
    }

    IoContext::~IoContext()
    {
        if (nullptr != poller_)
        {
            delete poller_;
            poller_ = nullptr;
        }
    }

    void IoContext::RunOnce()
    {
        uint8_t buf[1024] = {};
        std::list<Channel *> actChannels;
        poller_->Update(1000, actChannels);
        struct sockaddr addr;
        socklen_t addr_len;
        for (auto ch : actChannels)
        {
            if (nullptr == ch)
                continue;

            //Debug(LOCATION, "socket", ch->GetSocket(), " is Listenner: ", ch->IsListenr());
            if ((ch->CurrentEvent() & EventRead) > 0)
            {
                ch->HandleRead();
                /*
                int ret = accept4(ch->GetSocket(), &addr, &addr_len, SOCK_NONBLOCK|SOCK_CLOEXEC);
                if (ret > 0)
                {
                    Debug(LOCATION, "new socket: ", ret, " connected!");
                    Channel *chan = new Channel(ret);
                    poller_->AddChannel(chan);
                }
                else 
                {
                    Debug(LOCATION, "socket", ch->GetSocket()," accpet error: ", errno);
                }
                */
            } 
            else if ((ch->CurrentEvent() & EventWrite) > 0)
            {
                ch->HandleWrite();
            }
            else 
            {
                ch->HandleError();
            }
/*
            if ((ch->CurrentEvent() & EventRead) > 0 && ch->InBfferCapcity() > 0)
            {
                // decode msg
                auto bytes = ch->Recieve();
                if (bytes < 0)
                {
                    Debug(LOCATION, "ERROR: socket: " , ch->GetSocket() , " recv " , bytes , " bytes!  errno: " ,errno );
                    continue;
                }
                else if (bytes == 0)
                {
                    poller_->RemoveChannel(ch);
                    Debug(LOCATION, "INFO: socket: ", ch->GetSocket(), " disconnected!");
#if defined(_WIN32) || defined(_WIN64)
                    closesocket(ch->GetSocket());
#else
                    ::close(ch->GetSocket());
#endif
                }
            }
            // echo
            if (ch->CanRead())
            {
                auto count = ch->GetReadBuffer().get(buf, 1024);
                ch->Write(buf, count);
            }

            if (ch->WaitingSendBytes() <= 0)
            {
                ch->RemoveEvent(EventWrite);
            }
            */
            
        }
    }

    void IoContext::Run()
    {
        while (true)
        {
            RunOnce();
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    Channel* IoContext::AddAccept(int sock)
    {
        std::cout << "add accept socket: " << sock << std::endl;
        Channel *chan = new Channel(sock);
        chan->SetEvent(EventRead);
        channels_.insert(std::make_pair((uint64_t)chan, chan));
        socket_to_channels_.insert(std::make_pair(sock, chan));
        poller_->AddChannel(chan);
        return chan;
    }

    void IoContext::AddChannel(Channel* chan)
    {
        std::cout << "add socket: " << chan->GetSocket() << std::endl;
        channels_.insert(std::make_pair((uint64_t)chan, chan));
        socket_to_channels_.insert(std::make_pair(chan->GetSocket(), chan));
        poller_->AddChannel(chan);
    }
}