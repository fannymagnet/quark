#pragma once

#include "common.h"
#include "../noncopyable.h"
#include <vector>

namespace quark
{
#if !defined(_WIN32) && !defined(_WIN64)
    class PosixIoBuf : NonCopyable
    {
    public:
        void Clear() {
            count_ = 0;
            io_vecs_.clear();
        }

        bool Push(void* p, size_t len)
        {
            struct iovec buf;
            buf.iov_base = p;
            buf.iov_len = len;
            count_ += len;
            io_vecs_.emplace_back(buf);
            return true;
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
                if (bytes >= io_vecs_[0].iov_len)
                {
                    bytes -= io_vecs_[0].iov_len;
                    io_vecs_.erase(io_vecs_.begin());
                    continue;
                }
                else 
                {
                    io_vecs_[0].iov_len -= bytes;
                    io_vecs_[0].iov_base = io_vecs_[0].iov_base + bytes;
                    bytes = 0;
                }
            }

            return true;
        }

        size_t BufCount() {
            return io_vecs_.size();
        }

        bool Empty()
        {
            return count_ == 0;
        }

        buf_type* Data() {
            return io_vecs_.data();
        }
    private:
        size_t count_;
        std::vector<buf_type> io_vecs_;
    };
#endif
} // namespace quark