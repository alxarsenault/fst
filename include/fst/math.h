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
#include <math.h>

FST_BEGIN_NAMESPACE

    // clang-format off
    template <typename T> inline constexpr T zero = (T) 0.0;
    template <typename T> inline constexpr T one = (T) 1.0;
    template <typename T> inline constexpr T e = T(2.7182818284590452353602874713526624977572L);
    template <typename T> inline constexpr T pi = T(3.1415926535897932384626433832795028841972L);
    template <typename T> inline constexpr T half_pi = T(1.5707963267948966192313216916397514420986L);
    template <typename T> inline constexpr T sqrt_pi = T(1.7724538509055160272981674833411451827975L);
    template <typename T> inline constexpr T sqrt_half_pi = T(1.2533141373155002512078826424055226265035L);
    template <typename T> inline constexpr T two_pi = (T) 2.0 * pi<T>;
    template <typename T> inline constexpr T two_pi_2 = two_pi<T>* two_pi<T>;
    template <typename T> inline constexpr T one_over_pi = (T) 1.0 / pi<T>;
    template <typename T> inline constexpr T two_over_pi = (T) 2.0 / pi<T>;
    template <typename T> inline constexpr T one_over_pi_2 = (T) 1.0 / (pi<T> * pi<T>);
    template <typename T> inline constexpr T one_over_two_pi = (T) 1.0 / two_pi<T>;
    template <typename T> inline constexpr T pi_over_two = pi<T>*(T) 0.5;
    template <typename T> inline constexpr T pi_over_four = pi<T>*(T) 0.25;
    template <typename T> inline constexpr T sqrt_2 = T(1.4142135623730950488016887242096980785697L);
    template <typename T> inline constexpr T sqrt_2_over_2 = T(0.7071067811865476);
    template <typename T> inline constexpr T log_2 = T(0.6931471805599453094172321214581765680755L);
    template <typename T> inline constexpr T log_10 = T(2.3025850929940456840179914546843642076011L);
    template <typename T> inline constexpr T log_pi = T(1.1447298858494001741434273513530587116473L);
    template <typename T> inline constexpr T log_two_pi = T(1.8378770664093454835606594728112352797228L);
    template <typename T> inline constexpr T log_sqrt_two_pi = T(0.9189385332046727417803297364056176398614L);
    // clang-format on

    //
    //
    //

    template <typename T, __fst::enable_if_t<__fst::is_integral_v<T>>* = nullptr>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr T is_power_of_two(T v)
    {
        return v && !(v & (v - 1));
    }

    /// Returns the next power of two (in 64-bits) that is strictly greater than A.
    /// Returns zero on overflow.
    FST_NODISCARD FST_ALWAYS_INLINE constexpr uint64_t next_power_of_two(uint64_t v) noexcept
    {
        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);
        v |= (v >> 32);
        return v + 1;
    }

    FST_NODISCARD FST_ALWAYS_INLINE constexpr uint32_t power_of_two_bit_index(uint32_t v) noexcept
    {
        v = (v - 1) - (((v - 1) >> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
        return (((v + ((v >> 4) & 0xF0F0F0F)) * 0x1010101) >> 24);
    }

    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    template <typename T, __fst::enable_if_t<__fst::is_arithmetic_v<T>>* = nullptr>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr T round_to_power_of_two(T value) noexcept
    {
        // Compute the next highest power of 2 of 32-bit v.
        uint32_t v = static_cast<uint32_t>(value);
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    // https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
    FST_NODISCARD FST_ALWAYS_INLINE constexpr int32_t log2_of_power_of_two(uint32_t v)
    {
        constexpr int32_t multiply_debruijn_bit_position_2[32]
            = { 0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9 };

        return multiply_debruijn_bit_position_2[(uint32_t) (v * 0x077CB531U) >> (uint32_t) 27];
    }

    //
    //
    //

#define FST_DELARE_MATH_OP1(NAME)                                                                \
    template <typename T, __fst::enable_if_t<__fst::is_floating_point_v<T>, int> = 0>              \
    FST_NODISCARD FST_ALWAYS_INLINE T NAME(T x) noexcept                                         \
    {                                                                                            \
        if constexpr (__fst::is_same_v<float, T>) { return ::FST_CONCAT(NAME, f)(x); }            \
        else if constexpr (__fst::is_same_v<double, T>) { return ::NAME(x); }                     \
        else if constexpr (__fst::is_same_v<long double, T>) { return ::FST_CONCAT(NAME, l)(x); } \
        else { static_assert(__fst::always_false<T>, "Unsupported type"); }                       \
    }

#define FST_DELARE_MATH_OP2(NAME)                                                                   \
    template <typename T, __fst::enable_if_t<__fst::is_floating_point_v<T>, int> = 0>                 \
    FST_NODISCARD FST_ALWAYS_INLINE T NAME(T x, T y) noexcept                                       \
    {                                                                                               \
        if constexpr (__fst::is_same_v<float, T>) { return ::FST_CONCAT(NAME, f)(x, y); }            \
        else if constexpr (__fst::is_same_v<double, T>) { return ::NAME(x, y); }                     \
        else if constexpr (__fst::is_same_v<long double, T>) { return ::FST_CONCAT(NAME, l)(x, y); } \
        else { static_assert(__fst::always_false<T>, "Unsupported type"); }                          \
    }

    FST_DELARE_MATH_OP1(fabs)
    FST_DELARE_MATH_OP1(ceil)
    FST_DELARE_MATH_OP1(floor)
    FST_DELARE_MATH_OP1(trunc)
    FST_DELARE_MATH_OP1(round)
    FST_DELARE_MATH_OP2(fmod)
    FST_DELARE_MATH_OP2(fmin)
    FST_DELARE_MATH_OP2(fmax)
    FST_DELARE_MATH_OP2(fdim)
    FST_DELARE_MATH_OP1(exp)
    FST_DELARE_MATH_OP1(exp2)
    FST_DELARE_MATH_OP1(expm1)
    FST_DELARE_MATH_OP2(pow)
    FST_DELARE_MATH_OP1(sqrt)
    FST_DELARE_MATH_OP1(cbrt)
    FST_DELARE_MATH_OP1(sin)
    FST_DELARE_MATH_OP1(cos)
    FST_DELARE_MATH_OP1(tan)
    FST_DELARE_MATH_OP1(asin)
    FST_DELARE_MATH_OP1(acos)
    FST_DELARE_MATH_OP1(atan)
    FST_DELARE_MATH_OP2(atan2)
    FST_DELARE_MATH_OP1(sinh)
    FST_DELARE_MATH_OP1(cosh)
    FST_DELARE_MATH_OP1(tanh)
    FST_DELARE_MATH_OP1(asinh)
    FST_DELARE_MATH_OP1(acosh)
    FST_DELARE_MATH_OP1(atanh)
    FST_DELARE_MATH_OP1(log)
    FST_DELARE_MATH_OP1(log2)
    FST_DELARE_MATH_OP1(log10)
    FST_DELARE_MATH_OP1(log1p)

    template <class T>
    FST_NODISCARD FST_ALWAYS_INLINE T exp10(T x) noexcept
    {
        return __fst::pow(x, (T) 10);
    }

    namespace cxpr
    {
        template <class T>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T abs(T x) noexcept
        {
            return x < (T) 0 ? -x : x;
        }
    } // namespace cxpr

FST_END_NAMESPACE
