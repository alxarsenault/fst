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
//#include "fst/array.h"
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


    /* #define FMT_POWERS_OF_10(factor)                                             \
  factor * 10, (factor)*100, (factor)*1000, (factor)*10000, (factor)*100000, \
      (factor)*1000000, (factor)*10000000, (factor)*100000000,               \
      (factor)*1000000000*/

    //template <typename T> FMT_CONSTEXPR auto count_digits_fallback(T n) -> int {
    //  int count = 1;
    //  for (;;) {
    //    // Integer division is slow so do it for a group of four digits instead
    //    // of for every digit. The idea comes from the talk by Alexandrescu
    //    // "Three Optimization Tips for C++". See speed-test for a comparison.
    //    if (n < 10) return count;
    //    if (n < 100) return count + 1;
    //    if (n < 1000) return count + 2;
    //    if (n < 10000) return count + 3;
    //    n /= 10000u;
    //    count += 4;
    //  }

    //}
    //

    //// Compares two characters for equality.
    //template <typename Char> auto equal2(const Char* lhs, const char* rhs) -> bool {
    //  return lhs[0] == Char(rhs[0]) && lhs[1] == Char(rhs[1]);
    //}
    //inline auto equal2(const char* lhs, const char* rhs) -> bool {
    //  return memcmp(lhs, rhs, 2) == 0;
    //}
    //
    //// Copies two characters from src to dst.
    //template <typename Char>
    //FMT_CONSTEXPR20 FMT_INLINE void copy2(Char* dst, const char* src) {
    //  if (!is_constant_evaluated() && sizeof(Char) == sizeof(char)) {
    //    memcpy(dst, src, 2);
    //    return;
    //  }
    //  *dst++ = static_cast<Char>(*src++);
    //  *dst = static_cast<Char>(*src);
    //}
    //
    //template <typename Iterator> struct format_decimal_result {
    //  Iterator begin;
    //  Iterator end;
    //};
    //
    //// Formats a decimal unsigned integer value writing into out pointing to a
    //// buffer of specified size. The caller must ensure that the buffer is large
    //// enough.
    //template <typename Char, typename UInt>
    //FMT_CONSTEXPR20 auto format_decimal(Char* out, UInt value, int size)
    //    -> format_decimal_result<Char*> {
    //  FMT_ASSERT(size >= count_digits(value), "invalid digit count");
    //  out += size;
    //  Char* end = out;
    //  while (value >= 100) {
    //    // Integer division is slow so do it for a group of two digits instead
    //    // of for every digit. The idea comes from the talk by Alexandrescu
    //    // "Three Optimization Tips for C++". See speed-test for a comparison.
    //    out -= 2;
    //    copy2(out, digits2(static_cast<size_t>(value % 100)));
    //    value /= 100;
    //  }
    //  if (value < 10) {
    //    *--out = static_cast<Char>('0' + value);
    //    return {out, end};
    //  }
    //  out -= 2;
    //  copy2(out, digits2(static_cast<size_t>(value)));
    //  return {out, end};
    //}
    //
    //template <typename Char, typename UInt, typename Iterator,
    //          FMT_ENABLE_IF(!std::is_pointer<remove_cvref_t<Iterator>>::value)>
    //FMT_CONSTEXPR inline auto format_decimal(Iterator out, UInt value, int size)
    //    -> format_decimal_result<Iterator> {
    //  // Buffer is large enough to hold all digits (digits10 + 1).
    //  Char buffer[digits10<UInt>() + 1] = {};
    //  auto end = format_decimal(buffer, value, size).end;
    //  return {out, detail::copy_str_noinline<Char>(buffer, end, out)};
    //}
    //
    //template <unsigned BASE_BITS, typename Char, typename UInt>
    //FMT_CONSTEXPR auto format_uint(Char* buffer, UInt value, int num_digits,
    //                               bool upper = false) -> Char* {
    //  buffer += num_digits;
    //  Char* end = buffer;
    //  do {
    //    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    //    unsigned digit = static_cast<unsigned>(value & ((1 << BASE_BITS) - 1));
    //    *--buffer = static_cast<Char>(BASE_BITS < 4 ? static_cast<char>('0' + digit)
    //                                                : digits[digit]);
    //  } while ((value >>= BASE_BITS) != 0);
    //  return end;
    //}
    //
    //template <unsigned BASE_BITS, typename Char, typename It, typename UInt>
    //inline auto format_uint(It out, UInt value, int num_digits, bool upper = false)
    //    -> It {
    //  if (auto ptr = to_pointer<Char>(out, to_unsigned(num_digits))) {
    //    format_uint<BASE_BITS>(ptr, value, num_digits, upper);
    //    return out;
    //  }
    //  // Buffer should be large enough to hold all digits (digits / BASE_BITS + 1).
    //  char buffer[num_bits<UInt>() / BASE_BITS + 1];
    //  format_uint<BASE_BITS>(buffer, value, num_digits, upper);
    //  return detail::copy_str_noinline<Char>(buffer, buffer + num_digits, out);
    //}
    // It is a separate function rather than a part of count_digits to workaround
    // the lack of static constexpr in constexpr functions.
    //inline auto do_count_digits(uint64_t n) -> int {
    //  // This has comparable performance to the version by Kendall Willets
    //  // (https://github.com/fmtlib/format-benchmark/blob/master/digits10)
    //  // but uses smaller tables.
    //  // Maps bsr(n) to ceil(log10(pow(2, bsr(n) + 1) - 1)).
    //  static constexpr uint8_t bsr2log10[] = {
    //      1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
    //      6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10,
    //      10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
    //      15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};
    //  auto t = bsr2log10[FMT_BUILTIN_CLZLL(n | 1) ^ 63];
    //  static constexpr const uint64_t zero_or_powers_of_10[] = {
    //      0, 0, FMT_POWERS_OF_10(1U), FMT_POWERS_OF_10(1000000000ULL),
    //      10000000000000000000ULL};
    //  return t - (n < zero_or_powers_of_10[t]);
    //}

    //// __builtin_clz is broken in clang with Microsoft CodeGen:
    //// https://github.com/fmtlib/fmt/issues/519.
    //#if !FMT_MSC_VERSION
    //#  if FMT_HAS_BUILTIN(__builtin_clz) || FMT_GCC_VERSION || FMT_ICC_VERSION
    //#    define FMT_BUILTIN_CLZ(n) __builtin_clz(n)
    //#  endif
    //#  if FMT_HAS_BUILTIN(__builtin_clzll) || FMT_GCC_VERSION || FMT_ICC_VERSION
    //#    define FMT_BUILTIN_CLZLL(n) __builtin_clzll(n)
    //#  endif
    //#endif
    //
    //// __builtin_ctz is broken in Intel Compiler Classic on Windows:
    //// https://github.com/fmtlib/fmt/issues/2510.
    //#ifndef __ICL
    //#  if FMT_HAS_BUILTIN(__builtin_ctz) || FMT_GCC_VERSION || FMT_ICC_VERSION || \
//      defined(__NVCOMPILER)
    //#    define FMT_BUILTIN_CTZ(n) __builtin_ctz(n)
    //#  endif
    //#  if FMT_HAS_BUILTIN(__builtin_ctzll) || FMT_GCC_VERSION || \
//      FMT_ICC_VERSION || defined(__NVCOMPILER)
    //#    define FMT_BUILTIN_CTZLL(n) __builtin_ctzll(n)
    //#  endif
    //#endif
    //
    //#if FMT_MSC_VERSION
    //#  include <intrin.h>  // _BitScanReverse[64], _BitScanForward[64], _umul128
    //#endif
    //
    //// Some compilers masquerade as both MSVC and GCC-likes or otherwise support
    //// __builtin_clz and __builtin_clzll, so only define FMT_BUILTIN_CLZ using the
    //// MSVC intrinsics if the clz and clzll builtins are not available.
    //#if FMT_MSC_VERSION && !defined(FMT_BUILTIN_CLZLL) && \
//    !defined(FMT_BUILTIN_CTZLL)
    //FMT_BEGIN_NAMESPACE
    //namespace detail {
    //// Avoid Clang with Microsoft CodeGen's -Wunknown-pragmas warning.
    //#  if !defined(__clang__)
    //#    pragma intrinsic(_BitScanForward)
    //#    pragma intrinsic(_BitScanReverse)
    //#    if defined(_WIN64)
    //#      pragma intrinsic(_BitScanForward64)
    //#      pragma intrinsic(_BitScanReverse64)
    //#    endif
    //#  endif
    //
    //inline auto clz(uint32_t x) -> int {
    //  unsigned long r = 0;
    //  _BitScanReverse(&r, x);
    //  FMT_ASSERT(x != 0, "");
    //  // Static analysis complains about using uninitialized data
    //  // "r", but the only way that can happen is if "x" is 0,
    //  // which the callers guarantee to not happen.
    //  FMT_MSC_WARNING(suppress : 6102)
    //  return 31 ^ static_cast<int>(r);
    //}
    //#  define FMT_BUILTIN_CLZ(n) detail::clz(n)
    //
    //inline auto clzll(uint64_t x) -> int {
    //  unsigned long r = 0;
    //#  ifdef _WIN64
    //  _BitScanReverse64(&r, x);
    //#  else
    //  // Scan the high 32 bits.
    //  if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
    //    return 63 ^ static_cast<int>(r + 32);
    //  // Scan the low 32 bits.
    //  _BitScanReverse(&r, static_cast<uint32_t>(x));
    //#  endif
    //  FMT_ASSERT(x != 0, "");
    //  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
    //  return 63 ^ static_cast<int>(r);
    //}
    //#  define FMT_BUILTIN_CLZLL(n) detail::clzll(n)
    //
    //inline auto ctz(uint32_t x) -> int {
    //  unsigned long r = 0;
    //  _BitScanForward(&r, x);
    //  FMT_ASSERT(x != 0, "");
    //  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
    //  return static_cast<int>(r);
    //}
    //#  define FMT_BUILTIN_CTZ(n) detail::ctz(n)
    //
    //inline auto ctzll(uint64_t x) -> int {
    //  unsigned long r = 0;
    //  FMT_ASSERT(x != 0, "");
    //  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
    //#  ifdef _WIN64
    //  _BitScanForward64(&r, x);
    //#  else
    //  // Scan the low 32 bits.
    //  if (_BitScanForward(&r, static_cast<uint32_t>(x))) return static_cast<int>(r);
    //  // Scan the high 32 bits.
    //  _BitScanForward(&r, static_cast<uint32_t>(x >> 32));
    //  r += 32;
    //#  endif
    //  return static_cast<int>(r);
    //}
    //#  define FMT_BUILTIN_CTZLL(n) detail::ctzll(n)
    //}  // namespace detail
    //FMT_END_NAMESPACE
    //#endif

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

#define FST_DELARE_MATH_OP1(NAME)                                                                 \
    template <typename T, __fst::enable_if_t<__fst::is_floating_point_v<T>, int> = 0>             \
    FST_NODISCARD FST_ALWAYS_INLINE T NAME(T x) noexcept                                          \
    {                                                                                             \
        if constexpr (__fst::is_same_v<float, T>) { return ::FST_CONCAT(NAME, f)(x); }            \
        else if constexpr (__fst::is_same_v<double, T>) { return ::NAME(x); }                     \
        else if constexpr (__fst::is_same_v<long double, T>) { return ::FST_CONCAT(NAME, l)(x); } \
        else { static_assert(__fst::always_false<T>, "Unsupported type"); }                       \
    }

#define FST_DELARE_MATH_OP2(NAME)                                                                    \
    template <typename T, __fst::enable_if_t<__fst::is_floating_point_v<T>, int> = 0>                \
    FST_NODISCARD FST_ALWAYS_INLINE T NAME(T x, T y) noexcept                                        \
    {                                                                                                \
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


    
    // Returns true if value is negative, false otherwise.
    // Same as `value < 0` but doesn't produce warnings if T is an unsigned type.
    template <typename T, __fst::enable_if_t<__fst::is_signed_v<T>>>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_negative(T value) noexcept
    {
        return value < 0;
    }

    template <typename T, __fst::enable_if_t<!__fst::is_signed_v<T>>>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_negative(T) noexcept
    {
        return false;
    }

    namespace detail{
    FST_INLINE_VAR constexpr uint32_t uint32_powers_10[10] = {
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
        //   123456789
    };

    FST_INLINE_VAR constexpr uint64_t uint64_powers_10[20] = {
        1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL, 10000000ULL, 100000000ULL, 1000000000ULL, 10000000000ULL, 100000000000ULL, 1000000000000ULL,
        10000000000000ULL, 100000000000000ULL, 1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL, 1000000000000000000ULL, 10000000000000000000ULL
        //   1234567890123456789
    };
    }

    template <typename TUint>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr const TUint* powers_10()noexcept;
    
    template <>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr const uint32_t* powers_10() noexcept
    {
        return detail::uint32_powers_10;
    }

    template <>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr const uint64_t* powers_10() noexcept
    {
        return detail::uint64_powers_10;
    }

FST_END_NAMESPACE
