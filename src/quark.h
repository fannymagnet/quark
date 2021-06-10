#pragma once

#include "singleton.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

int Add(int a, int b);
int check_io_uring_function();
int setup_listening_socket(int port); 

/*
#ifdef __cplusplus
}
#endif
*/
