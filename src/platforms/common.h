#pragma once

#if defined(_WIN32) || defined(_WIN64)

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
namespace quark
{
    typedef SOCKET socket_type;
    typedef WSABUF buf_type;
}

#else

#include <unistd.h>
#include <sys/select.h>
#include <sys/uio.h>

namespace quark
{
    typedef int socket_type;
    typedef iovec buf_type;
}
#endif