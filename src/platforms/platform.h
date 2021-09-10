#pragma once

#include "win_fd_set.h"
#include "posix_fd_set.h"

#include "win_io_buf.h"
#include "posix_io_buf.h"

namespace quark
{
#if defined(WIN32) || defined(WIN64)
    typedef WinFdSet FdSetAdapter;
    typedef WinIoBuf MultiIoBuf;

    static uint32_t WriteVec(socket_type fd, buf_type* buf, int buf_size) {
        return 0;
    }

    static uint32_t ReadVec(socket_type fd, buf_type* buf, int buf_size) {
        return 0;
    }
#else
    typedef PosixFdSet FdSetAdapter;
    typedef PosixIoBuf MultiIoBuf;

    static uint32_t WriteVec(socket_type fd, buf_type* buf, int buf_size) {
        return writev(fd, buf, buf_size);
    }

    static uint32_t ReadVec(socket_type fd, buf_type* buf, int buf_size) {
        return readv(fd, buf, buf_size);
    }
#endif
    //typedef int socket_type;
} // namespace quark
