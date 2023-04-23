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
#include "fst/traits.h"

FST_BEGIN_NAMESPACE

    ///////////////////////////////////////////////////////////////////////////////
    template <class... _Args>
    struct type_tuple;
    template <class... _Args>
    struct tuple;
    template <class T1, class T2, bool BothEmpty = _FST::is_empty_v<T1>&& _FST::is_empty_v<T2>>
    struct pair;

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

    // indexed_type
    template <class T, size_t _Index>
    struct indexed_type
    {
        using type = T;
        static constexpr size_t index = 0;
    };
    template <class T, size_t _Index>
    using indexed_type_t = typename indexed_type<T, _Index>::type;
    template <class T, size_t _Index>
    FST_INLINE_VAR constexpr size_t indexed_type_index = indexed_type<T, _Index>::index;

    ///
    template <class T, size_t _Index, class = void>
    struct indexed_value : T
    {
        constexpr indexed_value() noexcept = default;
        inline constexpr indexed_value(const T&) noexcept {}
        inline constexpr T& get() noexcept { return *this; }
        inline constexpr const T& get() const noexcept { return *this; }
    };

    template <class T, size_t _Index>
    struct indexed_value<T, _Index, _FST::enable_if_t<_FST::is_empty_v<T>>> : T
    {
        constexpr indexed_value() noexcept = default;
        inline constexpr indexed_value(const T&) noexcept {}
        inline constexpr T& get() noexcept { return *this; }
        inline constexpr const T& get() const noexcept { return *this; }
    };

    template <class T, size_t _Index>
    struct indexed_value<T, _Index, _FST::enable_if_t<!_FST::is_empty_v<T>>>
    {
        constexpr indexed_value() noexcept = default;
        inline constexpr indexed_value(const T& v) noexcept
            : _value(v)
        {}
        inline constexpr T& get() noexcept { return _value; }
        inline constexpr const T& get() const noexcept { return _value; }

      private:
        T _value;
    };

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
                : indexed_value<T, 0>(_FST::forward<T>(t))
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
                : indexed_value_type(_FST::forward<T>(t))
                , tuple_imp<_Args...>(_FST::forward<_Args>(args)...)
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
                    return (_FST::maximum)(alignof(tuple_imp),
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
    struct tuple_size<tuple<Types...>> : _FST::integral_constant<size_t, sizeof...(Types)>
    {};
    template <class... Types>
    struct tuple_size<type_tuple<Types...>> : _FST::integral_constant<size_t, sizeof...(Types)>
    {};

    //
    template <class... _Args>
    struct tuple : detail::tuple_imp<_Args...>
    {
        using __self = tuple;
        static constexpr size_t size = sizeof...(_Args);

        constexpr tuple() noexcept = default;
        inline constexpr tuple(_Args&&... args) noexcept
            : detail::tuple_imp<_Args...>(_FST::forward<_Args>(args)...)
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
    };

    template <class T, class... _Args>
    struct type_tuple<T, _Args...>
        : indexed_type<T, sizeof...(_Args)>
        , type_tuple<_Args...>
    {};

    namespace detail
    {
        ///
        template <class T, bool _Empty = _FST::is_empty_v<T>>
        struct pair_first_value : T
        {
            constexpr pair_first_value() noexcept = default;
            inline constexpr pair_first_value(const T&) noexcept {}
            inline constexpr T& first() noexcept { return *this; }
            inline constexpr const T& first() const noexcept { return *this; }
        };

        template <class T>
        struct pair_first_value<T, false>
        {
            constexpr pair_first_value() noexcept = default;
            inline constexpr pair_first_value(const T& f) noexcept
                : _first(f)
            {}
            inline constexpr T& first() noexcept { return _first; }
            inline constexpr const T& first() const noexcept { return _first; }

          private:
            T _first;
        };

        ///
        template <class T, bool _Empty = _FST::is_empty_v<T>>
        struct pair_second_value : T
        {
            constexpr pair_second_value() noexcept = default;
            inline constexpr pair_second_value(const T&) noexcept {}
            inline constexpr T& second() noexcept { return *this; }
            inline constexpr const T& second() const noexcept { return *this; }
        };

        template <class T>
        struct pair_second_value<T, false>
        {
            constexpr pair_second_value() noexcept = default;
            inline constexpr pair_second_value(const T& s) noexcept
                : _second(s)
            {}
            inline constexpr pair_second_value(T&& s) noexcept
                : _second(_FST::move(s))
            {}
            inline constexpr T& second() noexcept { return _second; }
            inline constexpr const T& second() const noexcept { return _second; }

          private:
            T _second;
        };
    } // namespace detail

    ///
    template <class T1, class T2, bool BothEmpty>
    struct pair
        : detail::pair_first_value<T1>
        , detail::pair_second_value<T2>
    {
        constexpr pair() noexcept = default;
        inline constexpr pair(const T1& f, const T2& s) noexcept
            : detail::pair_first_value<T1>(f)
            , detail::pair_second_value<T2>(s)
        {}
    };

    FST_PRAGMA_PUSH()
    FST_PRAGMA_DISABLE_WARNING_MSVC(4172)
    template <class T1, class T2>
    struct pair<T1, T2, true>
    {
        inline constexpr pair() noexcept = default;
        inline constexpr pair(const T1&, const T2&) noexcept {}
        inline constexpr T1& first() noexcept
        {
            T1 value;
            return value;
        }
        inline constexpr const T1& first() const noexcept
        {
            T1 value;
            return value;
        }
        inline constexpr T2& second() noexcept { return _second_value; }
        inline constexpr const T2& second() const noexcept { return _second_value; }

        static constexpr T2 _second_value{};
    };

    FST_PRAGMA_POP()

    template <class T1, class T2>
    struct type_pair
    {
        using first = T1;
        using second = T2;
    };

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

    // is_pair
    template <class>
    struct is_pair : _FST::false_t
    {};
    template <class T1, class T2>
    struct is_pair<_FST::pair<T1, T2>> : _FST::true_t
    {};

    // is_tuple
    template <typename>
    struct is_tuple : _FST::false_t
    {};
    template <typename... Ts>
    struct is_tuple<_FST::tuple<Ts...>> : _FST::true_t
    {};

FST_END_NAMESPACE
