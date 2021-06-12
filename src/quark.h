#pragma once

#include "singleton.h"
#include "io_context.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

int Add(int a, int b);
int check_io_uring_function();
int setup_listening_socket(io_context* ctx, int port); 

/*
#ifdef __cplusplus
}
#endif
*/
