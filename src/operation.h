#include "noncopyable.h"
#include <functional>

namespace quark
{
    class Operation : NonCopyable
    {
    public:
    typedef std::function<void(void* owner, Operation* from, uint32_t& error_code, uint32_t bytes)> Handler;
        Operation (Handler func) : func_(func), result_(0) {}
        virtual ~Operation () {}
        void DoComplete(void* owner, uint32_t& ec, uint32_t bytes) {
            func_(owner, this, ec, bytes);
        }

        void Destroy() { 
            uint32_t ec = 0;
            func_(nullptr, this, ec, 0);
        }
    public:
        uint32_t result_;
    private:
        Handler func_;
        /* data */
    };
} // namespace quark
