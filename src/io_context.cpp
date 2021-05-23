#include "io_context.h"
#include <iostream>
#include <memory.h>

io_context::io_context(/* args */)
{
}

bool io_context::init() {
    // initialize io_uring
    struct io_uring_params params;
    memset(&params, 0, sizeof(params));
    int result = io_uring_queue_init_params(256, &m_ring, &params);
    if (result < 0) {
        std::cerr << "init uring queue failed! error: " << -result << std::endl;
        return false;
    }

    // check if IORING_FEAT_FAST_POLL is supported
    if (!(params.features & IORING_FEAT_FAST_POLL)) {
        std::cerr << "IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n";
        return false;
    }

    return true;
}

io_context::~io_context()
{
}