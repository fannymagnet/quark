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
    channel(int fd);
    ~channel();

    int GetSocket() { return m_rawfd;}
    EventType CurrentEvent() {return m_state.type;}
    void SetEvent(EventType t) { m_state.type = t;}

    std::array<uint8_t, MaxBufferSize>& GetBuffer() {
        return m_buff;
    }

    void SetDataCount(uint16_t count) {
        m_dataCount = count;
    }

    uint16_t GetDataCount() { return m_dataCount; }
private:
    /* data */
    int m_rawfd;
    EventType m_event;
    EventInfo m_state;
    //todo: add two ring buffer for write and read
    uint16_t m_dataCount = 0;
    std::array<uint8_t, MaxBufferSize> m_buff;

    std::array<uint8_t, MaxBufferSize> m_write_buff;
};

channel::channel(int fd)
{
    m_rawfd = fd;
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
