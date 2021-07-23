#pragma once

#include <type_traits>

namespace quark
{
    template <typename T>
    class NotNull
    {
    private:
        /* data */
        T ptr_;

    public:
        template <typename = std::enable_if_t<std::is_same<std::nullptr_t, T>::value>>
        constexpr NotNull(T t) : ptr_(std::move(t)) {}
        constexpr NotNull(T &&t) : ptr_(std::move(t)) {}
        ~NotNull() {}

        constexpr NotNull(T t) : ptr_(std::move(t)) {}

        // prevents compilation when someone attempts to assign a null pointer constant
        NotNull() = delete;
        NotNull(std::nullptr_t) = delete;
        NotNull &operator=(std::nullptr_t) = delete;

        constexpr std::conditional_t<std::is_copy_constructible<T>::value, T, const T &> get() const
        {
            //Ensures(ptr_ != nullptr);
            return ptr_;
        }
    };
} // namespace quark
