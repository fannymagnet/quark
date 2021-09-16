#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>

#include <string.h>
#if defined(_WIN32) || defined(_WIN64)

#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib")  

#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
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
    constexpr int buf_size = 256;

#if defined(_WIN32) || defined(_WIN64)
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if (0 != WSAStartup(sockVersion, &wsaData))
    {
        return false;
    }
#endif
    int sock = TcpConnect("127.0.0.1", 8888);
    if (sock <= 0)
    {
        cout << "connect error: " << sock << endl;
        return -1;
    }
    std::chrono::milliseconds ms{10000};

    auto begin = chrono::steady_clock::now();
    cout << " socket: " << sock << " begin send!" << endl;

    while (true)
    {
        std::string send_buffer;
        cin >> send_buffer;
#if defined(_WIN32) || defined(_WIN64)
        send(sock, send_buffer.c_str(), send_buffer.size(), 0);
#else
        write(sock, send_buffer.c_str(), send_buffer.size());
#endif
        cout << sock << " write " << send_buffer << " finished, begin read --" << endl;

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
        cout << sock << " read " << nbytes << " bytes finished , context: " << buffer << ". begin next" << endl;
    }

    //关闭套接字
#if defined(_WIN32) || defined(_WIN64)
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif

    return 0;
}