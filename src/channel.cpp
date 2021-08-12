#include "channel.h"
//#include "easylogging++.h"
//using namespace el::base::debug;
//using namespace el::base::type;

namespace quark
{
    Channel::Channel(int fd)
    {
        m_rawfd = fd;
        m_state.size = 0;
        m_state.type = EventIdle;
        m_state.fd = m_rawfd;
    }

    Channel::~Channel()
    {
        /*
    if (nullptr != m_state) {
        delete m_state;
        m_state = nullptr;
    }
    */
    }
    uint16_t Channel::CurrentEvent()
    {
        bool hasAccpet = (m_state.type & EventAccept) ? true : false;
        if (InBfferCapcity() > 0)
        {
            m_state.type |= EventRead;
        }

        if (WaitingSendBytes() > 0)
        {
            m_state.type |= EventWrite;
        }
        if (hasAccpet)
        {
            m_state |= EventAccept;
        }
        return m_state.type;
    }
    struct iovec *Channel::get_write_vecs(uint32_t &nv)
    {
        unsigned int nr_vecs = 2;
        for (size_t i = 0; i < 2; ++i)
        {
            write_vecs[i].iov_base = nullptr;
            write_vecs[i].iov_len = 0;
        }

        write_buff_.get_writeable_buffer(write_vecs, nr_vecs);
        nv = nr_vecs;
        return write_vecs;
    }

    struct iovec *Channel::get_read_vecs(uint32_t &nv)
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

    bool Channel::CanRead()
    {
        return read_buff_.readable_bytes() > 0;
    }

    uint32_t Channel::InBfferCapcity()
    {
        return read_buff_.writeable_bytes();
    }

    uint32_t Channel::OutBufferCapcity()
    {
        return write_buff_.writeable_bytes();
    }

    uint32_t Channel::WaitingSendBytes()
    {
        return write_buff_.readable_bytes();
    }
} // namespace quark