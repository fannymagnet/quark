#include "channel.h"

channel::channel(int fd)
{
    m_rawfd = fd;
    m_state.size = 0;
    m_state.type = EventIdle;
    m_state.fd = m_rawfd;
    m_buff.fill('\0');
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