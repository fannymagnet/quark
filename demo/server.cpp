#include <iostream>
#include <string.h>

#include "quark.h"

using namespace std;

int main()
{
    quark::io_context *ctx = new quark::io_context();
    if (!ctx->init())
    {
        cout << "init io context failed!" << endl;
        exit(1);
    }

    quark::Service service(ctx, "", 8888);
    service.start();
    service.run();
    /*
    quark::io_context *ctx = new quark::io_context();
    if (!ctx->init())
    {
        cout << "init io context failed!" << endl;
        exit(1);
    }

    quark::Acceptor acceptor(ctx, 100);
    if (!acceptor.Open(8888))
    {
        exit(1);
    }

    if (!acceptor.Accept())
    {
        exit(1);
    }
    cout << "server_socket begin listen: " << acceptor.GetSocket() << endl;
    try
    {
        ctx->run();
    }
    catch (exception e)
    {
        cout << "error happened : " << e.what() << endl;
        cerr << "errno: " << errno << endl;
    }
    */
    return 0;
}
