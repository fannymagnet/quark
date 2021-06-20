#include "channel.h"
#include "easylogging++.h"

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

struct iovec* channel::get_write_vecs(uint32_t& nv)
{
    unsigned int nr_vecs = 2;
    for (size_t i = 0; i < 2; ++i)
    {
        write_vecs[i].iov_base = nullptr;
        write_vecs[i].iov_len = 0;
    }
    
    m_buff.get_writeable_buffer(write_vecs, nr_vecs);
    nv = nr_vecs;
    return write_vecs;
}

struct iovec* channel::get_read_vecs( uint32_t& nv)
{
    unsigned int nr_vecs = 2;
    for (size_t i = 0; i < 2; ++i)
    {
        read_vecs[i].iov_base = nullptr;
        read_vecs[i].iov_len = 0;
    }

    m_buff.get_readable_buffer(read_vecs, nr_vecs);
    nv = nr_vecs;
    return read_vecs;
}