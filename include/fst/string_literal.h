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
#include "fst/string_view.h"

FST_BEGIN_NAMESPACE

    /// string_literal.
    template <size_t N>
    class string_literal
    {
      public:
        using value_type = char;
        using size_type = size_t;
        using view_type = __fst::string_view;

        constexpr string_literal(const char (&str)[N]) noexcept
        {
            for (size_type i = 0; i < N; i++)
            {
                value[i] = str[i];
            }
        }

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type size() noexcept { return N; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const char* c_str() const noexcept { return &value[0]; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr view_type view() const noexcept { return view_type(&value[0], N - 1); }

        value_type value[N];

      private:
        template <size_t Start, size_t Count, size_t... Indices>
        inline constexpr string_literal<Count + 1> substr_impl(__fst::index_sequence<Indices...>) const noexcept
        {
            return string_literal<Count + 1>({ value[Start + Indices]..., 0 });
        }

      public:
        template <size_t Start, size_t Count>
        inline constexpr string_literal<Count + 1> substr() const noexcept
        {
            return substr_impl<Start, Count>(__fst::make_index_sequence<Count>());
        }
    };

    template <size_t N>
    string_literal(const char(&str)[N]) -> string_literal<N>;

    template <string_literal A, string_literal B>
    struct is_s_same : __fst::false_t
    {};

    template <string_literal A>
    struct is_s_same<A, A> : __fst::true_t
    {};

    template <string_literal A, string_literal B>
    inline constexpr bool string_literal_compare() noexcept
    {
        return is_s_same<A, B>::value;
    }

    namespace detail
    {
        template <string_literal A, string_literal B, size_t... AIndices, size_t... BIndices>
        inline constexpr auto concat_impl(__fst::index_sequence<AIndices...>, __fst::index_sequence<BIndices...>) noexcept
        {
            return string_literal({ A.value[AIndices]..., B.value[BIndices]... });
        }

        template <string_literal Separator, string_literal A, string_literal B, size_t... SIndices, size_t... AIndices, size_t... BIndices>
        inline constexpr auto join_impl(__fst::index_sequence<SIndices...>, __fst::index_sequence<AIndices...>, __fst::index_sequence<BIndices...>) noexcept
        {
            return string_literal({ A.value[AIndices]..., Separator.value[SIndices]..., B.value[BIndices]... });
        }
    } // namespace detail.

    template <string_literal A, string_literal B, string_literal... Cs>
    inline constexpr auto string_literal_concat() noexcept
    {
        if constexpr (sizeof...(Cs) == 0) { return detail::concat_impl<A, B>(__fst::make_index_sequence<A.size() - 1>(), __fst::make_index_sequence<B.size()>()); }
        else { return concat<detail::concat_impl<A, B>(__fst::make_index_sequence<A.size() - 1>(), __fst::make_index_sequence<B.size()>()), Cs...>(); }
    }

    template <string_literal Separator, string_literal A, string_literal B, string_literal... Cs>
    inline constexpr auto string_literal_join() noexcept
    {
        if constexpr (sizeof...(Cs) == 0)
        {
            return detail::join_impl<Separator, A, B>(
                __fst::make_index_sequence<Separator.size() - 1>(), __fst::make_index_sequence<A.size() - 1>(), __fst::make_index_sequence<B.size()>());
        }
        else
        {
            return join<Separator,
                detail::join_impl<Separator, A, B>(
                    __fst::make_index_sequence<Separator.size() - 1>(), __fst::make_index_sequence<A.size() - 1>(), __fst::make_index_sequence<B.size()>()),
                Cs...>();
        }
    }
FST_END_NAMESPACE
