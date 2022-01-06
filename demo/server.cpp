#include <iostream>
#include <string.h>

#include "quark.h"

using namespace std;

int main()
{
    quark::IoContext *ctx = new quark::IoContext();
    if (!ctx->Init())
    {
        cout << "init io context failed!" << endl;
        exit(1);
    }

    quark::Service service(ctx, "0.0.0.0", 8888);
    service.Start();
    service.Run();
    return 0;
}
