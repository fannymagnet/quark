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
#else
    typedef PosixFdSet FdSetAdapter;
    typedef PosixIoBuf MultiIoBuf;
#endif
    //typedef int socket_type;
} // namespace quark
