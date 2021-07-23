#pragma once

#include "noncopyable.h"
#include <memory>
#include <mutex>
namespace quark
{
    template <typename T>
    class Singleton : public NonCopyable
    {
    public:
        template <typename... Args>
        static T *Instance(Args &&...args)
        {
            if (m_pInstance == nullptr)
            {
                call_once(
                    get_once_flag(),
                    [](Args &&...args)
                    {
                        m_pInstance.reset(new T(std::forward<Args>(args)...));
                    },
                    std::forward<Args>(args)...);
            }

            return m_pInstance.get();
        }

        static T *GetInstance()
        {
            if (m_pInstance == nullptr)
            {
                call_once(
                    get_once_flag(),
                    []()
                    {
                        m_pInstance.reset(new T());
                    });
            }
            return m_pInstance.get();
        }

    private:
        static std::unique_ptr<T> m_pInstance;
        static std::once_flag &get_once_flag()
        {
            static std::once_flag once_;
            return once_;
        }
    };

    template <typename T>
    std::unique_ptr<T> Singleton<T>::m_pInstance = nullptr;

}