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

#include "fst/common.h"
#include "fst/stream.h"

FST_BEGIN_NAMESPACE

    __fst::output_stream<char>& trace_stream(size_t, const char*, const __fst::source_location&, size_t) noexcept;

    template <typename T>
    using has_name_t = decltype(T::name);

    struct default_trace_section
    {
        static constexpr const char* name = "default";
    };

    template <class T>
    struct is_trace_section : __fst::is_detected<detail::has_name_t, T>
    {};

    template <size_t _Level, class _Section = default_trace_section>
    class tracer
    {
      public:
        using stream_type = __fst::output_stream<char>;
        using sep_type = __fst::stream_detail::stream_space_t;
        using end_type = __fst::stream_detail::stream_endl_t;
        static constexpr size_t level = _Level;

        static_assert(level <= FST_TRACE_LEVEL, "trace level cannot exceed FST_TRACE_LEVEL");

        inline tracer(const __fst::source_location& loc = __fst::source_location::current()) noexcept
            : _loc{ loc }
        {}

        template <typename... Args>
        inline void operator()(Args&&... args) const noexcept
        {
            basic_print<stream_type, sep_type, end_type>(__fst::trace_stream(level, _Section::name, _loc, sizeof...(args)), args..., __fst::endl);
        }

      private:
        __fst::source_location _loc;
    };

#if FST_TRACE_LEVEL > 0
#define fst_trace(LEVEL, SECTION, ...) FST_NAMESPACE::tracer<LEVEL, SECTION>()(__VA_ARGS__)
#else
#define fst_trace(...) FST_UNUSED(__VA_ARGS__)
#endif

#if FST_TRACE_LEVEL >= 1
#define fst_trace_l1(...) fst_trace(1, FST_NAMESPACE::default_trace_section, __VA_ARGS__)
#else
#define fst_trace_l1(...) FST_UNUSED(__VA_ARGS__)
#endif

#if FST_TRACE_LEVEL >= 2
#define fst_trace_l2(...) fst_trace(2, FST_NAMESPACE::default_trace_section, __VA_ARGS__)
#else
#define fst_trace_l2(...) FST_UNUSED(__VA_ARGS__)
#endif

#if FST_TRACE_LEVEL >= 3
#define fst_trace_l3(...) fst_trace(3, FST_NAMESPACE::default_trace_section, __VA_ARGS__)
#else
#define fst_trace_l3(...) FST_UNUSED(__VA_ARGS__)
#endif

#if FST_TRACE_LEVEL >= 4
#define fst_trace_l4(...) fst_trace(4, FST_NAMESPACE::default_trace_section, __VA_ARGS__)
#else
#define fst_trace_l4(...) FST_UNUSED(__VA_ARGS__)
#endif

#if FST_TRACE_LEVEL >= 5
#define fst_trace_l5(...) fst_trace(5, FST_NAMESPACE::default_trace_section, __VA_ARGS__)
#else
#define fst_trace_l5(...) FST_UNUSED(__VA_ARGS__)
#endif

FST_END_NAMESPACE
