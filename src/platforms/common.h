#pragma once

#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
namespace quark
{
    typedef SOCKET socket_type;
}

#else
namespace quark
{
    typedef int socket_type;
}
#endif