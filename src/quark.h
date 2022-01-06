#pragma once

#include "singleton.h"
#include "io_context.h"
#include "service.h"
#include "task.h"
#include "acceptor.h"
#include "service.h"

using namespace std;
namespace quark
{
    /*
#ifdef __cplusplus
extern "C" {
#endif
*/

    extern int Add(int a, int b);
    //int check_io_uring_function();
#if !defined(_WIN32) || !defined(_WIN64)
    int setup_connect_socket(IoContext *ctx, char *addr, int port);
#endif

    /*
#ifdef __cplusplus
}
#endif
*/

} // namespace quark