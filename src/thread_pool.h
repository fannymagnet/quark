#pragma once

#include <thread>
#include <memory>
#include <future>
#include <functional>
#include <vector>
#include <experimental/tuple>
#include <atomic>

#define __USE_SAFEQUEUE
#ifndef __USE_SAFEQUEUE
#include "concurrentqueue/blockingconcurrentqueue.h"
#else
#include "safe_queue.h"
#endif


namespace quark
{
    typedef std::function<void()> Task;

    class ThreadPool
    {
    public:
        ThreadPool(size_t pool_size) : m_pool_size(pool_size)
        {
            m_running.store(true, std::memory_order::memory_order_release);
            m_worker_balance.store(0);
            m_workers.reserve(pool_size);
            m_tasks.reserve(pool_size);
            //m_tasks = std::vector(pool_size, 1024));
            for (size_t i = 0; i < pool_size; i++)
            {
#ifndef __USE_SAFEQUEUE
                m_tasks.emplace_back(moodycamel::BlockingConcurrentQueue<Task>());
#else
                m_tasks.emplace_back(SafeQueue<Task>());
#endif
                m_workers.emplace_back(std::thread([this](size_t idx)
                                                   {
                                                       while (this->m_running.load(std::memory_order::memory_order_acquire))
                                                       {
                                                           Task t = nullptr;
#ifndef __USE_SAFEQUEUE
                                                           this->m_tasks[idx].wait_dequeue(t);
#else
                                                           this->m_tasks[idx].move_pop(t);
#endif
                                                           if (t != nullptr)
                                                           {
                                                               t();
                                                           } /*else {
                        this->woker_steal(idx, t);
                        if (t != nullptr) {
                            t();
                        }
                    }*/
                                                       }
                                                   },
                                                   i));
            }
        }

        ~ThreadPool()
        {
            for (size_t i = 0; i < m_pool_size; ++i)
            {
                m_workers[i].join();
            }
        }

        void stop()
        {
            m_running.store(false, std::memory_order::memory_order_release);
        }

        template <typename F, typename... Args>
        auto enqueue_task(F &&func, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
        {
            using result_type = typename std::invoke_result<F, Args...>::type;
            auto task = std::make_shared<std::packaged_task<result_type()>>(
                [func = std::move(func),
                 args = std::make_tuple(std::forward<Args>(args)...)]()
                {
                    return std::experimental::apply(func, args);
                });

            std::future<result_type> res = task->get_future();
            unsigned char worker_in_turn = m_worker_balance.fetch_add(1, std::memory_order_acq_rel);
            int index = worker_in_turn % m_pool_size;
#ifndef __USE_SAFEQUEUE
            m_tasks[index].enqueue([task]()
                                   { (*task)(); });
#else
            m_tasks[index].push([task]()
                                { (*task)(); });
#endif
            return res;
        }

        template <typename F, typename... Args>
        auto enqueue_task_ptr(std::shared_ptr<F> p_func, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
        {
            using result_type = typename std::invoke_result<F, Args...>::type;
            auto task = std::make_shared<std::packaged_task<result_type()>>(
                [pfunc = std::move(p_func),
                 args = std::make_tuple(std::forward<Args>(args)...)]()
                {
                    return std::experimental::apply(*pfunc, args);
                });

            std::future<result_type> res = task->get_future();
            unsigned char worker_in_turn = m_worker_balance.fetch_add(1, std::memory_order_acq_rel);
            int index = worker_in_turn % m_pool_size;
#ifndef __USE_SAFEQUEUE
            m_tasks[index].enqueue([task]()
                                   { (*task)(); });
#else
            m_tasks[index].push([task]()
                                { (*task)(); });
#endif
            return res;
        }

    private:
        bool woker_steal(u_int8_t src, Task &item)
        {
#ifndef __USE_SAFEQUEUE
            return false;
#else
            //LOG_INFO(LOG_MSG("steal begin: ") << (int)src);
            size_t tsize = m_tasks.size();
            for (size_t i = src; i != src; (++i) %= tsize)
            {
                if (i == src)
                {
                    continue;
                }
                //if (m_tasks[i].size() > 10) {
                bool r = m_tasks[i].try_move_pop(item);
                if (!r)
                {
                    continue;
                }
                else
                {
                    return true;
                }
                //}
            }

            return false;
#endif
        }

    private:
        size_t m_pool_size;
        std::atomic<unsigned char> m_worker_balance;
        std::atomic<bool> m_running;
        std::vector<std::thread> m_workers;
#ifndef __USE_SAFEQUEUE
        std::vector<moodycamel::BlockingConcurrentQueue<Task>> m_tasks;
#else
        std::vector<SafeQueue<Task>> m_tasks;
#endif
    };
}