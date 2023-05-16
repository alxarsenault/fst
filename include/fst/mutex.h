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
#ifndef _FST_MUTEX_H // include guard for 3rd party interop
#define _FST_MUTEX_H
//#pragma system_header

#include "fst/common.h"
#include "fst/traits.h"
#include "fst/aligned_storage.h"
#include "fst/atomic.h"

//#define MacroStr(x)   #x
//#define MacroStr2(x)  MacroStr(x)
//#define Message(desc) __pragma(message(__FILE__ "(" FST_STRINGIFY(__LINE__) ") :" #desc))

FST_TODO("BINFDLJ")
FST_BEGIN_NAMESPACE

    class mutex
    {
      public:
        using handle = void*;

        mutex() noexcept;
        mutex(const mutex&) = delete;
        mutex(mutex&&) = delete;

        ~mutex() noexcept;

        mutex& operator=(const mutex&) = delete;
        mutex& operator=(mutex&&) = delete;

        void lock() noexcept;

        bool try_lock() noexcept;

        void unlock() noexcept;

        handle get_handle() const noexcept;

      private:
        struct native;
        __fst::aligned_storage<64, 8> _storage;
    };

    // https://rigtorp.se/spinlock/
    class spin_lock
    {
      public:
        inline void lock() noexcept
        {
            for (;;)
            {
                // Optimistically assume the lock is free on the first try.
                if (!_lock.exchange(true)) { break; }

                // Wait for lock to be released without generating cache misses.
                while (_lock.load())
                {
                    // Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
                    // hyper-threads
                    FST_NOP();
                }
            }
        }

        inline bool try_lock() noexcept
        {
            // First do a relaxed load to check if lock is free in order to prevent
            // unnecessary cache misses if someone does while(!try_lock())
            return !_lock.load() && !_lock.exchange(true);
        }

        inline void unlock() noexcept { _lock.store(false); }

      private:
        __fst::atomic<bool> _lock = false;
    };

FST_END_NAMESPACE
#endif // _FST_MUTEX_H

//#if __FST_WINDOWS__
//#ifdef _WINDOWS_
////#error DJKSLJDSK
//#pragma message("_M_IX86 >= 500")
//#endif
//#endif
//#pragma message("_M_IX86 >= 500")
//#pragma message (__FILE__ "[" STRING(__LINE__) "]: test")