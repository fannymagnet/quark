#include "quark.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <cstring>

#include "liburing.h"
//#include "easylogging++.h"

namespace quark
{
    //INITIALIZE_EASYLOGGINGPP

    CoTask<int> CounterTimes(int a, int b, int c)
    {
        int sum = 0;
        for (int i = 0; i < c; ++i)
        {
            sum += a;
            sum += b;
            printf("current %d sum: %d\n", i, sum);
            co_await CoTaskAwaitable<TaskPromise<int>>{};
            //co_await std::suspend_always{};
        }
        co_return sum;
    }

    int Add(int a, int b)
    {
        auto co = CounterTimes(a, b, 5);
        std::cout << " coroutine return value = " << co.GetValue().value_or(-1) << std::endl;
        return a + b;
    }

    static const char *op_strs[] = {
        "IORING_OP_NOP",
        "IORING_OP_READV",
        "IORING_OP_WRITEV",
        "IORING_OP_FSYNC",
        "IORING_OP_READ_FIXED",
        "IORING_OP_WRITE_FIXED",
        "IORING_OP_POLL_ADD",
        "IORING_OP_POLL_REMOVE",
        "IORING_OP_SYNC_FILE_RANGE",
        "IORING_OP_SENDMSG",
        "IORING_OP_RECVMSG",
        "IORING_OP_TIMEOUT",
        "IORING_OP_TIMEOUT_REMOVE",
        "IORING_OP_ACCEPT",
        "IORING_OP_ASYNC_CANCEL",
        "IORING_OP_LINK_TIMEOUT",
        "IORING_OP_CONNECT",
        "IORING_OP_FALLOCATE",
        "IORING_OP_OPENAT",
        "IORING_OP_CLOSE",
        "IORING_OP_FILES_UPDATE",
        "IORING_OP_STATX",
        "IORING_OP_READ",
        "IORING_OP_WRITE",
        "IORING_OP_FADVISE",
        "IORING_OP_MADVISE",
        "IORING_OP_SEND",
        "IORING_OP_RECV",
        "IORING_OP_OPENAT2",
        "IORING_OP_EPOLL_CTL",
        "IORING_OP_SPLICE",
        "IORING_OP_PROVIDE_BUFFERS",
        "IORING_OP_REMOVE_BUFFERS",
        "IORING_OP_TEE",
        "IORING_OP_SHUTDOWN",
        "IORING_OP_RENAMEAT",
        "IORING_OP_UNLINKAT",
        "IORING_OP_MKDIRAT",
    };

    int check_io_uring_function()
    {
        struct utsname u;
        uname(&u);
        //LOG(INFO) << "You are running kernel version: " << u.release;
        struct io_uring_probe *probe = io_uring_get_probe();
        printf("Report of your kernel's list of supported io_uring operations:\n");
        for (char i = 0; i < IORING_OP_LAST; i++)
        {
            printf("%s: ", op_strs[i]);
            if (io_uring_opcode_supported(probe, i))
                printf("yes.\n");
            else
                printf("no.\n");
        }
        free(probe);
        return 0;
    }

    int setup_listening_socket(io_context *ctx, int port)
    {
        int sock;
        struct sockaddr_in srv_addr;

        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        //LOG(ERROR) << "socket()";

        {
            int enable = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
                std::cout << "setsockopt(SO_REUSEADDR)" << std::endl;
        }

        //LOG(ERROR) << "setsockopt(SO_REUSEADDR)";

        memset(&srv_addr, 0, sizeof(srv_addr));
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_port = htons(port);
        srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        /* We bind to a port and turn this socket into a listening
     * socket.
     * */
        if (bind(sock,
                 (const struct sockaddr *)&srv_addr,
                 sizeof(srv_addr)) < 0)
            //LOG(ERROR) << "bind()";

            if (listen(sock, 10) < 0)
                //LOG(ERROR) << "listen()";

                if (nullptr == ctx)
                {
                    //LOG(ERROR) << "io context is nullptr";
                    exit(1);
                }

        ctx->add_accept(sock);

        return (sock);
    }

    int setup_connect_socket(io_context *ctx, char *addr, int port)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == sock)
        {
            //LOG(ERROR) << "create socket error!" << sock;
            exit(1);
        }

        //向服务器（特定的IP和端口）发起请求
        struct ::sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));    //每个字节都用0填充
        serv_addr.sin_family = AF_INET;              //使用IPv4地址
        serv_addr.sin_addr.s_addr = inet_addr(addr); //具体的IP地址
        serv_addr.sin_port = htons(port);            //端口

        int r = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (r < 0)
        {
            //LOG(ERROR) << "socket connect error: " << r;
            exit(1);
        }

        return sock;
    }
}