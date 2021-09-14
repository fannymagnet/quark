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
    int setup_connect_socket(io_context *ctx, char *addr, int port);

    /*
#ifdef __cplusplus
}
#endif
*/

} // namespace quark