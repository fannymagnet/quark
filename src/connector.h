#pragma once

#include <memory>
#include <functional>
#include "ringbuffer.h"

class Channel;
class IoContext;
class Poller;

namespace quark
{
    class Connector;
    typedef std::shared_ptr<Connector> ConnectorPtr;
    typedef std::function<void(ConnectorPtr, uint8_t*)> MsgCallBack;

    class Connector : public std::enable_shared_from_this<Connector>, private NonCopyable
    {
    public:
        Connector(/* args */);
        virtual ~Connector();

        void SetMessageCallback(MsgCallBack cb) { msg_cb_ = cb; }

        void HandleWrite();
        void HandleRead();

        void Send(uint8_t *data, uint32_t len);
    private:
        /* data */
        IoContext *context_;
        Poller *poller_;
        Channel *channel_;

        RingBuffer write_buff_;
        RingBuffer read_buff_;

        MsgCallBack msg_cb_;
    };

} // namespace quark
