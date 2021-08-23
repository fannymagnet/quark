#pragma once

#include <iostream>
#include <array>
#include <sys/uio.h>

#include "ringbuffer.h"


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
        void SetEvent(uint16_t t) { m_state.type = t; }

        inline RingBuffer &GetWriteBuffer() { return write_buff_; }
        inline RingBuffer &GetReadBuffer() { return read_buff_; }

        struct iovec *get_write_vecs(uint32_t &nv);
        struct iovec *get_read_vecs(uint32_t &nv);

        // channel 接收网络消息的缓冲区是否还有数据 
        bool CanRead();

        // channel 接收网络消息的缓冲区空间大小 
        uint32_t InBfferCapcity();

        // channel 网络消息的发送缓冲区空间大小 
        uint32_t OutBufferCapcity();

        // channel 网络消息的发送缓冲区是否还有未发送数据 
        uint32_t WaitingSendBytes();

    private:
        /* data */
        int m_rawfd;
        EventInfo m_state;
        //todo: add two ring buffer for write and read
        uint16_t m_dataCount = 0;
        RingBuffer write_buff_;
        RingBuffer read_buff_;

        struct iovec read_vecs[2];
        struct iovec write_vecs[2];
        //std::array<uint8_t, MaxBufferSize> m_write_buff;
    };

}