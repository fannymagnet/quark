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
            return false;
        }

        bool Pop(size_t bytes)
        {
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