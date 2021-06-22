#pragma once

#include "singleton.h"
#include "io_context.h"
#include "service.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

int Add(int a, int b);
int check_io_uring_function();
int setup_listening_socket(io_context* ctx, int port); 
int setup_connect_socket(io_context* ctx, char* addr, int port); 

/*
#ifdef __cplusplus
}
#endif
*/
