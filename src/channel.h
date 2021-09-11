#pragma once

#include <iostream>
#include <array>

#include "ringbuffer.h"
#include "platforms/platform.h"


namespace quark
{
    enum EventType
    {
        EventIdle = 0,
        EventAccept = 2,
        EventRead = 4,
        EventWrite = 8,
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

        int GetSocket() { return m_rawfd; }
        uint16_t CurrentEvent();  
        void SetEvent(uint16_t t);
        void RemoveEvent(uint16_t t);

        inline RingBuffer &GetWriteBuffer() { return write_buff_; }
        inline RingBuffer &GetReadBuffer() { return read_buff_; }

        void Send();
        int Recieve();

        MultiIoBuf& get_write_vecs();
        MultiIoBuf& get_read_vecs();

        // channel 接收网络消息的缓冲区是否还有数据 
        bool CanRead();
        bool IsListenr() {
            return is_listener_;
        }

        // channel 接收网络消息的缓冲区空间大小 
        uint32_t InBfferCapcity();

        // channel 网络消息的发送缓冲区空间大小 
        uint32_t OutBufferCapcity();

        // channel 网络消息的发送缓冲区是否还有未发送数据 
        uint32_t WaitingSendBytes();

    private:
        /* data */
        int m_rawfd;
        bool is_listener_;
        EventInfo m_state;
        //todo: add two ring buffer for write and read
        uint16_t m_dataCount = 0;
        RingBuffer write_buff_;
        RingBuffer read_buff_;

        MultiIoBuf read_vecs;
        MultiIoBuf write_vecs;
    };

}