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
        EventType type;
        uint16_t size;
    };

    const int MaxBufferSize = 64;

    class Channel
    {
    public:
        Channel(int fd);
        ~Channel();

        int GetSocket() { return m_rawfd; }
        EventType CurrentEvent() { return m_state.type; }
        void SetEvent(EventType t) { m_state.type = t; }

        RingBuffer &GetBuffer()
        {
            return m_buff;
        }

        struct iovec *get_write_vecs(uint32_t &nv);
        struct iovec *get_read_vecs(uint32_t &nv);

    private:
        /* data */
        int m_rawfd;
        EventType m_event;
        EventInfo m_state;
        //todo: add two ring buffer for write and read
        uint16_t m_dataCount = 0;
        RingBuffer m_buff;

        struct iovec read_vecs[2];
        struct iovec write_vecs[2];
        std::array<uint8_t, MaxBufferSize> m_write_buff;
    };

}