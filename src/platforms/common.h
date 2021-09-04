#pragma once

#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
namespace quark
{
    typedef SOCKET socket_type;
    typedef WSADATA buf_type;
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