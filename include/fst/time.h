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
#include "fst/utility.h"
#include "fst/memory_range.h"

#include <time.h>

FST_BEGIN_NAMESPACE

    ///
    class time_point
    {
      public:
        using value_type = int64_t;

        constexpr time_point() noexcept = default;
        constexpr time_point(const time_point&) noexcept = default;
        constexpr time_point(time_point&&) noexcept = default;

        FST_ALWAYS_INLINE constexpr time_point(value_type t) noexcept
            : _time(t)
            , _ns(0)
        {}

        FST_ALWAYS_INLINE constexpr time_point(value_type t, value_type ns) noexcept
            : _time(t)
            , _ns(ns)
        {}

        constexpr time_point& operator=(const time_point&) noexcept = default;
        constexpr time_point& operator=(time_point&&) noexcept = default;

        FST_NODISCARD FST_ALWAYS_INLINE constexpr value_type time_since_epoch() const noexcept { return _time; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr value_type nanoseconds() const noexcept { return _ns; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr value_type milliseconds() const noexcept { return _ns / 1000000; }

        FST_NODISCARD ::time_t to_time_t() const noexcept { return (::time_t) time_since_epoch(); }

        FST_ALWAYS_INLINE constexpr time_point& operator++() noexcept
        {
            ++_time;
            return *this;
        }

        FST_ALWAYS_INLINE constexpr time_point operator++(int) noexcept { return time_point{ _time++ }; }

        FST_ALWAYS_INLINE constexpr time_point& operator--() noexcept
        {
            --_time;
            return *this;
        }

        FST_ALWAYS_INLINE constexpr time_point operator--(int) noexcept { return time_point{ _time-- }; }

        friend __fst::output_stream<char>& operator<<(__fst::output_stream<char>& stream, time_point now);

      private:
        value_type _time = 0;
        value_type _ns = 0; // nanoseconds [0, 999999999]
    };

    ///
    struct system_clock
    {
        static time_point now() noexcept;

        FST_NODISCARD static ::time_t to_time_t(const time_point& t) noexcept { return (::time_t) t.time_since_epoch(); }
    };

    size_t time_to_string(__fst::memory_range<char, 13> buffer, time_point now, char sep = ':') noexcept;
    size_t time_to_string2(__fst::memory_range<char, 12> buffer, time_point now, char sep) noexcept;
    ;
FST_END_NAMESPACE
