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
        if (InBfferCapcity() > 0)
        {
            m_state.type |= EventRead;
        }

        if (WaitingSendBytes() > 0)
        {
            m_state.type |= EventWrite;
        }
        return m_state.type;
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
        auto& vec = get_write_vecs();
        auto bytes = writev(GetSocket(), vec.Data(), vec.BufCount());
        GetWriteBuffer().erase(bytes);
    }

    int Channel::Recieve()
    {
        uint32_t nv = 0;
        auto& vec = get_read_vecs();
        auto bytes = readv(GetSocket(), vec.Data(), vec.BufCount());
        if (bytes > 0)
        {
            GetReadBuffer().add(bytes);
        }
        return bytes;
    }

    MultiIoBuf& Channel::get_write_vecs()
    {
        write_vecs.Clear();
        write_buff_.get_readable_buffer(write_vecs);
        return write_vecs;
    }

    MultiIoBuf& Channel::get_read_vecs()
    {
        read_vecs.Clear();
        read_buff_.get_writeable_buffer(read_vecs);
        return read_vecs;
    }
} // namespace quark