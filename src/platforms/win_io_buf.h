#pragma once
#include "common.h"
#include "../noncopyable.h"

#include <vector>

namespace quark
{
#if defined(_WIN32) || defined(_WIN64)
    class WinIoBuf : NonCopyable
    {
    public:
        void Clear()
        {
            count_ = 0;
            io_vecs_.clear();
        }

        bool Push(void *p, size_t len)
        {
            WSABUF buf;
            buf.buf = (char *)p;
            buf.len = len;
            count_ += len;
            io_vecs_.emplace_back(buf);
            return false;
        }

        bool Pop(size_t bytes)
        {
            if (count_ < bytes)
            {
                return false;
            }

            count_ -= bytes;
            if (count_ == 0)
            {
                io_vecs_.clear();
                return true;
            }

            while (bytes > 0 && io_vecs_.size() > 0)
            {
                if (bytes >= io_vecs_[0].len)
                {
                    bytes -= io_vecs_[0].len;
                    io_vecs_.erase(io_vecs_.begin());
                    continue;
                }
                else 
                {
                    io_vecs_[0].len -= bytes;
                    io_vecs_[0].buf = io_vecs_[0].buf + bytes;
                    bytes = 0;
                }
            }

            return false;
        }

        size_t BufCount()
        {
            return io_vecs_.size();
        }

        bool Empty()
        {
            return count_ == 0;
        }

        buf_type *Data()
        {
            return io_vecs_.data();
        }

    private:
        size_t count_;
        std::vector<buf_type> io_vecs_;
    };
#endif
} // namespace quark