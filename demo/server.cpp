#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "quark.h"

using namespace std;

int main(){

    quark::io_context* ctx = new quark::io_context();
    if (!ctx->init()){
        cout << "init io context failed!" << endl;
        exit(1);
    }

    int serv_socket = quark::setup_listening_socket(ctx, 8888);
    if (serv_socket < 0)
    {
        exit(1);
    }
    cout << "server_socket begin listen: " << serv_socket << endl;
    try 
    {
        ctx->run();
    } 
    catch(exception e) 
    {
        cout << "error happened : " << e.what() << endl;
        cerr << "errno: " << errno << endl;
    }
    return 0;
}
