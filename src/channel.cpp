#if defined(_WIN32) || defined(_WIN64)
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include "channel.h"
#include "log.h"

namespace quark
{
    Channel::Channel(int fd) : is_listener_(false)
    {
        m_rawfd = fd;

#if defined(WIN32) || defined(WIN64)
#else
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif

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
        if (WaitingSendBytes() == 0)
        {
            return;
        }
        auto &vec = get_write_vecs();
        auto bytes = WriteVec(GetSocket(), vec.Data(), vec.BufCount());
        if (bytes > 0)
        {
            vec.Pop(bytes);
            GetWriteBuffer().erase(bytes);
        }
    }

    int Channel::Recieve()
    {
        auto &vec = get_read_vecs();
        auto bytes = ReadVec(GetSocket(), vec.Data(), vec.BufCount());
        if (bytes > 0)
        {
            GetReadBuffer().add(bytes);
        }
        return bytes;
    }

    MultiIoBuf &Channel::get_write_vecs()
    {
        write_vecs.Clear();
        write_buff_.get_readable_buffer(write_vecs);
        return write_vecs;
    }

    MultiIoBuf &Channel::get_read_vecs()
    {
        read_vecs.Clear();
        read_buff_.get_writeable_buffer(read_vecs);
        return read_vecs;
    }

    void Channel::SetPoller(Poller *poller)
    {
        poller_ = poller;
    }

    void Channel::Write(uint8_t *data, uint32_t len)
    {
        if (nullptr == poller_)
        {
            Debug("channle not in poller");
            return;
        }
        /// TODO: FIX THIS DIRTY CODE
        auto func = [this, data, len]()
        {
            if (WaitingSendBytes() > 0)
            {
                uint32_t to_be_sent = len;
                while (to_be_sent > 0)
                {
                    uint32_t max_bytes = this->write_buff_.writeable_bytes();
                    auto bytes = this->write_buff_.put(data, max_bytes >= len ? len : max_bytes);
                    to_be_sent = bytes < to_be_sent ? to_be_sent - bytes : 0;
                    this->Send();
                }
                if (WaitingSendBytes() > 0)
                {
                    SetEvent(EventWrite);
                }
            }
            else
            {
                auto bytes = 0; //write(GetSocket(), data, len);
                #if defined(_WIN32) || defined(_WIN64)
                        //send(sock, send_buffer, sizeof(send_buffer)-1, 0);
                    bytes = send(GetSocket(), (const char*)data, len, 0);
                #else
                    //write(sock, send_buffer.c_str(), send_buffer.size());
                    bytes = write(GetSocket(), data, len);
                #endif
                if (bytes < len)
                {
                    uint32_t to_be_sent = len - bytes;
                    while (to_be_sent > 0)
                    {
                        uint32_t max_bytes = this->write_buff_.writeable_bytes();
                        auto bytes = this->write_buff_.put(data, max_bytes >= len ? len : max_bytes);
                        to_be_sent = bytes < to_be_sent ? to_be_sent - bytes : 0;
                        if (to_be_sent > 0)
                            this->Send();
                    }
                    if (WaitingSendBytes() > 0)
                    {
                        SetEvent(EventWrite);
                    }
                }
            }
        };
        poller_->RunInPoller(func);
    }
} // namespace quark