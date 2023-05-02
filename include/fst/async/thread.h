///
/// MIT License
///
/// Copyright (c) 2023 Alexandre Arsenault
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#pragma once

#include "fst/common.h"
#include "fst/async/common.h"
#include "fst/pointer.h"

FST_BEGIN_SUB_NAMESPACE(async)

    class thread
    {
      public:
        //using data_type = void*;
        using process_callback = int (*)(thread*, void*);
        using release_callback = void (*)(thread*, void*);

        struct thread_data
        {
            process_callback process;
            release_callback release;
            void* data;
        };

        struct native;

        thread() noexcept;
        thread(const thread&) = delete;

        thread(thread_data tdata) noexcept;

        template <class _Fct>
        inline thread(_Fct&& fct) noexcept
            : thread(thread_data{ [](thread*, void* data)
                {
                    _Fct& f = *(_Fct*) (data);
                    f();
                    return 0;
                },
                [](thread*, void* data)
                {
                    _Fct& f = *(_Fct*) (data);
                    f.~_Fct();
                    __fst::aligned_deallocate(data);
                },
                fst_placement_new((_Fct*) __fst::aligned_allocate(sizeof(_Fct), alignof(_Fct))) _Fct(__fst::forward<_Fct>(fct)) })
        {}

        ~thread() noexcept;

        // types
        //class id;
        ////using native_handle_type = /* implementation-defined */;

        //// construct/copy/destroy
        //thread() noexcept;

        //template <class F, class... Args>
        //explicit thread(F&& f, Args&&... args);

        //~thread();
        //thread(const thread&) = delete;
        //thread(thread&&) noexcept;
        thread& operator=(const thread&) = delete;
        thread& operator=(thread&&) noexcept;

        //// members
        //void swap(thread&) noexcept;
        //bool joinable() const noexcept;
        //void join();
        //void detach();

        //id get_id() const noexcept;
        //native_handle_type native_handle();

        // static members
        //static unsigned int hardware_concurrency() noexcept;

        using native_pointer = __fst::unique_ptr<native, __fst::default_memory_zone, __fst::async_memory_category>;
        native_pointer _native;
        thread_data _thread_data;
    };
FST_END_SUB_NAMESPACE
