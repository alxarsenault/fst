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
#include "fst/indexed_value.h"

FST_BEGIN_NAMESPACE

    struct default_construct_t
    {};

    struct variadic_separator_t
    {};

    struct variadic_first_separator_t
    {};

    template <class T1, class T2>
    struct pair
        : __fst::indexed_value<T1, 0>
        , __fst::indexed_value<T2, 1>
    {
        using first_type = __fst::indexed_value<T1, 0>;
        using first_reference = typename first_type::reference;
        using first_const_reference = typename first_type::const_reference;

        using second_type = __fst::indexed_value<T2, 1>;
        using second_reference = typename second_type::reference;
        using second_const_reference = typename second_type::const_reference;

        constexpr pair() noexcept = default;

        inline constexpr pair(__fst::default_construct_t, __fst::default_construct_t) noexcept
            : first_type()
            , second_type()
        {}

        inline constexpr pair(__fst::default_construct_t, const second_type& s) noexcept
            : first_type()
            , second_type(s)
        {}

        inline constexpr pair(__fst::default_construct_t, second_type&& s) noexcept
            : first_type()
            , second_type(__fst::move(s))
        {}

        inline constexpr pair(const first_type& f, __fst::default_construct_t) noexcept
            : first_type(f)
            , second_type()
        {}

        inline constexpr pair(first_type&& f, __fst::default_construct_t) noexcept
            : first_type(__fst::move(f))
            , second_type()
        {}

        inline constexpr pair(first_type&& f, second_type&& s) noexcept
            : first_type(__fst::move(f))
            , second_type(__fst::move(s))
        {}

        inline constexpr pair(const first_type& f, const second_type& s) noexcept
            : first_type(f)
            , second_type(s)
        {}

        template <class... _Args2>
        inline constexpr pair(__fst::default_construct_t, _Args2&&... args2) noexcept
            : first_type()
            , second_type(__fst::forward<_Args2>(args2)...)

        {}

        template <class... _Args>
        inline constexpr pair(_Args&&... args, __fst::default_construct_t) noexcept
            : first_type(__fst::forward<_Args>(args)...)
            , second_type()

        {}

        FST_NODISCARD FST_ALWAYS_INLINE constexpr first_reference first() noexcept { return first_type::get(); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr first_const_reference first() const noexcept { return first_type::get(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr second_reference second() noexcept { return second_type::get(); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr second_const_reference second() const noexcept { return second_type::get(); }
    };

    template <class T1, class T2>
    struct type_pair
    {
        using first = T1;
        using second = T2;
    };

    // is_pair
    template <class>
    struct is_pair : __fst::false_t
    {};

    template <class T1, class T2>
    struct is_pair<__fst::pair<T1, T2>> : __fst::true_t
    {};
FST_END_NAMESPACE
