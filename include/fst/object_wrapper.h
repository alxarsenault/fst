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
#include "fst/traits.h"

FST_BEGIN_NAMESPACE

    template <class T, class = void>
    struct object_wrapper;

    template <class T>
    struct object_wrapper<T, __fst::enable_if_t<__fst::is_empty_v<T>>> : T
    {
        using value_type = T;
        using reference = value_type;
        using const_reference = value_type;

        using value_type::value_type;
        using value_type::operator=;

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference get() noexcept { return value_type{}; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference get() const noexcept { return value_type{}; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator reference() noexcept { return value_type{}; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator const_reference() const noexcept { return value_type{}; }
    };

    template <class T>
    struct object_wrapper<T, __fst::enable_if_t<!__fst::is_empty_v<T>>>
    {
        using value_type = T;

        using reference = value_type&;
        using const_reference = const value_type&;

        FST_ALWAYS_INLINE constexpr object_wrapper() noexcept = default;

        template <class... _Args, __fst::enable_if_t<__fst::is_constructible_v<T, _Args...>, int> = 0>
        FST_ALWAYS_INLINE constexpr object_wrapper(_Args&&... args) noexcept
            : _value(__fst::forward<_Args>(args)...)
        {}

        template <class _U = value_type, __fst::enable_if_t<__fst::is_assignable_v<value_type, _U>, int> = 0>
        FST_ALWAYS_INLINE constexpr object_wrapper& operator=(_U&& other) noexcept
        {
            _value = __fst::forward<_U>(other);
            return *this;
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference get() noexcept { return _value; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference get() const noexcept { return _value; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator reference() noexcept { return _value; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator const_reference() const noexcept { return _value; }

      private:
        value_type _value;
    };

FST_END_NAMESPACE
