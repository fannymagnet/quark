#include <iostream>
#include <string.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        cerr << sock << endl;
        exit(1);
    }

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //读取服务器传回的数据
    char buffer[40];
    read(sock, buffer, sizeof(buffer)-1);
   
    cout << "Message form server: " << buffer << endl;
   
    //关闭套接字
    close(sock);

    return 0;
}