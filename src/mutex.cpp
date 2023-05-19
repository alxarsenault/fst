#include "fst/mutex.h"
#include "fst/memory.h"
//#include "fst/stream.h"

#if __FST_WINDOWS__
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // __FST_WINDOWS__

FST_TODO("mutex posix implementation")

FST_BEGIN_NAMESPACE

#if __FST_WINDOWS__
    struct mutex::native
    {
        using native_type = ::CRITICAL_SECTION;

        FST_ALWAYS_INLINE native() noexcept
        {

            //fst::debug()(sizeof(native_type));
            ::InitializeCriticalSection(&_critical_section);
        }

        FST_ALWAYS_INLINE ~native() noexcept { ::DeleteCriticalSection(&_critical_section); }

        FST_ALWAYS_INLINE void lock() noexcept { ::EnterCriticalSection(&_critical_section); }

        FST_ALWAYS_INLINE bool try_lock() noexcept { return (bool) ::TryEnterCriticalSection(&_critical_section); }

        FST_ALWAYS_INLINE void unlock() noexcept { ::LeaveCriticalSection(&_critical_section); }

        FST_ALWAYS_INLINE handle get_handle() const noexcept { return (void*) &_critical_section; }

        native_type _critical_section;
    };

#else
    struct mutex::native
    {
        FST_ALWAYS_INLINE native() noexcept
        {

        }

        FST_ALWAYS_INLINE ~native() noexcept {  }

        FST_ALWAYS_INLINE void lock() noexcept {   }

        FST_ALWAYS_INLINE bool try_lock() noexcept { return false; }

        FST_ALWAYS_INLINE void unlock() noexcept {   }

        FST_ALWAYS_INLINE handle get_handle() const noexcept { return nullptr; }

    };
#endif // __FST_WINDOWS__

    mutex::mutex() noexcept
    {
        fst_placement_new(_storage.data()) native();
    }

    mutex::~mutex() noexcept
    {
        _storage.data<native>()->~native();
    }

    void mutex::lock() noexcept
    {
        _storage.data<native>()->lock();
    }

    bool mutex::try_lock() noexcept
    {
        return _storage.data<native>()->try_lock();
    }

    void mutex::unlock() noexcept
    {
        _storage.data<native>()->unlock();
    }

    mutex::handle mutex::get_handle() const noexcept
    {
        return _storage.data<native>()->get_handle();
    }

FST_END_NAMESPACE
