#pragma once
#include <array>

enum EventType {
    EventIdle = 0,
    EventAccept = 2,
    EventRead = 4,
    EventWrite = 8,
};

struct EventInfo {
    int fd;
    EventType type;
    uint16_t size;
};

const int MaxBufferSize = 64000;

class channel
{
public:
    channel(/* args */);
    ~channel();

    int GetSocket() { return m_rawfd;}
    EventType CurrentEvent() {return m_state.type;}

    uint8_t* GetReadBuffer() {
        return m_read_buff.data();
    }
private:
    /* data */
    int m_rawfd;
    EventType m_event;
    EventInfo m_state;
    //todo: add two ring buffer for write and read
    std::array<uint8_t, MaxBufferSize> m_read_buff;
    std::array<uint8_t, MaxBufferSize> m_write_buff;
};

channel::channel(/* args */)
{
    m_state.size = 0;
    m_state.type = EventIdle;
    m_state.fd = m_rawfd;
}

channel::~channel()
{
    /*
    if (nullptr != m_state) {
        delete m_state;
        m_state = nullptr;
    }
    */
}
