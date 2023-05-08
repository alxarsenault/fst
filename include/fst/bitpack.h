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
#include "fst/array.h"

/// @fst core
/// @{
FST_BEGIN_NAMESPACE

    //
    template <size_t Pos, size_t Len>
    FST_NODISCARD inline constexpr size_t bitmask() noexcept
    {
        static_assert((Pos + Len) <= (sizeof(size_t) * 8), "Out of range mask");
        return (~(~0ull << Len) << Pos);
    }

    //
    FST_NODISCARD inline constexpr size_t bitmask(size_t Pos, size_t Len) noexcept
    {
        fst_assert((Pos + Len) <= (sizeof(size_t) * 8), "Out of range mask");
        return (~(~0ull << Len) << Pos);
    }

    template <class _T, size_t _Len>
    struct bitpack_t
    {
        static_assert(__fst::is_integral_v<_T>, "wronf type");
        using type = _T;
        static constexpr size_t len = _Len;

        static_assert(len <= __fst::bitsize<type>, "wronf type");

        FST_NODISCARD static constexpr bool in_range(type v) noexcept { return v >= (bitpack_t::min)() && v <= (bitpack_t::max)(); }

        FST_NODISCARD static constexpr type(min)() noexcept
        {
            if constexpr (__fst::is_signed_v<type> && __fst::bitsize<type> == len) { return (__fst::numeric_limits<type>::min)(); }
            else { return 0; }
        }

        FST_NODISCARD static constexpr type(max)() noexcept
        {
            if constexpr (__fst::is_signed_v<type>)
            {
                if constexpr (__fst::bitsize<type> == len)
                    return (__fst::numeric_limits<type>::max)();
                else
                    return (type) ((1 << (len - 1)) - 1);
            }
            else
                return (type) (uint64_t(uint64_t(1) << uint64_t(len)) - 1);
        }
    };

    template <class _SizeType, class... _Args>
    class bitpack
    {
      private:
        template <class T>
        static constexpr size_t type_bitlen = T::len;

        static constexpr __fst::array<size_t, sizeof...(_Args)> sizes = { type_bitlen<_Args>... };

        static constexpr __fst::array<size_t, sizeof...(_Args)> offsets = []()
        {
            __fst::array<size_t, sizeof...(_Args)> arr = { 0 };
            for (size_t i = 1; i < arr.size(); i++)
            {
                arr[i] = sizes[i - 1] + arr[i - 1];
            }
            return arr;
        }();

        static constexpr __fst::array<size_t, sizeof...(_Args)> masks = []()
        {
            __fst::array<size_t, sizeof...(_Args)> arr = { 0 };
            for (size_t i = 0; i < arr.size(); i++)
            {
                arr[i] = __fst::bitmask(offsets[i], sizes[i]);
            }
            return arr;
        }();

      public:
        using size_type = _SizeType;

        using types = __fst::type_list<_Args...>;

        using value_types = __fst::type_list<typename _Args::type...>;

        template <size_t I>
        using type_at = types::template type_at<I>;

        template <size_t I>
        using value_type_at = value_types::template type_at<I>;

        static constexpr size_t bit_length = (type_bitlen<_Args> + ...);
        static_assert(bit_length <= __fst::bitsize<size_type>, "bit_length should be <= bitsize<size_type>");

        constexpr bitpack() noexcept = default;

        ///
        FST_ALWAYS_INLINE bitpack(typename _Args::type... values) noexcept
            : bitpack(values..., __fst::make_index_sequence<sizeof...(_Args)>{})
        {}

        ///
        template <size_t I>
        FST_NODISCARD FST_ALWAYS_INLINE void set(value_type_at<I> v) noexcept
        {
            static_assert(I < sizeof...(_Args), "Wronf idsdsdsdsdsng");
            fst_assert(type_at<I>::in_range(v));
            _value = (_value & ~masks[I]) | size_type((uint64_t(v) << offsets[I]) & masks[I]);
        }

        ///
        template <size_t I>
        FST_NODISCARD FST_ALWAYS_INLINE value_type_at<I> get() const noexcept
        {
            return static_cast<value_type_at<I>>((_value & masks[I]) >> offsets[I]);
        }

        ///
        FST_NODISCARD FST_ALWAYS_INLINE size_type& value() noexcept { return _value; }
        FST_NODISCARD FST_ALWAYS_INLINE size_type value() const noexcept { return _value; }

      private:
        size_type _value = 0;

        ///
        template <size_t... I>
        FST_ALWAYS_INLINE bitpack(typename _Args::type... values, __fst::index_sequence<I...>) noexcept
        {
            (set<I>(values), ...);
        }
    };

FST_END_NAMESPACE
/// @}
