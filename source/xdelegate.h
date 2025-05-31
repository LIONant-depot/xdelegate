#include <vector>
#include <functional>
#include <mutex>
#include <cstdint>
#include <algorithm>

namespace xdelegate
{
    //--------------------------------------------------------------------------------------
    // A delegate class that manages a list of callbacks with variadic arguments.
    // Supports member functions, lambdas and free functions.
    //--------------------------------------------------------------------------------------
    template<typename... T_ARGS>
    struct thread_unsafe
    {
        struct info
        {
            using callback = void(void* pPtr, T_ARGS...);

            callback*   m_pCallback;
            void*       m_pClass;
        };


        thread_unsafe() = default;
        ~thread_unsafe() = default;

        // Delete copy constructor and assignment
        thread_unsafe(const thread_unsafe&) = delete;
        thread_unsafe& operator=(const thread_unsafe&) = delete;

        //--------------------------------------------------------------------------------------
        // Registers a member function callback for a class instance.
        // @param classInstance Reference to the class instance.
        // @param Template Parameter. Note that the member function pointer must be past to the function as a template parameter
        //        Example: MyDelegates.Register<&pepe::MyFuction>( MyClass );
        template< auto T_MEMBER_FUNC, typename  T_CLASS >
        __inline void Register(T_CLASS& ClassInstance ) noexcept
        {
            static_assert(std::is_member_function_pointer_v<decltype(T_MEMBER_FUNC)>, "T_MEMBER_FUNC must be a member function pointer.");

            m_Delegates.push_back
            ( 
                info 
                { .m_pCallback = [](void* pClassInstance, T_ARGS... Args) constexpr noexcept
                    {
                        std::invoke(T_MEMBER_FUNC, static_cast<T_CLASS*>(pClassInstance), std::forward<T_ARGS>(Args)...);
                    }
                , .m_pClass = &ClassInstance
                }
            );
        }

        //--------------------------------------------------------------------------------------
        // Registers a free function or static member function callback.
        // @param pHandle is an optional pointer that later can be used to identify this entry for deletion
        // @param Template Parameter. The parameter could be a lambda or callable
        //        Example: MyDelegates.Register<[](int& a){ a = a + 1; }>();
        template< auto T_CALLABLE_V >
        __inline void Register(void* pHandle = nullptr) noexcept
        {
            static_assert(std::is_invocable_v<std::remove_pointer_t<decltype(T_CALLABLE_V)>>, "T_CALLABLE_V must be a callable (e.g., a lambda or a free function)");
            m_Delegates.push_back
            (
                info
                { .m_pCallback = [](void*, T_ARGS... Args) constexpr noexcept
                    {
                        std::invoke(T_CALLABLE_V, std::forward<T_ARGS>(Args)...);
                    }
                , .m_pClass = pHandle
                }
            );
        }

        //--------------------------------------------------------------------------------------
        // Invokes all registered callbacks with the provided arguments.
        // @param args Variadic arguments to pass to the callbacks.
        __inline void NotifyAll(T_ARGS... Args) const noexcept
        {
            for (const auto& D : m_Delegates)
            {
                D.m_pCallback(D.m_pClass, std::forward<T_ARGS>(Args)...);
            }
        }

        //--------------------------------------------------------------------------------------
        // Removes a specific delegate using its handle.
        // @param pInstance is the instance used to register the delegate it could be the class instance or a pHandle... 
        __inline void RemoveDelegates(void* pInstance) noexcept
        {
            // Remove all the instances of the class
            m_Delegates.erase
            ( std::remove_if( m_Delegates.begin(), m_Delegates.end(), [pInstance](const info& d) constexpr noexcept
                { 
                    return d.m_pClass == pInstance; 
                })
            , m_Delegates.end()
            );
        }

        std::vector<info> m_Delegates;
    };

    //-----------------------------------------------------------------------------------
    // This is the thread safe version of the above 
    template<typename... T_ARGS>
    struct thread_safe : thread_unsafe<T_ARGS...>
    {
        thread_safe() = default;
        ~thread_safe() = default;

        // Delete copy constructor and assignment
        thread_safe(const thread_safe&)            = delete;
        thread_safe& operator=(const thread_safe&) = delete;

        __inline void RemoveDelegates(void* pInstance) noexcept
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            thread_unsafe<T_ARGS...>::RemoveDelegates(pInstance);
        }

        __inline void NotifyAll(T_ARGS... Args) const noexcept
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            thread_unsafe<T_ARGS...>::NotifyAll( std::forward<T_ARGS>(Args)... );
        }

        template< auto T_CALLABLE_V >
        __inline void Register(void* pHandle = nullptr) noexcept
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            thread_unsafe<T_ARGS...>::Register<T_CALLABLE_V>(pHandle);
        }

        template< auto T_FUNCTION_PTR_V, typename  T_CLASS >
        __inline void Register(T_CLASS& ClassInstance) noexcept
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            thread_unsafe<T_ARGS...>::Register<T_FUNCTION_PTR_V>(ClassInstance);
        }

        mutable std::mutex m_Mutex;
    };
}