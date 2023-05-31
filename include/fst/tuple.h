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
#include "fst/indexed_value.h"

FST_BEGIN_NAMESPACE

    ///////////////////////////////////////////////////////////////////////////////
    template <class... _Args>
    struct type_tuple;

    template <class... _Args>
    struct tuple;

    // tuple_element
    template <size_t I, class T>
    struct tuple_element;

    template <size_t I, class T>
    using tuple_element_t = typename tuple_element<I, T>::type;

    // type_tuple_element
    template <size_t I, class T>
    using type_tuple_element = tuple_element<I, T>;

    template <size_t I, class T>
    using type_tuple_element_t = typename type_tuple_element<I, T>::type;

    // tuple_size
    template <class... Types>
    struct tuple_size;

    template<class ...Types>
    FST_INLINE_VAR constexpr size_t tuple_size_v = tuple_size<Types...>::value;

    template <class... Ts>
    struct to_tuple;

    template <class... Ts>
    struct to_type_tuple;

    template <class... Types>
    using type_tuple_size = tuple_size<Types...>;

    template <class... Ts>
    using to_tuple_t = typename to_tuple<Ts...>::type;

    template <class... Ts>
    using to_type_tuple_t = typename to_type_tuple<Ts...>::type;

    // merged_tuple
    template <class...>
    struct merged_tuple;

    template <class... Args1, class... Args2>
    struct merged_tuple<tuple<Args1...>, tuple<Args2...>>
    {
        using type = tuple<Args1..., Args2...>;
    };

    template <class... Args1, class... Args2, class... Tail>
    struct merged_tuple<tuple<Args1...>, tuple<Args2...>, Tail...>
    {
        using type = typename merged_tuple<tuple<Args1..., Args2...>, Tail...>::type;
    };

    template <class... Args>
    using merged_tuple_t = typename merged_tuple<Args...>::type;

    namespace detail
    {
        template <class... _Args>
        struct tuple_imp;

        template <class T>
        struct tuple_imp<T> : indexed_value<T, 0>
        {
            static constexpr size_t index = 0;
            constexpr tuple_imp() noexcept = default;

            inline constexpr tuple_imp(T&& t) noexcept
                : indexed_value<T, 0>(__fst::forward<T>(t))
            {}

            template <size_t _Index>
            static inline constexpr size_t offset_of() noexcept
            {
                return 0;
            }
        };

        template <class T, class... _Args>
        struct tuple_imp<T, _Args...>
            : indexed_value<T, sizeof...(_Args)>
            , tuple_imp<_Args...>
        {
            static constexpr size_t index = sizeof...(_Args);
            static constexpr size_t offset = sizeof...(_Args);
            using indexed_value_type = indexed_value<T, sizeof...(_Args)>;

            constexpr tuple_imp() noexcept = default;

            inline constexpr tuple_imp(T&& t, _Args&&... args) noexcept
                : indexed_value_type(__fst::forward<T>(t))
                , tuple_imp<_Args...>(__fst::forward<_Args>(args)...)
            {}

            /*template<size_t _Index>
    static inline constexpr size_t offset_of() noexcept
    {
            if constexpr (index == _Index)
            {
                    return 0;
            }
            else
            {
                    return (__fst::maximum)(alignof(tuple_imp),
    sizeof(indexed_value_type)) + tuple_imp<_Args...>::template
    offset_of<_Index>();
            }
    }*/
        };
    } // namespace detail

    // tuple_element
    template <size_t I, class Head, class... Tail>
    struct tuple_element<I, tuple<Head, Tail...>> : tuple_element<I - 1, tuple<Tail...>>
    {};

    template <class Head, class... Tail>
    struct tuple_element<0, tuple<Head, Tail...>>
    {
        using type = Head;
    };

    template <size_t I, class Head, class... Tail>
    struct tuple_element<I, type_tuple<Head, Tail...>> : tuple_element<I - 1, type_tuple<Tail...>>
    {};

    template <class Head, class... Tail>
    struct tuple_element<0, type_tuple<Head, Tail...>>
    {
        using type = Head;
    };

    // tuple_size
    template <class... Types>
    struct tuple_size<tuple<Types...>> : __fst::integral_constant<size_t, sizeof...(Types)>
    {};

    template <class... Types>
    struct tuple_size<type_tuple<Types...>> : __fst::integral_constant<size_t, sizeof...(Types)>
    {};

    //
    template <class... _Args>
    struct tuple : detail::tuple_imp<_Args...>
    {
        using __self = tuple;
        static constexpr size_t size = sizeof...(_Args);

        constexpr tuple() noexcept = default;

        inline constexpr tuple(_Args&&... args) noexcept
            : detail::tuple_imp<_Args...>(__fst::forward<_Args>(args)...)
        {}

        template <size_t _Index>
        static inline constexpr size_t offset_of() noexcept
        {
            return detail::tuple_imp<_Args...>::template offset_of<size - _Index - 1>();
        }

        template <size_t _Index>
        inline constexpr tuple_element_t<_Index, tuple>& get() noexcept
        {
            return indexed_value<tuple_element_t<_Index, tuple>, size - _Index - 1>::get();
        }

        template <size_t _Index>
        inline constexpr const tuple_element_t<_Index, tuple>& get() const noexcept
        {
            return indexed_value<tuple_element_t<_Index, tuple>, size - _Index - 1>::get();
        }

        template <size_t _Index, class _T>
        inline constexpr void set(_T&& value) noexcept
        {
            indexed_value<tuple_element_t<_Index, tuple>, size - _Index - 1>::get() = __fst::forward<_T>(value);
        }
    };

    template <class T, class... _Args>
    struct type_tuple<T, _Args...>
        : indexed_type<T, sizeof...(_Args)>
        , type_tuple<_Args...>
    {};

    // to_tuple
    template <class... Ts>
    struct to_tuple
    {
        using type = tuple<Ts...>;
    };

    template <class... Ts>
    struct to_tuple<type_tuple<Ts...>>
    {
        using type = tuple<Ts...>;
    };

    template <class... Ts>
    struct to_tuple<tuple<Ts...>>
    {
        using type = tuple<Ts...>;
    };

    template <class... Ts>
    struct to_tuple<__fst::type_list<Ts...>>
    {
        using type = tuple<Ts...>;
    };

    // to_type_tuple
    template <class... Ts>
    struct to_type_tuple
    {
        using type = type_tuple<Ts...>;
    };
    
    template <class... Ts>
    struct to_type_tuple<type_tuple<Ts...>>
    {
        using type = type_tuple<Ts...>;
    };

    template <class... Ts>
    struct to_type_tuple<tuple<Ts...>>
    {
        using type = type_tuple<Ts...>;
    };

    // is_tuple
    template <typename>
    struct is_tuple : __fst::false_t
    {};

    template <typename... Ts>
    struct is_tuple<__fst::tuple<Ts...>> : __fst::true_t
    {};

FST_END_NAMESPACE
