#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

using namespace std;

int TcpConnect(string ipv4, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        cerr << sock << endl;
        exit(1);
    }

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ipv4.c_str());  //具体的IP地址
    serv_addr.sin_port = htons(port);  //端口

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    return sock;
}

struct Sum {
    int64_t in = 0;
    int64_t out = 0;
};

int main() {
    int thread_count = 20;
    int buf_size = 128;
    vector<Sum> results;
    for (int i = 0; i < thread_count; ++i)
    {
        results.push_back(Sum());
    }

    vector<thread> ts;
    for (int i = 0 ; i < thread_count; ++i) {

        thread t([i, &results, buf_size]() {
            Sum s;
            int sock = TcpConnect("127.0.0.1", 8888); 
            std::chrono::milliseconds ms {10000};

            auto begin = chrono::steady_clock::now();
            cout << " socket: " << sock << " begin send!" << endl;

            while (true)
            {
                auto end = chrono::steady_clock::now();
                if ((end - begin) > ms) {
                    cout << " socket: " << sock << " finiesh send!" << endl;
                    break;
                }
                char send_buffer[buf_size];
                memset(send_buffer, 1, sizeof(send_buffer));
                write(sock, send_buffer, sizeof(send_buffer)-1);
                ++s.in;
                cout << sock << " write finished, begin read --" << endl;

                //读取服务器传回的数据
                char buffer[buf_size];
                memset(buffer, 0, sizeof(buffer));
                int nbytes = read(sock, buffer, sizeof(buffer)-1);
                if (nbytes == 0) {
                    cout << "Message form server: " << sock << " bytes: " << nbytes << " disconnected" << endl;
                    break;
                }
                cout << sock << " read finished " << nbytes <<", begin next --" << endl;
                ++s.out;
            }

            results[i] = s;
            cout << sock << " begin setting result: " << s.in << " : " << s.out << endl;
            //关闭套接字
            close(sock);
        });
        ts.emplace_back(std::move(t));
    }

    for(int i = 0; i < ts.size(); i++) {
        ts[i].join();
    }

    Sum sum_counter;
    for(int i = 0; i < results.size(); ++i)
    {
        sum_counter.in += results[i].in;
        sum_counter.out += results[i].out;
    }

    cout << "in:" << sum_counter.in / 10 << endl;
    cout << "out:" << sum_counter.out / 10 << endl;

    return 0;
}