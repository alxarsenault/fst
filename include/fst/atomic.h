//
// MIT License
//
// Copyright (c) 2023 Alexandre Arsenault
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "fst/utility.h"
#include "fst/memory.h"

#if __FST_CLANG__ || __FST_GCC__
#define __FST_ATOMIC_GCC_INTRINSICS__ 1

#elif __FST_MSVC__
#define __FST_ATOMIC_MSVC_INTRINSICS__ 1

#elif __cplusplus >= 201103L
#define ATOMIC_USE_CPP11_ATOMIC
#include <atomic>
#else
#error Unsupported compiler / system.
#endif

#ifndef __FST_ATOMIC_GCC_INTRINSICS__
#define __FST_ATOMIC_GCC_INTRINSICS__ 0
#endif

#ifndef __FST_ATOMIC_MSVC_INTRINSICS__
#define __FST_ATOMIC_MSVC_INTRINSICS__ 0
#endif

#if __FST_ATOMIC_MSVC_INTRINSICS__

// Define which functions we need (don't include <intrin.h>).
extern "C" {
short _InterlockedIncrement16(short volatile*);
long _InterlockedIncrement(long volatile*);
__int64 _InterlockedIncrement64(__int64 volatile*);

short _InterlockedDecrement16(short volatile*);
long _InterlockedDecrement(long volatile*);
__int64 _InterlockedDecrement64(__int64 volatile*);

char _InterlockedExchange8(char volatile*, char);
short _InterlockedExchange16(short volatile*, short);
long __cdecl _InterlockedExchange(long volatile*, long);
__int64 _InterlockedExchange64(__int64 volatile*, __int64);

char _InterlockedCompareExchange8(char volatile*, char, char);
short _InterlockedCompareExchange16(short volatile*, short, short);
long __cdecl _InterlockedCompareExchange(long volatile*, long, long);
__int64 _InterlockedCompareExchange64(__int64 volatile*, __int64, __int64);
};

// Define which functions we want to use as inline intriniscs.
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedIncrement16)

#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedDecrement16)

#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange8)
#pragma intrinsic(_InterlockedCompareExchange16)

#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchange8)
#pragma intrinsic(_InterlockedExchange16)

#if defined(_M_X64)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedExchange64)
#endif // _M_X64

FST_BEGIN_NAMESPACE

    namespace msvc
    {
        template <typename T, size_t N = sizeof(T)>
        struct interlocked;

        template <typename T>
        struct interlocked<T, 1>
        {
            static inline T increment(T volatile* x) noexcept
            {
                // There's no _InterlockedIncrement8().
                char old_val, new_val;
                do
                {
                    old_val = static_cast<char>(*x);
                    new_val = old_val + static_cast<char>(1);
                } while (_InterlockedCompareExchange8(reinterpret_cast<volatile char*>(x), new_val, old_val) != old_val);
                return static_cast<T>(new_val);
            }

            static inline T decrement(T volatile* x) noexcept
            {
                // There's no _InterlockedDecrement8().
                char old_val, new_val;
                do
                {
                    old_val = static_cast<char>(*x);
                    new_val = old_val - static_cast<char>(1);
                } while (_InterlockedCompareExchange8(reinterpret_cast<volatile char*>(x), new_val, old_val) != old_val);
                return static_cast<T>(new_val);
            }

            static inline T compare_exchange(T volatile* x, T new_val, T expected_val) noexcept
            {
                return static_cast<T>(_InterlockedCompareExchange8(reinterpret_cast<volatile char*>(x), static_cast<char>(new_val), static_cast<char>(expected_val)));
            }

            static inline T exchange(T volatile* x, T new_val) noexcept
            {
                return static_cast<T>(_InterlockedExchange8(reinterpret_cast<volatile char*>(x), static_cast<char>(new_val)));
            }
        };

        template <typename T>
        struct interlocked<T, 2>
        {
            static inline T increment(T volatile* x) noexcept { return static_cast<T>(_InterlockedIncrement16(reinterpret_cast<volatile short*>(x))); }

            static inline T decrement(T volatile* x) noexcept { return static_cast<T>(_InterlockedDecrement16(reinterpret_cast<volatile short*>(x))); }

            static inline T compare_exchange(T volatile* x, T new_val, T expected_val) noexcept
            {
                return static_cast<T>(_InterlockedCompareExchange16(reinterpret_cast<volatile short*>(x), static_cast<short>(new_val), static_cast<short>(expected_val)));
            }

            static inline T exchange(T volatile* x, T new_val) noexcept
            {
                return static_cast<T>(_InterlockedExchange16(reinterpret_cast<volatile short*>(x), static_cast<short>(new_val)));
            }
        };

        template <typename T>
        struct interlocked<T, 4>
        {
            static inline T increment(T volatile* x) noexcept { return static_cast<T>(_InterlockedIncrement(reinterpret_cast<volatile long*>(x))); }

            static inline T decrement(T volatile* x) noexcept { return static_cast<T>(_InterlockedDecrement(reinterpret_cast<volatile long*>(x))); }

            static inline T compare_exchange(T volatile* x, T new_val, T expected_val) noexcept
            {
                return static_cast<T>(_InterlockedCompareExchange(reinterpret_cast<volatile long*>(x), static_cast<long>(new_val), static_cast<long>(expected_val)));
            }

            static inline T exchange(T volatile* x, T new_val) noexcept
            {
                return static_cast<T>(_InterlockedExchange(reinterpret_cast<volatile long*>(x), static_cast<long>(new_val)));
            }
        };

        template <typename T>
        struct interlocked<T, 8>
        {
            static inline T increment(T volatile* x) noexcept
            {
#if defined(_M_X64)
                return static_cast<T>(_InterlockedIncrement64(reinterpret_cast<volatile __int64*>(x)));
#else
                // There's no _InterlockedIncrement64() for 32-bit x86.
                __int64 old_val, new_val;
                do
                {
                    old_val = static_cast<__int64>(*x);
                    new_val = old_val + static_cast<__int64>(1);
                } while (_InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(x), new_val, old_val) != old_val);
                return static_cast<T>(new_val);
#endif // _M_X64
            }

            static inline T decrement(T volatile* x) noexcept
            {
#if defined(_M_X64)
                return static_cast<T>(_InterlockedDecrement64(reinterpret_cast<volatile __int64*>(x)));
#else
                // There's no _InterlockedDecrement64() for 32-bit x86.
                __int64 old_val, new_val;
                do
                {
                    old_val = static_cast<__int64>(*x);
                    new_val = old_val - static_cast<__int64>(1);
                } while (_InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(x), new_val, old_val) != old_val);
                return static_cast<T>(new_val);
#endif // _M_X64
            }

            static inline T compare_exchange(T volatile* x, T new_val, T expected_val) noexcept
            {
                return static_cast<T>(
                    _InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(x), static_cast<__int64>(new_val), static_cast<__int64>(expected_val)));
            }

            static inline T exchange(T volatile* x, T new_val) noexcept
            {
#if defined(_M_X64)
                return static_cast<T>(_InterlockedExchange64(reinterpret_cast<volatile __int64*>(x), static_cast<__int64>(new_val)));
#else
                // There's no _InterlockedExchange64 for 32-bit x86.
                __int64 old_val;
                do
                {
                    old_val = static_cast<__int64>(*x);
                } while (_InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(x), new_val, old_val) != old_val);
                return static_cast<T>(old_val);
#endif // _M_X64
            }
        };
    } // namespace msvc
FST_END_NAMESPACE
#endif

FST_BEGIN_NAMESPACE
    template <typename T>
    class atomic
    {
      public:
        static_assert(__fst::is_one_of((int) sizeof(T), 1, 2, 4, 8), "Only types of size 1, 2, 4 or 8 are supported");

        FST_ALWAYS_INLINE atomic() noexcept
            : _value(static_cast<T>(0))
        {}

        FST_ALWAYS_INLINE atomic(T value) noexcept
            : _value(value)
        {}

        atomic(const atomic&) = delete;
        atomic& operator=(const atomic&) = delete;

        /// @brief Performs an atomic increment operation (value + 1).
        /// @returns The new value of the atomic object.
        FST_ALWAYS_INLINE T operator++() noexcept
        {
#if __FST_ATOMIC_GCC_INTRINSICS__
            return __atomic_add_fetch(&_value, 1, __ATOMIC_SEQ_CST);
#elif __FST_ATOMIC_MSVC_INTRINSICS__
            return msvc::interlocked<T>::increment(&_value);
#else
            return ++_value;
#endif
        }

        /// @brief Performs an atomic decrement operation (value - 1).
        /// @returns The new value of the atomic object.
        FST_ALWAYS_INLINE T operator--() noexcept
        {
#if __FST_ATOMIC_GCC_INTRINSICS__
            return __atomic_sub_fetch(&_value, 1, __ATOMIC_SEQ_CST);
#elif __FST_ATOMIC_MSVC_INTRINSICS__
            return msvc::interlocked<T>::decrement(&_value);
#else
            return --_value;
#endif
        }

        FST_ALWAYS_INLINE T operator++(int) noexcept
        {
#if __FST_ATOMIC_GCC_INTRINSICS__
            T after = __atomic_add_fetch(&_value, 1, __ATOMIC_SEQ_CST);
#elif __FST_ATOMIC_MSVC_INTRINSICS__
            T after = msvc::interlocked<T>::increment(&_value);
#else
            T after = ++_value;

#endif
            return --after;
        }

        /// @brief Performs an atomic compare-and-swap (CAS) operation.
        ///
        /// The value of the atomic object is only updated to the new value if the
        /// old value of the atomic object matches @c expected_val.
        ///
        /// @param expected_val The expected value of the atomic object.
        /// @param new_val The new value to write to the atomic object.
        /// @returns True if new_value was written to the atomic object.
        FST_ALWAYS_INLINE bool compare_exchange(T& expected_val, T new_val) noexcept
        {
#if __FST_ATOMIC_GCC_INTRINSICS__
            T e = expected_val;
            return __atomic_compare_exchange_n(&_value, &e, new_val, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#elif __FST_ATOMIC_MSVC_INTRINSICS__
            const T old_val = msvc::interlocked<T>::compare_exchange(&_value, new_val, expected_val);
            if (old_val == expected_val) { return true; }
            else
            {
                expected_val = old_val;
                return false;
            }
#else
            T e = expected_val;
            return _value.compare_exchange_weak(e, new_val);
#endif
        }

        /// @brief Performs an atomic set operation.
        ///
        /// The value of the atomic object is unconditionally updated to the new
        /// value.
        ///
        /// @param new_val The new value to write to the atomic object.
        FST_ALWAYS_INLINE void store(T new_val) noexcept
        {
#if __FST_ATOMIC_GCC_INTRINSICS__
            __atomic_store_n(&_value, new_val, __ATOMIC_SEQ_CST);
#elif __FST_ATOMIC_MSVC_INTRINSICS__
            (void) msvc::interlocked<T>::exchange(&_value, new_val);
#else
            _value.store(new_val);
#endif
        }

        /// @returns the current value of the atomic object.
        /// @note Be careful about how this is used, since any operations on the
        /// returned value are inherently non-atomic.
        FST_NODISCARD FST_ALWAYS_INLINE T load() const noexcept
        {
#if __FST_ATOMIC_GCC_INTRINSICS__
            return __atomic_load_n(&_value, __ATOMIC_SEQ_CST);
#elif __FST_ATOMIC_MSVC_INTRINSICS__
            // TODO(m): Is there a better solution for MSVC?
            return _value;
#else
            return _value;
#endif
        }

        /// @brief Performs an atomic exchange operation.
        ///
        /// The value of the atomic object is unconditionally updated to the new
        /// value, and the old value is returned.
        ///
        /// @param new_val The new value to write to the atomic object.
        /// @returns the old value.
        FST_NODISCARD FST_ALWAYS_INLINE T exchange(T new_val) noexcept
        {
#if __FST_ATOMIC_GCC_INTRINSICS__
            return __atomic_exchange_n(&_value, new_val, __ATOMIC_SEQ_CST);
#elif __FST_ATOMIC_MSVC_INTRINSICS__
            return msvc::interlocked<T>::exchange(&_value, new_val);
#else
            return _value.exchange(new_val);
#endif
        }

        FST_ALWAYS_INLINE T operator=(T new_value) noexcept
        {
            store(new_value);
            return new_value;
        }

        FST_NODISCARD FST_ALWAYS_INLINE operator T() const noexcept { return load(); }

      private:
#if __FST_ATOMIC_GCC_INTRINSICS__ || __FST_ATOMIC_MSVC_INTRINSICS__
        T _value;
#else
        std::atomic<T> _value;
#endif

        //ATOMIC_DISALLOW_COPY(atomic)
    };

// Undef temporary defines.
#undef ATOMIC_USE_GCC_INTRINSICS
#undef __FST_ATOMIC_MSVC_INTRINSICS__
#undef ATOMIC_USE_CPP11_ATOMIC

FST_END_NAMESPACE
