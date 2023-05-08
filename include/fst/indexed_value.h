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
#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    /// indexed_type
    template <class T, size_t _Index>
    struct indexed_type
    {
        using type = T;
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t index() noexcept { return _Index; }
    };

    ///
    template <class T, size_t _Index>
    using indexed_type_t = typename indexed_type<T, _Index>::type;

    ///
    template <class T, size_t _Index>
    FST_INLINE_VAR constexpr size_t indexed_type_index = indexed_type<T, _Index>::index();

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

      protected:
        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference first() noexcept { return value_type{}; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference first() const noexcept { return value_type{}; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference second() noexcept { return value_type{}; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference second() const noexcept { return value_type{}; }
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

      protected:
        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference first() noexcept { return _value; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference first() const noexcept { return _value; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference second() noexcept { return _value; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference second() const noexcept { return _value; }

      private:
        value_type _value;
    };

    template <class T, size_t _Index, class = void>
    struct indexed_value;

    template <class T, size_t _Index>
    struct indexed_value<T, _Index, __fst::enable_if_t<__fst::is_empty_v<T>>>
    {
        using value_type = T;
        using reference = value_type;
        using const_reference = value_type;

        constexpr indexed_value() noexcept = default;

        template <class... _Args, __fst::enable_if_t<__fst::is_constructible_v<T, _Args...> && (sizeof...(_Args) > 0), int> = 0>
        FST_ALWAYS_INLINE constexpr indexed_value(_Args&&...) noexcept
        {}

        template <class _U = value_type, __fst::enable_if_t<__fst::is_assignable_v<value_type, _U>, int> = 0>
        FST_ALWAYS_INLINE constexpr indexed_value& operator=(_U&&) noexcept
        {
            return *this;
        }

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t index() noexcept { return _Index; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference get() noexcept { return value_type{}; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference get() const noexcept { return value_type{}; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator reference() noexcept { return value_type{}; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator const_reference() const noexcept { return value_type{}; }
    };

    template <class T, size_t _Index>
    struct indexed_value<T, _Index, __fst::enable_if_t<!__fst::is_empty_v<T>>>
    {
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;

        constexpr indexed_value() noexcept = default;

        template <class... _Args, __fst::enable_if_t<__fst::is_constructible_v<T, _Args...> && (sizeof...(_Args) > 0), int> = 0>
        FST_ALWAYS_INLINE constexpr indexed_value(_Args&&... args) noexcept
            : _value(__fst::forward<_Args>(args)...)
        {}

        template <class _U = value_type, __fst::enable_if_t<__fst::is_assignable_v<value_type, _U>, int> = 0>
        FST_ALWAYS_INLINE constexpr indexed_value& operator=(_U&& other) noexcept
        {
            _value = __fst::forward<_U>(other);
            return *this;
        }

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t index() noexcept { return _Index; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr value_type& get() noexcept { return _value; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const value_type& get() const noexcept { return _value; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator value_type&() noexcept { return _value; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator const value_type&() const noexcept { return _value; }

      private:
        value_type _value;
    };

    ///
    template <class T, size_t _Index>
    using indexed_value_t = typename indexed_value<T, _Index>::type;

    ///
    template <class T, size_t _Index>
    FST_INLINE_VAR constexpr size_t indexed_value_index = indexed_value<T, _Index>::index();

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
