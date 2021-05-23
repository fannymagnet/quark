#pragma once

enum EventType {
    EventAccept = 0,
    EventRead = 2,
    EventWrite = 4,
};

class channel
{
public:
    channel(/* args */);
    ~channel();

private:
    /* data */
    int m_rawfd;
    EventType m_event;
    //todo: add two ring buffer for write and read
};

channel::channel(/* args */)
{
}

channel::~channel()
{
}
