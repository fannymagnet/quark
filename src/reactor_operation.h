#include "operation.h"
#include <functional>

namespace quark
{
    class ReactorOperation : public Operation
    {
    public:
        typedef std::functional<void(ReactorOperation *)> PerformFunc;
        enum Status
        {
            NotDone,
            Done
        };
        uint32_t error_code;
        uint32_t bytes_transfered;

        ReactorOperation(PerformFunc perform, Operation::Handler handle)
            : Operation(handle), perform_func_(perform), error_code(0), bytes_transfered(0) {}
        virtual ~ReactorOperation() {}

        Status Perfrom()
        {
            if (nullptr == perform_func_)
            {
                return Done;
            }
            return perform_func_(this);
        }

    private:
        /* data */
        PerformFunc perform_func_;
    };
} // namespace quark
