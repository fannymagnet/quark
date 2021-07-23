#pragma once
namespace quark
{
    class NonCopyable
    {
    protected:
        NonCopyable() {}
        ~NonCopyable() {}

    private: // emphasize the following members are private
        NonCopyable(const NonCopyable &) = delete;
        const NonCopyable &operator=(const NonCopyable &) = delete;
    };

}