#pragma once

#include "win_fd_set.h"
#include "posix_fd_set.h"

namespace quark
{
#if defined(WIN32) || defined(WIN64)
    typedef WinFdSet FdSetAdapter;
#else
    typedef PosixFdSet FdSetAdapter;
#endif
    //typedef int socket_type;
} // namespace quark
