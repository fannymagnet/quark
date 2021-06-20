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
    //check_io_uring_function();
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        cerr << sock << endl;
        exit(1);
    }

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));           //每个字节都用0填充
    serv_addr.sin_family = AF_INET;                     //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //具体的IP地址
    serv_addr.sin_port = htons(8888);                   //端口

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (true)
    {
        string msg = "";
        cin >> msg;
        if (msg == "exit")
        {
            break;
        }
        write(sock, msg.data(), msg.size());
        cout << "send to server: "<< msg << " : " << msg.size() << " bytes" << endl;

        //读取服务器传回的数据
        char buffer[400];
        memset(buffer, 0, sizeof(buffer));
        int nbytes = read(sock, buffer, sizeof(buffer) - 1);
        cout << "Message form server: " << buffer << " bytes: " << nbytes << endl;
    }

    //关闭套接字
    close(sock);
}