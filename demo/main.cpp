#include "quark.h"
#include <iostream>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> /* See NOTES */
#include <unistd.h>

using namespace std;

int main()
{
    check_io_uring_function();
    Add(5,8);
}