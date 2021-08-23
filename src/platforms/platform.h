#if defined(WIN32) || defined(WIN64)
#define SOCKET socket_type

#include "win_fd_set.h"
#define WinFdSet FdSetAdapter

#else

#define int socket_type

#include "posix_fd_set.h"
#define PosixFdSet FdSetAdapter

#endif