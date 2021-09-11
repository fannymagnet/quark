#pragma once
/*
#include <coroutine>
#include <optional>
#include <cassert>

namespace quark
{

    template <typename T>
    class CoTask;

    template <typename T>
    class TaskPromise
    {
    public:
        CoTask<T> get_return_object()
        {
            return std::coroutine_handle<TaskPromise<T>>::from_promise(*this);
        }
        std::suspend_never initial_suspend()
        {
            std::cout << "call initial_suspend" << std::endl;
            return {};
        }
        std::suspend_always final_suspend() noexcept
        {
            std::cout << "call final_suspend" << std::endl;
            return {};
        }
        void return_value(T value)
        {
            std::cout << "return value: " << value << "\n";
            result = value;
        }
        void unhandled_exception() {}

        std::optional<T> result{};
    };

    template <typename T>
    struct CoTaskAwaitable
    {
        bool await_ready() { return ready; }
        void await_suspend(std::coroutine_handle<T> h)
        {
            std::cout << "coroutine task waiting " << std::endl;
            //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            //ready = true;
            h.resume();
        }
        void await_resume() {}
        bool ready = false;
    };

    template <typename T>
    class CoTask
    {
    public:
        using promise_type = TaskPromise<T>;
        CoTask() {}
        ~CoTask()
        {
            handle_.destroy();
            std::cout << "coroutine task destroyed " << std::endl;
        }
        CoTask(std::coroutine_handle<promise_type> handle) : handle_(handle) {}

        auto operator co_await() &&noexcept
        {
            return CoTaskAwaitable<T>{};
        }

        std::optional<T> GetValue()
        {
            assert(handle_.done());
            return handle_.promise().result;
        }

    private:
        std::coroutine_handle<promise_type> handle_;
    };
}
*/