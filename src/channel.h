#pragma once

#include <iostream>
#include <functional>
#include <array>

#include "ringbuffer.h"
#include "platforms/platform.h"
#include "poller.h"

namespace quark
{
    enum EventType
    {
        EventIdle = 0,
        EventRead = 2,
        EventWrite = 4,
    };

    struct EventInfo
    {
        int fd;
        uint16_t type;
        uint16_t size;
    };

    const int MaxBufferSize = 64;

    class Channel
    {
    public:
        Channel(int fd);
        ~Channel();
        int GetSocket() { return rawfd_; }
        uint16_t CurrentEvent();  
        void SetEvent(uint16_t t);
        void RemoveEvent(uint16_t t);
        void SetPoller(Poller* poller);
        inline RingBuffer &GetWriteBuffer() { return write_buff_; }
        inline RingBuffer &GetReadBuffer() { return read_buff_; }
        void Write(uint8_t* data, uint32_t len);
        void Send();
        int Recieve();
        MultiIoBuf& GetWriteVecs();
        MultiIoBuf& GetReadVecs();
        // channel 接收网络消息的缓冲区是否还有数据 
        bool CanRead();
        // channel 接收网络消息的缓冲区空间大小 
        uint32_t InBfferCapcity();
        // channel 网络消息的发送缓冲区空间大小 
        uint32_t OutBufferCapcity();
        // channel 网络消息的发送缓冲区是否还有未发送数据 
        uint32_t WaitingSendBytes();

        void HandleRead();
        void HandleWrite();
        void HandleError();

        void SetRead(std::function<void()>&& handle) 
        {
            try 
            {
                Debug("Channel SetRead");
                read_handle_ = std::move(handle);
                Debug("Channel SetRead Finished");
            }
            catch(std::exception e)
            {
                Debug(e.what());
            }
        }

    private:
        /* data */
        int rawfd_;
        bool is_listener_;
        uint16_t event_;
        //todo: add two ring buffer for write and read
        uint16_t data_count_ = 0;
        RingBuffer write_buff_;
        RingBuffer read_buff_;
        MultiIoBuf read_vecs_;
        MultiIoBuf write_vecs_;
        Poller* poller_;

        std::function<void()> read_handle_;
    };

}