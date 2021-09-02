#include <fcntl.h>
#include <unistd.h>

#include "channel.h"
#include "log.h"
//#include "easylogging++.h"
//using namespace el::base::debug;
//using namespace el::base::type;

namespace quark
{
    Channel::Channel(int fd) : is_listener_(false)
    {
        m_rawfd = fd;

        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);

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

    void Channel::SetEvent(uint16_t t)
    {
        //Debug(LOCATION, "socket ", m_rawfd, " set event: ", t);
        // todo: fix this
        if ((t & EventAccept) != 0)
        {
            is_listener_ = true;
            //Debug(LOCATION, "socket ", m_rawfd, " set listener", is_listener_);
        }

        if ((t & EventRead) != 0)
        {
            m_state.type |= EventRead;
            //Debug(LOCATION, "socket ", m_rawfd, " set read");
        }

        if ((t & EventWrite) != 0)
        {
            m_state.type |= EventWrite;
            //Debug(LOCATION, "socket ", m_rawfd, " set write");
        }
    }

    void Channel::RemoveEvent(uint16_t t)
    {
        if ((t & m_state.type) != 0)
        {
            m_state.type &= ~t;
            //Debug(LOCATION, "socket ", m_rawfd, " set write");
        }
    }

    uint16_t Channel::CurrentEvent()
    {
        //bool hasAccpet = (m_state.type & EventAccept) ? true : false;
        if (InBfferCapcity() > 0)
        {
            m_state.type |= EventRead;
        }

        if (WaitingSendBytes() > 0)
        {
            m_state.type |= EventWrite;
        }
        //if (hasAccpet)
        //{
        //    m_state.type |= EventAccept;
        //}
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

        write_buff_.get_readable_buffer(write_vecs, nr_vecs);
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

        read_buff_.get_writeable_buffer(read_vecs, nr_vecs);
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

    void Channel::Send()
    {
        uint32_t nv = 0;
        iovec *vec = get_write_vecs(nv);
        auto bytes = writev(GetSocket(), vec, nv);
        GetWriteBuffer().erase(bytes);
    }

    int Channel::Recieve()
    {
        uint32_t nv = 0;
        iovec *vec = get_read_vecs(nv);
        auto bytes = readv(GetSocket(), vec, nv);
        if (bytes > 0)
        {
            GetReadBuffer().add(bytes);
        }
        return bytes;
    }
} // namespace quark