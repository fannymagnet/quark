#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>

#include <string.h>
#if defined(_WIN32) || defined(_WIN64)

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#endif

using namespace std;

int TcpConnect(string ipv4, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        cerr << sock << endl;
        exit(1);
    }

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));            //每个字节都用0填充
    serv_addr.sin_family = AF_INET;                      //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ipv4.c_str()); //具体的IP地址
    serv_addr.sin_port = htons(port);                    //端口

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    return sock;
}

int main()
{

#if defined(_WIN32) || defined(_WIN64)
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (0 != WSAStartup(sockVersion, &wsaData))
    {
        return false;
    }
#endif
    int thread_count = 6;
    constexpr int buf_size = 128;

    vector<thread> ts;
    for (int i = 0; i < thread_count; ++i)
    {

        thread t([i, buf_size]()
                 {
                    int sock = TcpConnect("127.0.0.1", 8888);
                    if (sock != -1)
                    {
                        cout << "socket: " << sock << " begin send!" << endl;
                    }
                    else 
                    {
                        cout << "socket: connect error!" << endl;
                        return;
                    }

                    int msg_count = 0;
                    while (++msg_count > 10000)
                    {
                       char send_buffer[buf_size];
                       memset(send_buffer, 1, sizeof(send_buffer));
#if defined(_WIN32) || defined(_WIN64)
                        send(sock, send_buffer, sizeof(send_buffer) - 1, 0);
#else
                        write(sock, send_buffer, sizeof(send_buffer) - 1);
#endif
                        cout << sock << " write finished, begin read --" << endl;

                        //读取服务器传回的数据
                        char buffer[buf_size];
                        memset(buffer, 0, sizeof(buffer));
                        int nbytes = 0;
#if defined(_WIN32) || defined(_WIN64)
                        nbytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
#else
                        nbytes = read(sock, buffer, sizeof(buffer) - 1);
#endif
                        if (nbytes == 0)
                        {
                            cout << "Message form server: " << sock << " bytes: " << nbytes << " disconnected" << endl;
                            break;
                        }
                        cout << sock << " read finished " << nbytes << ", begin next --" << endl;
                     }
            //关闭套接字
#if defined(_WIN32) || defined(_WIN64)
                    closesocket(sock);
#else
                    close(sock);
#endif
                });
        ts.emplace_back(std::move(t));
    }

    for (int i = 0; i < ts.size(); i++)
    {
        ts[i].join();
    }

    while (true)
    {
        sleep(1000);
    }
#if defined(_WIN32) || defined(_WIN64)
    WSACleanup();
#endif

    return 0;
}