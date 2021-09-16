#pragma once

#include "win_fd_set.h"
#include "posix_fd_set.h"

#include "win_io_buf.h"
#include "posix_io_buf.h"

#include "../log.h"

namespace quark
{
#if defined(WIN32) || defined(WIN64)
    typedef WinFdSet FdSetAdapter;
    typedef WinIoBuf MultiIoBuf;

    static uint32_t WriteVec(socket_type fd, MultiIoBuf& io_buf) {
        DWORD write_bytes = 0;
        auto r = WSASend(fd, io_buf.Data(), io_buf.BufCount(), &write_bytes, (DWORD)MSG_PEEK, nullptr, nullptr);
        Debug(LOCATION, "r: ", r, "  write_bytes: ", write_bytes);
        return write_bytes;
    }

    static uint32_t ReadVec(socket_type fd, MultiIoBuf& io_buf) {
        DWORD read_bytes = 0;
        auto r = WSARecv(fd, io_buf.Data(), io_buf.BufCount(), &read_bytes, (LPDWORD)MSG_PEEK, nullptr, nullptr);
        Debug(LOCATION, "r: ", r, "  read_bytes: ", read_bytes);
        return read_bytes;
    }
#else
    typedef PosixFdSet FdSetAdapter;
    typedef PosixIoBuf MultiIoBuf;

    static u_int32_t WriteVec(socket_type fd, MultiIoBuf& io_buf) {
        return writev(fd, io_buf.Data(), io_buf.BufCount());
    }

    static u_int32_t ReadVec(socket_type fd, MultiIoBuf& io_buf) {
        return readv(fd, io_buf.Data(), io_buf.BufCount());
    }
#endif
    //typedef int socket_type;
} // namespace quark
