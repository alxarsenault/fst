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
#include "fst/iterator.h"
#include "fst/pointer.h"
#include "fst/utility.h"

#include <string.h>
#include <ctype.h>
#include <wchar.h>

#if __FST_MSVC__ && __FST_INTEL__
#include <intrin.h>
#endif

FST_BEGIN_NAMESPACE

    ///
    FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t strlen(const char* str) noexcept
    {
#if __FST_MSVC__ || __FST_CLANG__
        return __builtin_strlen(str);
#else
        return ::strlen(str);
#endif
    }

    FST_NODISCARD FST_ALWAYS_INLINE size_t strnlen(const char* str, size_t max_length) noexcept
    {
        return ::strnlen(str, max_length);
    }
    

    ///
    FST_NODISCARD FST_ALWAYS_INLINE int strncmp(const char* a, const char* b, size_t size) noexcept
    {

#if __FST_CLANG__
        return __builtin_strncmp(a, b, size);
#else
        return ::strncmp(a, b, size);
#endif
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE int strcmp(const char* a, const char* b) noexcept
    {
#if __FST_MSVC__ && __FST_INTEL__
#pragma intrinsic(strcmp)
#endif

        return ::strcmp(a, b);

#if __FST_MSVC__ && __FST_INTEL__
#pragma function(strcmp)
#endif
    }

    FST_NODISCARD FST_ALWAYS_INLINE char* strncpy(char* dst, const char* src, size_t count) noexcept
    {
        return ::strncpy(dst, src, count);
    }

    FST_NODISCARD FST_ALWAYS_INLINE char* strncat(char* dst, const char* src, size_t count) noexcept
    {
        return ::strncat(dst, src, count);
    }

    template <typename _CharT>
    class basic_string_view;

    ///
    ///
    ///
    enum class char_encoding {
        utf8 = sizeof(char),
        utf16 = sizeof(char16_t),
        utf32 = sizeof(char32_t)
    };

    ///
    ///
    ///
    // clang-format off
    template <class CharT>
    struct is_utf_char_type : __fst::disjunction<
          __fst::is_same_rcv<char, CharT>
        , __fst::is_same_rcv<char16_t, CharT>
        , __fst::is_same_rcv<char32_t, CharT>
        , __fst::is_same_rcv<wchar_t, CharT>
        FST_IF_CHAR8_T(, __fst::is_same_rcv<char8_t, CharT>)>
    {};
    // clang-format on

    ///
    ///
    template <size_t _CSize>
    struct is_utf_char_size : __fst::bool_t<_CSize == sizeof(char) || _CSize == sizeof(char16_t) || _CSize == sizeof(char32_t)>
    {};

    template <class _CharT, class _T>
    struct is_basic_string_view_convertible : __fst::is_convertible<const _T&, __fst::basic_string_view<_CharT>>
    {};

    template <class _CharT, class _T>
    FST_INLINE_VAR constexpr bool is_basic_string_view_convertible_v = __fst::is_basic_string_view_convertible<_CharT, _T>::value;

    template <class _CharT, class _T>
    struct is_basic_string_view_constructible : __fst::is_constructible<__fst::basic_string_view<_CharT>, _T>
    {};

    template <class _CharT, class _T>
    struct is_basic_string_view_convertible_not_char_buffer
        : __fst::bool_t<__fst::is_basic_string_view_convertible<_CharT, _T>::value && !__fst::is_convertible<const _T&, const _CharT*>::value>
    {};

    template <class _CharT, class _T>
    FST_INLINE_VAR constexpr bool is_basic_string_view_convertible_not_char_buffer_v = __fst::is_basic_string_view_convertible_not_char_buffer<_CharT, _T>::value;

    // clang-format off
    template <class _T> struct is_string_view_convertible : __fst::is_convertible<const _T&, __fst::basic_string_view<char>> {};
    template <class _T> struct is_wstring_view_convertible : __fst::is_convertible<const _T&, __fst::basic_string_view<wchar_t>> {};
    template <class _T> struct is_u16string_view_convertible : __fst::is_convertible<const _T&, __fst::basic_string_view<char16_t>> {};
    template <class _T> struct is_u32string_view_convertible : __fst::is_convertible<const _T&, __fst::basic_string_view<char32_t>> {};

    template < class _T>
    FST_INLINE_VAR constexpr bool is_string_view_convertible_v = __fst::is_string_view_convertible<  _T>::value;

    template <class _T> struct is_string_view_convertible_not_char_buffer : __fst::is_basic_string_view_convertible_not_char_buffer<char, _T> {};
    template <class _T> struct is_wstring_view_convertible_not_char_buffer : __fst::is_basic_string_view_convertible_not_char_buffer<wchar_t, _T> {};
    template <class _T> struct is_u16string_view_convertible_not_char_buffer : __fst::is_basic_string_view_convertible_not_char_buffer<char16_t, _T> {};
    template <class _T> struct is_u32string_view_convertible_not_char_buffer : __fst::is_basic_string_view_convertible_not_char_buffer<char32_t, _T> {};

#if FST_HAS_CHAR8_T
    template <class _T> struct is_u8string_view_convertible : __fst::is_convertible<const _T&, __fst::basic_string_view<char8_t>> {};
    template <class _T> struct is_u8string_view_convertible_not_char_buffer : __fst::is_basic_string_view_convertible_not_char_buffer<char8_t, _T> {};
#endif // FST_HAS_CHAR8_T
    // clang-format on

    template <class SType>
    struct is_utf_string_type
        : __fst::bool_t<__fst::is_constructible_v<__fst::basic_string_view<char>, SType> //
                        || __fst::is_constructible_v<__fst::basic_string_view<char16_t>, SType> //
                        || __fst::is_constructible_v<__fst::basic_string_view<char32_t>, SType> //
                        || __fst::is_constructible_v<__fst::basic_string_view<wchar_t>, SType> //
                            FST_IF_CHAR8_T(|| __fst::is_constructible_v<__fst::basic_string_view<char8_t>, SType>)>
    {};

    template <class SType>
    using enable_if_utf_string_type_t = __fst::enable_if_t<__fst::is_utf_string_type<SType>::value, __fst::nullptr_t>;

    template <class SType>
    struct is_utf_basic_string_type : __fst::bool_t<__fst::is_utf_string_type<SType>::value && __fst::has_push_back_v<SType>>
    {};

    ///
    ///
    ///
    template <class SType, typename = void>
    struct string_char_type;

    ///
    ///
    ///
    template <class SType>
    using string_char_type_t = typename string_char_type<SType>::type;

    template <class SType, __fst::enable_if_t<__fst::is_utf_string_type<SType>::value, __fst::nullptr_t> = nullptr>
    using string_view_type = __fst::basic_string_view<__fst::string_char_type_t<SType>>;

    ///
    ///
    ///
    template <typename CharT, __fst::enable_if_t<__fst::is_utf_char_type<CharT>::value, __fst::nullptr_t> = nullptr>
    struct utf_encoding_of
    {
        static constexpr char_encoding value = []()
        {
            if constexpr (sizeof(CharT) == sizeof(char)) { return char_encoding::utf8; }
            else if constexpr (sizeof(CharT) == sizeof(char16_t)) { return char_encoding::utf16; }
            else if constexpr (sizeof(CharT) == sizeof(char32_t)) { return char_encoding::utf32; }
        }();
    };

    template <char_encoding Encoding>
    struct utf_encoding_size
    {
        static constexpr size_t value = static_cast<size_t>(Encoding);
    };

    template <char_encoding Encoding>
    struct utf_encoding_to_max_char_count
    {
        static constexpr size_t value = []()
        {
            if constexpr (Encoding == char_encoding::utf8) { return 4; }
            else if constexpr (Encoding == char_encoding::utf16) { return 2; }
            else if constexpr (Encoding == char_encoding::utf32) { return 1; }
        }();
    };

    template <typename CharT, __fst::enable_if_t<sizeof(CharT) == sizeof(char), __fst::nullptr_t> = nullptr>
    size_t utf8_length(const CharT* str, size_t size) noexcept;

    template <typename CharT, __fst::enable_if_t<sizeof(CharT) == sizeof(char16_t), __fst::nullptr_t> = nullptr>
    size_t utf16_length(const CharT* str, size_t size) noexcept;

    template <typename CharT, __fst::enable_if_t<sizeof(CharT) == sizeof(char32_t), __fst::nullptr_t> = nullptr>
    size_t utf32_length(const CharT*, size_t size) noexcept;

    template <class SType, class _CharT>
    using enable_if_string_view_ctor_t = __fst::enable_if_t<__fst::is_basic_string_view_constructible<_CharT, __fst::remove_cvref_t<SType>>::value>;

    // clang-format off
    template <class SType, typename> struct string_char_type { using type = void; };
    template <class SType> struct string_char_type<SType, enable_if_string_view_ctor_t<SType, char>> { using type = char; };
    template <class SType> struct string_char_type<SType, enable_if_string_view_ctor_t<SType, char16_t>> { using type = char16_t; };
    template <class SType> struct string_char_type<SType, enable_if_string_view_ctor_t<SType, char32_t>> { using type = char32_t; };
    template <class SType> struct string_char_type<SType, enable_if_string_view_ctor_t<SType, wchar_t>> { using type = wchar_t; };

#if FST_HAS_CHAR8_T
    template <class SType> struct string_char_type<SType, enable_if_string_view_ctor_t<SType, char8_t>> { using type = char8_t; };
#endif // FST_HAS_CHAR8_T
    // clang-format on

    ///
    template <class SType, __fst::enable_if_utf_string_type_t<SType> = nullptr>
    inline constexpr typename __fst::string_view_type<SType>::size_type find_string(const SType& s, __fst::string_view_type<SType> str) noexcept;

    ///
    template <class SType, __fst::enable_if_utf_string_type_t<SType> = nullptr>
    size_t count_lines(const SType& str) noexcept;

    template <class _CharT>
    constexpr size_t str_find_not_ch(const _CharT* _Haystack, size_t _Hay_size, size_t _Start_at, _CharT _Ch) noexcept
    {
        // search [_Haystack, _Haystack + _Hay_size) for any value other than _Ch, at/after _Start_at
        if (_Start_at < _Hay_size)
        { // room for match, look for it
            const auto _End = _Haystack + _Hay_size;
            for (auto _Match_try = _Haystack + _Start_at; _Match_try < _End; ++_Match_try)
            {
                if (*_Match_try != _Ch)
                {
                    return static_cast<size_t>(_Match_try - _Haystack); // found a match
                }
            }
        }

        return static_cast<size_t>(-1); // no match
    }

    ///
    template <class _CharT>
    struct char_traits
    {
        using char_type = _CharT;
        using int_type = int;
        using pos_type = ::ptrdiff_t;
        using off_type = ::ptrdiff_t;

        static inline constexpr size_t length(const char_type* s) { return __fst::strlen(s); }

        static inline constexpr int compare(const char_type* s1, const char_type* s2, size_t count) { return __fst::strncmp(s1, s2, count); }

        static inline constexpr bool compare(const char_type* s1, size_t size1, const char_type* s2, size_t size2)
        {
            return (size1 == size2) && (compare(s1, s2, size1) == 0);
        }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, const char_type _Ch) noexcept { return ::strchr(_First, _Ch); }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, size_t _Count, const char_type _Ch) noexcept
        {

#if FST_HAS_BUILTIN(__builtin_char_memchr)
            return (const char_type*) __builtin_char_memchr(_First, _Ch, _Count);
#else
            return (const char_type*) ::memchr(_First, _Ch, _Count);
#endif
        }


    };

    template <>
    struct char_traits<wchar_t>
    {
        using char_type = wchar_t;
        using int_type = ::wint_t;
        using pos_type = ::ptrdiff_t;
        using off_type = ::ptrdiff_t;

        static inline constexpr size_t length(const char_type* s)
        {
            size_t count = 0;
            while (*s != char_type())
            {
                ++count;
                ++s;
            }

            return count;
        }

        static inline constexpr int compare(const char_type* s1, const char_type* s2, size_t count)
        {
            for (; 0 < count; --count, ++s1, ++s2)
            {
                if (*s1 != *s2) { return *s1 < *s2 ? -1 : +1; }
            }

            return 0;
        }

        static inline constexpr bool compare(const char_type* s1, size_t size1, const char_type* s2, size_t size2)
        {
            return (size1 == size2) && (compare(s1, s2, size1) == 0);
        }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, size_t _Count, const char_type _Ch) noexcept
        {

            for (size_t i = 0; i < _Count; i++)
            {
                if (_First[i] == _Ch) { return _First + i; }
            }

            return nullptr;
        }
    };

    template <>
    struct char_traits<char16_t>
    {
        using char_type = char16_t;
        using int_type = ::uint_least16_t;
        using pos_type = ::ptrdiff_t;
        using off_type = ::ptrdiff_t;

        static constexpr size_t length(const char_type* s)
        {
            size_t count = 0;
            while (*s != char_type())
            {
                ++count;
                ++s;
            }

            return count;
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, size_t count)
        {
            for (; 0 < count; --count, ++s1, ++s2)
            {
                if (*s1 != *s2) { return *s1 < *s2 ? -1 : +1; }
            }

            return 0;
        }

        static inline constexpr bool compare(const char_type* s1, size_t size1, const char_type* s2, size_t size2)
        {
            return (size1 == size2) && (compare(s1, s2, size1) == 0);
        }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, size_t _Count, const char_type _Ch) noexcept
        {

            for (size_t i = 0; i < _Count; i++)
            {
                if (_First[i] == _Ch) { return _First + i; }
            }

            return nullptr;
        }
    };

    template <>
    struct char_traits<char32_t>
    {
        using char_type = char32_t;
        using int_type = ::uint_least32_t;
        using pos_type = ::ptrdiff_t;
        using off_type = ::ptrdiff_t;

        static constexpr size_t length(const char_type* s)
        {
            size_t count = 0;
            while (*s != char_type())
            {
                ++count;
                ++s;
            }

            return count;
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, size_t count)
        {
            for (; 0 < count; --count, ++s1, ++s2)
            {
                if (*s1 != *s2) { return *s1 < *s2 ? -1 : +1; }
            }

            return 0;
        }

        static inline constexpr bool compare(const char_type* s1, size_t size1, const char_type* s2, size_t size2)
        {
            return (size1 == size2) && (compare(s1, s2, size1) == 0);
        }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, size_t _Count, const char_type _Ch) noexcept
        {

            for (size_t i = 0; i < _Count; i++)
            {
                if (_First[i] == _Ch) { return _First + i; }
            }

            return nullptr;
        }
    };

#if FST_HAS_CHAR8_T
    template <>
    struct char_traits<char8_t>
    {
        using char_type = char8_t;
        using int_type = unsigned int;
        using pos_type = ::ptrdiff_t;
        using off_type = ::ptrdiff_t;

        static constexpr size_t length(const char_type* s)
        {
            size_t count = 0;
            while (*s != char_type())
            {
                ++count;
                ++s;
            }

            return count;
        }

        static constexpr int compare(const char_type* s1, const char_type* s2, size_t count)
        {
            for (; 0 < count; --count, ++s1, ++s2)
            {
                if (*s1 != *s2) { return *s1 < *s2 ? -1 : +1; }
            }

            return 0;
        }

        static inline constexpr bool compare(const char_type* s1, size_t size1, const char_type* s2, size_t size2)
        {
            return (size1 == size2) && (compare(s1, s2, size1) == 0);
        }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, size_t _Count, const char_type _Ch) noexcept
        {

            for (size_t i = 0; i < _Count; i++)
            {
                if (_First[i] == _Ch) { return _First + i; }
            }

            return nullptr;
        }
    };
#endif // FST_HAS_CHAR8_T

    namespace detail
    {

        FST_INLINE_VAR constexpr char digits_0_to_99[201] = { "00010203040506070809"
                                                              "10111213141516171819"
                                                              "20212223242526272829"
                                                              "30313233343536373839"
                                                              "40414243444546474849"
                                                              "50515253545556575859"
                                                              "60616263646566676869"
                                                              "70717273747576777879"
                                                              "80818283848586878889"
                                                              "90919293949596979899" };

        FST_INLINE_VAR constexpr uint16_t digits_to_99[100] = { //
            //  00,     01,     02,     03,     04,     05,     06,     07,     08,     09
            0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930, //
            0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931, //
            0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932, //
            0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933, //
            0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934, //
            0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935, //
            0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, //
            0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937, //
            0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938, //
            //  90,     91,     92,     93,     94,     95,     96,     97,     98,     99
            0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939
        };
    } // namespace detail

    //FST_ALWAYS_INLINE void get_digit_pair(char* buffer, size_t index) noexcept
    //{
    //*(uint16_t*) (buffer) = detail::digits_to_99[index];
    //}

    FST_ALWAYS_INLINE constexpr void get_digit_pair(char* buffer, size_t index) noexcept
    {
        const char* ptr = detail::digits_0_to_99 + index * 2;
        *buffer++ = *ptr++;
        *buffer = *ptr;
    }

    // Compilers should be able to optimize this into the ror instruction.
    //FMT_CONSTEXPR inline uint32_t rotr(uint32_t n, uint32_t r) noexcept {
    //  r &= 31;
    //  return (n >> r) | (n << (32 - r));
    //}
    //FMT_CONSTEXPR inline uint64_t rotr(uint64_t n, uint32_t r) noexcept {
    //  r &= 63;
    //  return (n >> r) | (n << (64 - r));
    //}
    //    // Various fast log computations.
    //inline int floor_log10_pow2_minus_log10_4_over_3(int e) noexcept {
    //  FMT_ASSERT(e <= 2936 && e >= -2985, "too large exponent");
    //  return (e * 631305 - 261663) >> 21;
    //}
    // Parses the range [begin, end) as an unsigned integer. This function assumes
    // that the range is non-empty and the first character is a digit.
    //template <typename Char>
    //FMT_CONSTEXPR auto parse_nonnegative_int(const Char*& begin, const Char* end,
    //                                         int error_value) noexcept -> int {
    //  FMT_ASSERT(begin != end && '0' <= *begin && *begin <= '9', "");
    //  unsigned value = 0, prev = 0;
    //  auto p = begin;
    //  do {
    //    prev = value;
    //    value = value * 10 + unsigned(*p - '0');
    //    ++p;
    //  } while (p != end && '0' <= *p && *p <= '9');
    //  auto num_digits = p - begin;
    //  begin = p;
    //  if (num_digits <= std::numeric_limits<int>::digits10)
    //    return static_cast<int>(value);
    //  // Check for overflow.
    //  const unsigned max = to_unsigned((std::numeric_limits<int>::max)());
    //  return num_digits == std::numeric_limits<int>::digits10 + 1 &&
    //                 prev * 10ull + unsigned(p[-1] - '0') <= max
    //             ? static_cast<int>(value)
    //             : error_value;
    //}
    // Returns the largest possible value for type T. Same as
    // std::numeric_limits<T>::max() but shorter and not affected by the max macro.
    //template <typename T> constexpr auto max_value() -> T {
    //  return (std::numeric_limits<T>::max)();
    //}
    //template <typename T> constexpr auto num_bits() -> int {
    //  return std::numeric_limits<T>::digits;
    //}

    //FMT_INLINE_VARIABLE constexpr struct {
    //  uint32_t divisor;
    //  int shift_amount;
    //} div_small_pow10_infos[] = {{10, 16}, {100, 16}};
    //
    //// Replaces n by floor(n / pow(10, N)) returning true if and only if n is
    //// divisible by pow(10, N).
    //// Precondition: n <= pow(10, N + 1).
    //template <int N>
    //bool check_divisibility_and_divide_by_pow10(uint32_t& n) noexcept {
    //  // The numbers below are chosen such that:
    //  //   1. floor(n/d) = floor(nm / 2^k) where d=10 or d=100,
    //  //   2. nm mod 2^k < m if and only if n is divisible by d,
    //  // where m is magic_number, k is shift_amount
    //  // and d is divisor.
    //  //
    //  // Item 1 is a common technique of replacing division by a constant with
    //  // multiplication, see e.g. "Division by Invariant Integers Using
    //  // Multiplication" by Granlund and Montgomery (1994). magic_number (m) is set
    //  // to ceil(2^k/d) for large enough k.
    //  // The idea for item 2 originates from Schubfach.
    //  constexpr auto info = div_small_pow10_infos[N - 1];
    //  FMT_ASSERT(n <= info.divisor * 10, "n is too large");
    //  constexpr uint32_t magic_number =
    //      (1u << info.shift_amount) / info.divisor + 1;
    //  n *= magic_number;
    //  const uint32_t comparison_mask = (1u << info.shift_amount) - 1;
    //  bool result = (n & comparison_mask) < magic_number;
    //  n >>= info.shift_amount;
    //  return result;
    //}
    //
    //// Computes floor(n / pow(10, N)) for small n and N.
    //// Precondition: n <= pow(10, N + 1).
    //template <int N> uint32_t small_division_by_pow10(uint32_t n) noexcept {
    //  constexpr auto info = div_small_pow10_infos[N - 1];
    //  FMT_ASSERT(n <= info.divisor * 10, "n is too large");
    //  constexpr uint32_t magic_number =
    //      (1u << info.shift_amount) / info.divisor + 1;
    //  return (n * magic_number) >> info.shift_amount;
    //}

    //
    //// Remove trailing zeros from n and return the number of zeros removed (float)
    //FMT_INLINE int remove_trailing_zeros(uint32_t& n) noexcept {
    //  FMT_ASSERT(n != 0, "");
    //  // Modular inverse of 5 (mod 2^32): (mod_inv_5 * 5) mod 2^32 = 1.
    //  // See https://github.com/fmtlib/fmt/issues/3163 for more details.
    //  const uint32_t mod_inv_5 = 0xcccccccd;
    //  // Casts are needed to workaround a bug in MSVC 19.22 and older.
    //  const uint32_t mod_inv_25 =
    //      static_cast<uint32_t>(uint64_t(mod_inv_5) * mod_inv_5);
    //
    //  int s = 0;
    //  while (true) {
    //    auto q = rotr(n * mod_inv_25, 2);
    //    if (q > max_value<uint32_t>() / 100) break;
    //    n = q;
    //    s += 2;
    //  }
    //  auto q = rotr(n * mod_inv_5, 1);
    //  if (q <= max_value<uint32_t>() / 10) {
    //    n = q;
    //    s |= 1;
    //  }
    //  return s;
    //}
    //
    //// Removes trailing zeros and returns the number of zeros removed (double)
    //FMT_INLINE int remove_trailing_zeros(uint64_t& n) noexcept {
    //  FMT_ASSERT(n != 0, "");
    //
    //  // This magic number is ceil(2^90 / 10^8).
    //  constexpr uint64_t magic_number = 12379400392853802749ull;
    //  auto nm = umul128(n, magic_number);
    //
    //  // Is n is divisible by 10^8?
    //  if ((nm.high() & ((1ull << (90 - 64)) - 1)) == 0 && nm.low() < magic_number) {
    //    // If yes, work with the quotient.
    //    auto n32 = static_cast<uint32_t>(nm.high() >> (90 - 64));
    //
    //    const uint32_t mod_inv_5 = 0xcccccccd;
    //    const uint32_t mod_inv_25 = mod_inv_5 * mod_inv_5;
    //
    //    int s = 8;
    //    while (true) {
    //      auto q = rotr(n32 * mod_inv_25, 2);
    //      if (q > max_value<uint32_t>() / 100) break;
    //      n32 = q;
    //      s += 2;
    //    }
    //    auto q = rotr(n32 * mod_inv_5, 1);
    //    if (q <= max_value<uint32_t>() / 10) {
    //      n32 = q;
    //      s |= 1;
    //    }
    //
    //    n = n32;
    //    return s;
    //  }
    //
    //  // If n is not divisible by 10^8, work with n itself.
    //  const uint64_t mod_inv_5 = 0xcccccccccccccccd;
    //  const uint64_t mod_inv_25 = mod_inv_5 * mod_inv_5;
    //
    //  int s = 0;
    //  while (true) {
    //    auto q = rotr(n * mod_inv_25, 2);
    //    if (q > max_value<uint64_t>() / 100) break;
    //    n = q;
    //    s += 2;
    //  }
    //  auto q = rotr(n * mod_inv_5, 1);
    //  if (q <= max_value<uint64_t>() / 10) {
    //    n = q;
    //    s |= 1;
    //  }
    //
    //  return s;
    //}
    //
    //// Computes floor(log10(pow(2, e))) for e in [-2620, 2620] using the method from
    //// https://fmt.dev/papers/Dragonbox.pdf#page=28, section 6.1.
    //inline int floor_log10_pow2(int e) noexcept {
    //  FMT_ASSERT(e <= 2620 && e >= -2620, "too large exponent");
    //  static_assert((-1 >> 1) == -1, "right shift is not arithmetic");
    //  return (e * 315653) >> 20;
    //}
    //
    //inline int floor_log2_pow10(int e) noexcept {
    //  FMT_ASSERT(e <= 1233 && e >= -1233, "too large exponent");
    //  return (e * 1741647) >> 19;
    //}
    //
    //// Computes upper 64 bits of multiplication of two 64-bit unsigned integers.
    //inline uint64_t umul128_upper64(uint64_t x, uint64_t y) noexcept {
    //#if FMT_USE_INT128
    //  auto p = static_cast<uint128_opt>(x) * static_cast<uint128_opt>(y);
    //  return static_cast<uint64_t>(p >> 64);
    //#elif defined(_MSC_VER) && defined(_M_X64)
    //  return __umulh(x, y);
    //#else
    //  return umul128(x, y).high();
    //#endif
    //}
    //template <typename T> constexpr auto num_bits() -> int {
    //  return std::numeric_limits<T>::digits;
    //}
    //// std::numeric_limits<T>::digits may return 0 for 128-bit ints.
    //template <> constexpr auto num_bits<int128_opt>() -> int { return 128; }
    //template <> constexpr auto num_bits<uint128_t>() -> int { return 128; }
    //
    //// A heterogeneous bit_
    //
    //// Computes 128-bit result of multiplication of two 64-bit unsigned integers.
    //inline uint128_fallback umul128(uint64_t x, uint64_t y) noexcept {
    //#if FMT_USE_INT128
    //  auto p = static_cast<uint128_opt>(x) * static_cast<uint128_opt>(y);
    //  return {static_cast<uint64_t>(p >> 64), static_cast<uint64_t>(p)};
    //#elif defined(_MSC_VER) && defined(_M_X64)
    //  auto result = uint128_fallback();
    //  result.lo_ = _umul128(x, y, &result.hi_);
    //  return result;
    //#else
    //  const uint64_t mask = static_cast<uint64_t>(max_value<uint32_t>());
    //
    //  uint64_t a = x >> 32;
    //  uint64_t b = x & mask;
    //  uint64_t c = y >> 32;
    //  uint64_t d = y & mask;
    //
    //  uint64_t ac = a * c;
    //  uint64_t bc = b * c;
    //  uint64_t ad = a * d;
    //  uint64_t bd = b * d;
    //
    //  uint64_t intermediate = (bd >> 32) + (ad & mask) + (bc & mask);
    //
    //  return {ac + (intermediate >> 32) + (ad >> 32) + (bc >> 32),
    //          (intermediate << 32) + (bd & mask)};
    //#endif
    //}
    namespace utf
    {
        inline constexpr const uint16_t k_lead_surrogate_min = 0xD800u;
        inline constexpr const uint16_t k_lead_offset = k_lead_surrogate_min - (0x10000 >> 10);
        inline constexpr const uint16_t k_trail_surrogate_min = 0xDC00u;

        /// Maximum valid value for a Unicode code point.
        inline constexpr const uint32_t k_code_point_max = 0x0010FFFFu;
        inline constexpr const uint32_t k_surrogate_offset = 0x10000u - (k_lead_surrogate_min << 10) - k_trail_surrogate_min;

        inline constexpr const uint8_t bom[] = { 0xef, 0xbb, 0xbf };

        template <class _U8CharT>
        inline uint8_t cast_8(_U8CharT c) noexcept
        {
            return static_cast<uint8_t>(c);
        }

        template <class _U16CharT>
        inline uint16_t cast_16(_U16CharT c) noexcept
        {
            return static_cast<uint16_t>(c);
        }

        template <class _U8CharT>
        inline bool is_trail(_U8CharT c) noexcept
        {
            return (cast_8(c) >> 6) == 0x2;
        }

        inline constexpr bool is_surrogate(char16_t uc) noexcept
        {
            return ((uint32_t) uc - (uint32_t) k_lead_surrogate_min) < 2048u;
        }
        // inline constexpr bool is_surrogate(char16_t uc) noexcept { return (uc - 0xD800u) < 2048u; }

        // Lead.
        inline constexpr bool is_high_surrogate(char16_t uc) noexcept
        {
            return (uc & 0xFFFFFC00) == k_lead_surrogate_min;
        }
        // inline constexpr bool is_high_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == 0xD800; }

        // Trail.
        inline constexpr bool is_low_surrogate(char16_t uc) noexcept
        {
            return (uc & 0xFFFFFC00) == k_trail_surrogate_min;
        }
        // inline constexpr bool is_low_surrogate(char16_t uc) noexcept { return (uc & 0xFFFFFC00) == 0xDC00; }

        template <class _U32CharT>
        inline constexpr bool is_valid_code_point(_U32CharT cp) noexcept
        {
            return cp <= k_code_point_max && !is_surrogate(cp);
        }

        inline constexpr size_t sequence_length(uint8_t lead) noexcept
        {
            return (size_t) (lead < 0x80 ? 1 : (lead >> 5) == 0x6 ? 2 : (lead >> 4) == 0xE ? 3 : (lead >> 3) == 0x1E ? 4 : 0);
        }

        inline constexpr size_t u16_sequence_length(char16_t c) noexcept
        {
            return (size_t) (is_high_surrogate(c) ? 2 : 1);
        }

        template <typename u8_iterator>
        inline constexpr bool starts_with_bom(u8_iterator it, u8_iterator end) noexcept
        {
            return (((it != end) && (cast_8(*it++)) == bom[0]) && ((it != end) && (cast_8(*it++)) == bom[1]) && ((it != end) && (cast_8(*it)) == bom[2]));
        }

        template <typename u8_iterator>
        inline u8_iterator append_u32_to_u8(uint32_t cp, u8_iterator it) noexcept
        {
            using ctype = __fst::output_iterator_value_type_t<u8_iterator>;

            // 1 byte.
            if (cp < 0x80) { *it++ = static_cast<ctype>(cp); }
            // 2 bytes.
            else if (cp < 0x800)
            {
                *it++ = static_cast<ctype>((cp >> 6) | 0xC0);
                *it++ = static_cast<ctype>((cp & 0x3F) | 0x80);
            }
            // 3 bytes.
            else if (cp < 0x10000)
            {
                *it++ = static_cast<ctype>((cp >> 12) | 0xE0);
                *it++ = static_cast<ctype>(((cp >> 6) & 0x3F) | 0x80);
                *it++ = static_cast<ctype>((cp & 0x3F) | 0x80);
            }
            // 4 bytes.
            else
            {
                *it++ = static_cast<ctype>((cp >> 18) | 0xF0);
                *it++ = static_cast<ctype>(((cp >> 12) & 0x3F) | 0x80);
                *it++ = static_cast<ctype>(((cp >> 6) & 0x3F) | 0x80);
                *it++ = static_cast<ctype>((cp & 0x3F) | 0x80);
            }

            return it;
        }

        inline size_t code_point_size_u8(uint32_t cp) noexcept
        {
            // 1 byte.
            if (cp < 0x80) { return 1; }
            // 2 bytes.
            else if (cp < 0x800) { return 2; }
            // 3 bytes.
            else if (cp < 0x10000) { return 3; }
            // 4 bytes.
            else { return 4; }
        }

        template <typename u8_iterator>
        inline uint32_t next_u8_to_u32(u8_iterator& it) noexcept
        {
            uint32_t cp = __fst::utf::cast_8(*it);

            using difference_type = typename __fst::iterator_traits<u8_iterator>::difference_type;
            difference_type length = static_cast<difference_type>(__fst::utf::sequence_length(static_cast<uint8_t>(*it)));

            switch (length)
            {
            case 1: break;
            case 2:
                it++;
                cp = ((cp << 6) & 0x7FF) + ((*it) & 0x3F);
                break;
            case 3:
                ++it;
                cp = ((cp << 12) & 0xFFFF) + ((__fst::utf::cast_8(*it) << 6) & 0xFFF);
                ++it;
                cp += (*it) & 0x3F;
                break;
            case 4:
                ++it;
                cp = ((cp << 18) & 0x1FFFFF) + ((__fst::utf::cast_8(*it) << 12) & 0x3FFFF);
                ++it;
                cp += (__fst::utf::cast_8(*it) << 6) & 0xFFF;
                ++it;
                cp += (*it) & 0x3F;
                break;
            }
            ++it;
            return cp;
        }

        template <typename octet_iterator>
        uint32_t prior_u8_to_u32(octet_iterator& it) noexcept
        {
            while (__fst::utf::is_trail(*(--it)))
                ;

            octet_iterator temp = it;
            return next_u8_to_u32(temp);
        }

        template <typename u16_iterator, typename u8_iterator>
        u16_iterator u8_to_u16(u8_iterator start, u8_iterator end, u16_iterator outputIt) noexcept
        {
            while (start < end)
            {
                uint32_t cp = next_u8_to_u32(start);

                if (cp > 0xFFFF)
                { // make a surrogate pair
                    *outputIt++ = __fst::utf::cast_16((cp >> 10) + __fst::utf::k_lead_offset);
                    *outputIt++ = __fst::utf::cast_16((cp & 0x3FF) + __fst::utf::k_trail_surrogate_min);
                }
                else { *outputIt++ = __fst::utf::cast_16(cp); }
            }

            return outputIt;
        }

        template <typename u8_iterator>
        size_t u8_to_u16_length(u8_iterator start, u8_iterator end) noexcept
        {
            size_t count = 0;
            while (start < end)
            {
                uint32_t cp = next_u8_to_u32(start);
                count += (cp > 0xFFFF) ? 2 : 1;
            }

            return count;
        }

        template <typename u32_iterator, typename u8_iterator>
        u32_iterator u8_to_u32(u8_iterator start, u8_iterator end, u32_iterator outputIt) noexcept
        {
            using ctype = __fst::output_iterator_value_type_t<u32_iterator>;

            while (start < end)
            {
                *outputIt++ = static_cast<ctype>(next_u8_to_u32(start));
            }

            return outputIt;
        }

        template <typename u8_iterator>
        size_t u8_to_u32_length(u8_iterator start, u8_iterator end) noexcept
        {

            size_t count = 0;
            while (start < end)
            {
                switch (__fst::utf::sequence_length(static_cast<uint8_t>(*start++)))
                {
                case 1: break;
                case 2: ++start; break;
                case 3:
                    ++start;
                    ++start;
                    break;
                case 4:
                    ++start;
                    ++start;
                    ++start;
                    break;
                }
                count++;
            }

            return count;
        }
        template <typename u16_iterator, typename u8_iterator>
        u8_iterator u16_to_u8(u16_iterator start, u16_iterator end, u8_iterator outputIt) noexcept
        {
            while (start != end)
            {
                uint32_t cp = __fst::utf::cast_16(*start++);
                //

                // Take care of surrogate pairs first.
                if (__fst::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(__fst::utf::cast_16(*start++)) + __fst::utf::k_surrogate_offset;
                }

                outputIt = append_u32_to_u8(cp, outputIt);
            }

            return outputIt;
        }

        template <typename u16_iterator, typename u32_iterator>
        u32_iterator u16_to_u32(u16_iterator start, u16_iterator end, u32_iterator outputIt) noexcept
        {
            using ctype = __fst::output_iterator_value_type_t<u32_iterator>;

            while (start != end)
            {
                uint32_t cp = __fst::utf::cast_16(*start++);

                // Take care of surrogate pairs first.
                if (__fst::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(__fst::utf::cast_16(*start++)) + __fst::utf::k_surrogate_offset;
                }

                *outputIt++ = static_cast<ctype>(cp);
            }

            return outputIt;
        }

        template <typename u16_iterator>
        size_t u16_to_u8_length(u16_iterator start, u16_iterator end) noexcept
        {
            size_t count = 0;
            while (start != end)
            {
                uint32_t cp = __fst::utf::cast_16(*start++);

                // Take care of surrogate pairs first.
                if (__fst::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(__fst::utf::cast_16(*start++)) + __fst::utf::k_surrogate_offset;
                }

                count += code_point_size_u8(cp);
            }

            return count;
        }

        template <typename u16_iterator>
        size_t u16_to_u32_length(u16_iterator start, u16_iterator end) noexcept
        {
            size_t count = 0;

            while (start != end)
            {
                uint32_t cp = __fst::utf::cast_16(*start++);

                // Take care of surrogate pairs first.
                if (__fst::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(__fst::utf::cast_16(*start++)) + __fst::utf::k_surrogate_offset;
                }

                count++;
            }

            return count;
        }

        template <typename u8_iterator, typename u32_iterator>
        u8_iterator u32_to_u8(u32_iterator start, u32_iterator end, u8_iterator outputIt) noexcept
        {

            while (start != end)
            {
                outputIt = append_u32_to_u8(static_cast<uint32_t>(*start++), outputIt);
            }

            return outputIt;
        }

        template <typename u16_iterator, typename u32_iterator>
        u16_iterator u32_to_u16(u32_iterator start, u32_iterator end, u16_iterator outputIt) noexcept
        {
            while (start != end)
            {
                uint32_t cp = static_cast<uint32_t>(*start++);

                using value_type = __fst::output_iterator_value_type_t<u16_iterator>;

                if (cp <= 0x0000FFFF)
                {
                    // UTF-16 surrogate values are illegal in UTF-32
                    // 0xFFFF or 0xFFFE are both reserved values.
                    if (cp >= 0xD800 && cp <= 0xDFFF) { *outputIt++ = 0x0000FFFD; }
                    else
                    {
                        // BMP character.
                        *outputIt++ = static_cast<value_type>(cp);
                    }
                }
                else if (cp > 0x0010FFFF)
                {
                    // U+10FFFF is the largest code point of Unicode character set.
                    *outputIt++ = static_cast<value_type>(0x0000FFFD);
                }
                else
                {
                    // c32 is a character in range 0xFFFF - 0x10FFFF.
                    cp -= 0x0010000UL;
                    *outputIt++ = static_cast<value_type>(((cp >> 10) + 0xD800));
                    *outputIt++ = static_cast<value_type>(((cp & 0x3FFUL) + 0xDC00));
                }
            }

            return outputIt;
        }

        template <typename u32_iterator>
        size_t u32_to_u8_length(u32_iterator start, u32_iterator end) noexcept
        {
            size_t count = 0;

            while (start != end)
            {
                count += code_point_size_u8(static_cast<uint32_t>(*start++));
            }

            return count;
        }

        template <typename u32_iterator>
        size_t u32_to_u16_length(u32_iterator start, u32_iterator end) noexcept
        {
            size_t count = 0;

            while (start != end)
            {
                uint32_t cp = static_cast<uint32_t>(*start++);
                count += (cp <= 0x0000FFFF || cp > 0x0010FFFF) ? 1 : 2;
            }

            return count;
        }
    } // namespace utf.

    template <typename CharT, __fst::enable_if_t<sizeof(CharT) == sizeof(char), __fst::nullptr_t>>
    size_t utf8_length(const CharT* str, size_t size) noexcept
    {
        size_t dist = 0;

        for (size_t i = 0; i < size; i += __fst::utf::sequence_length(static_cast<uint8_t>(str[i])))
        {
            dist++;
        }

        return dist;
    }

    template <typename CharT, __fst::enable_if_t<sizeof(CharT) == sizeof(char16_t), __fst::nullptr_t>>
    size_t utf16_length(const CharT* str, size_t size) noexcept
    {
        size_t dist = 0;

        for (size_t i = 0; i < size; i++)
        {
            if (__fst::utf::is_high_surrogate(__fst::utf::cast_16(str[i]))) { i++; }

            dist++;
        }

        return dist;
    }

    template <typename CharT, __fst::enable_if_t<sizeof(CharT) == sizeof(char32_t), __fst::nullptr_t>>
    size_t utf32_length(const CharT*, size_t size) noexcept
    {
        return size;
    }

    template <typename _CharT>
    class basic_string_view
    {
      public:
        using value_type = _CharT;
        using traits_type = __fst::char_traits<_CharT>;
        using size_type = size_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = const_pointer;
        using const_iterator = const_pointer;

        static constexpr size_type npos = size_type(-1);

        constexpr basic_string_view() noexcept = default;
        constexpr basic_string_view(const basic_string_view&) noexcept = default;
        constexpr basic_string_view(basic_string_view&&) noexcept = default;

        inline constexpr basic_string_view(const_pointer str) noexcept
            : _data(str)
            , _size(traits_type::length(str))
        {}

        inline constexpr basic_string_view(const_pointer str, size_type count) noexcept
            : _data(str)
            , _size(count)
        {}

        template <size_t _Size>
        inline constexpr basic_string_view(const value_type (&__arr)[_Size]) noexcept
            : _data{ __arr }
            , _size{ (size_type) _Size }
        {}

        template <size_t _Size>
        inline constexpr basic_string_view(const __fst::array<value_type, _Size>& arr) noexcept
            : _data{ arr.data() }
            , _size{ (size_type) _Size }
        {}

        template <class _Container, __fst::enable_if_t<__fst::is_container_v<_Container> && __fst::is_different_v<__fst::remove_cvref_t<_Container>, basic_string_view>
                                                           && __fst::is_same_v<__fst::container_value_type_t<_Container>, value_type>,
                                        int>
                                    = 0>
        FST_ALWAYS_INLINE constexpr basic_string_view(const _Container& c) noexcept
            : _data(c.data())
            , _size(c.size())
        {}

        constexpr basic_string_view& operator=(const basic_string_view&) = default;
        constexpr basic_string_view& operator=(basic_string_view&&) = default;

        FST_NODISCARD constexpr size_type size() const noexcept { return _size; }
        FST_NODISCARD constexpr size_type length() const noexcept { return _size; }
        FST_NODISCARD constexpr size_type max_size() const noexcept { return npos - 1; }

        FST_NODISCARD constexpr bool empty() const noexcept { return !_data || size() == 0; }

        FST_NODISCARD constexpr const_pointer data() const noexcept { return _data; }

        FST_NODISCARD constexpr const_reference operator[](size_type pos) const noexcept { return _data[pos]; }

        FST_NODISCARD constexpr const_reference front() const noexcept { return _data[0]; }
        FST_NODISCARD constexpr const_reference back() const noexcept { return _data[size() - 1]; }

        FST_NODISCARD constexpr const_iterator begin() const noexcept { return data(); }
        FST_NODISCARD constexpr const_iterator cbegin() const noexcept { return data(); }

        FST_NODISCARD constexpr const_iterator end() const noexcept { return data() + size(); }
        FST_NODISCARD constexpr const_iterator cend() const noexcept { return end(); }

        inline constexpr void swap(basic_string_view& v) noexcept
        {
            __fst::mem_swap(_size, v._size);
            __fst::mem_swap(_data, v._data);
        }

        FST_NODISCARD inline constexpr basic_string_view substr(size_t pos = 0, size_t len = npos) const noexcept
        {
            const size_type max_length = pos > size() ? 0 : size() - pos;
            fst_assert(pos <= size(), "Index out of range in basic_string_view::substr");
            return basic_string_view(_data + pos, __fst::minimum(len, max_length));
        }

        FST_NODISCARD inline constexpr basic_string_view last(size_type __count) const noexcept FST_UNIT_TESTED
        {
            __count = __fst::minimum(size(), __count);
            return { data() + size() - __count, __count };
        }

        FST_NODISCARD inline constexpr int compare(basic_string_view v) const noexcept
        {
            if (size() != v.size()) { return size() < v.size() ? -1 : size() > v.size() ? 1 : 0; }

            return traits_type::compare(data(), v.data(), __fst::minimum(size(), v.size()));
        }

        constexpr void remove_prefix(const size_type _Count) noexcept
        {
            _data += _Count;
            _size -= _Count;
        }

        inline constexpr size_type find(value_type c) const noexcept
        {
            const value_type* ptr = traits_type::find(data(), size(), c);
            return ptr == nullptr ? npos : (size_type) __fst::pdistance(begin(), ptr);
        }
        inline constexpr size_type find(value_type c, size_t start) const noexcept
        {
            const value_type* ptr = traits_type::find(data() + start, size() - start, c);
            return ptr == nullptr ? npos : (size_type) __fst::pdistance(begin(), ptr);
        }

        inline constexpr size_type find(basic_string_view str) const noexcept
        {
            size_type n = size();
            size_type m = str.size();

            for (size_type i = 0; i <= n - m; ++i)
            {
                size_type j = 0;
                while (j < m && (*this)[i + j] == str[j])
                {
                    ++j;
                }
                if (j == m)
                { // match found
                    return i;
                }
            }
            return npos;
        }
        inline constexpr size_type find(basic_string_view str, size_t start) const noexcept
        {
            size_type n = size();
            size_type m = str.size();

            for (size_type i = start; i <= n - m; ++i)
            {
                size_type j = 0;
                while (j < m && (*this)[i + j] == str[j])
                {
                    ++j;
                }
                if (j == m)
                { // match found
                    return i;
                }
            }
            return npos;
        }

        inline constexpr size_type find_first_of(value_type c) const noexcept { return find(c); }
        inline constexpr size_type find_first_of(basic_string_view str) const noexcept { return find(str); }

        inline constexpr size_type find_last_of(value_type c) const noexcept
        {
            if (empty()) { return npos; }

            for (size_type i = 0, k = _size - 1; i < _size; i++, k--)
            {
                if (_data[k] == c) { return k; }
            }

            return npos;
        }

      private:
        const_pointer _data = nullptr;
        size_type _size = 0;
    };

    template <class SType, class _CharT, __fst::enable_if_t<__fst::is_utf_string_type<SType>::value, __fst::nullptr_t> = nullptr>
    inline __fst::array<__fst::basic_string_view<string_char_type_t<SType>>, 2> strsplit(const SType& str, _CharT sp) noexcept
    {
        using char_type = string_char_type_t<SType>;
        using view_type = __fst::basic_string_view<char_type>;
        using ret_type = __fst::array<view_type, 2>;

        view_type strv(str);
        const char_type* ptr = __fst::char_traits<char_type>::find(strv.begin(), strv.size(), sp);
        if (ptr)
        {
            size_t delta = __fst::pdistance(strv.begin(), ptr);
            return ret_type{ view_type(strv.substr(0, delta)), strv.substr(delta + 1) };
        }

        return ret_type{ strv, view_type() };
    }
    ///
    ///
    ///

    template <typename _CharT>
    class basic_string_range
    {
      public:
        using value_type = _CharT;
        using traits_type = __fst::char_traits<_CharT>;
        using size_type = size_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;

        static constexpr size_type npos = size_type(-1);

        constexpr basic_string_range() noexcept = default;
        constexpr basic_string_range(const basic_string_range&) noexcept = default;
        constexpr basic_string_range(basic_string_range&&) noexcept = default;

        inline constexpr basic_string_range(pointer str) noexcept
            : _data(str)
            , _size(traits_type::length(str))
        {}

        inline constexpr basic_string_range(pointer str, size_type count) noexcept
            : _data(str)
            , _size(count)
        {}

        template <size_t _Size>
        inline constexpr basic_string_range(value_type (&__arr)[_Size]) noexcept
            : _data{ __arr }
            , _size{ (size_type) _Size }
        {}

        template <size_t _Size>
        inline constexpr basic_string_range(__fst::array<value_type, _Size>& arr) noexcept
            : _data{ arr.data() }
            , _size{ (size_type) _Size }
        {}

        template <class _Container, __fst::enable_if_t<__fst::is_container_v<_Container> && __fst::is_different_v<__fst::remove_cvref_t<_Container>, basic_string_range>
                                                           && __fst::is_same_v<__fst::container_value_type_t<_Container>, value_type>,
                                        int>
                                    = 0>
        FST_ALWAYS_INLINE constexpr basic_string_range(_Container& c) noexcept
            : _data(c.data())
            , _size(c.size())
        {}

        constexpr basic_string_range& operator=(const basic_string_range&) = default;
        constexpr basic_string_range& operator=(basic_string_range&&) = default;

        FST_NODISCARD constexpr size_type size() const noexcept { return _size; }
        FST_NODISCARD constexpr size_type length() const noexcept { return _size; }
        FST_NODISCARD constexpr size_type max_size() const noexcept { return npos - 1; }

        FST_NODISCARD constexpr bool empty() const noexcept { return !_data || size() == 0; }

        FST_NODISCARD constexpr pointer data() noexcept { return _data; }
        FST_NODISCARD constexpr const_pointer data() const noexcept { return _data; }

        inline __fst::basic_string_view<_CharT> view() const noexcept { return { _data, _size }; }
        FST_NODISCARD constexpr reference operator[](size_type pos) noexcept { return _data[pos]; }
        FST_NODISCARD constexpr const_reference operator[](size_type pos) const noexcept { return _data[pos]; }

        FST_NODISCARD constexpr reference front() noexcept { return _data[0]; }
        FST_NODISCARD constexpr const_reference front() const noexcept { return _data[0]; }
        FST_NODISCARD constexpr reference back() noexcept { return _data[size() - 1]; }
        FST_NODISCARD constexpr const_reference back() const noexcept { return _data[size() - 1]; }

        FST_NODISCARD constexpr iterator begin() noexcept { return data(); }
        FST_NODISCARD constexpr const_iterator begin() const noexcept { return data(); }
        FST_NODISCARD constexpr const_iterator cbegin() const noexcept { return data(); }

        FST_NODISCARD constexpr iterator end() noexcept { return data() + size(); }
        FST_NODISCARD constexpr const_iterator end() const noexcept { return data() + size(); }
        FST_NODISCARD constexpr const_iterator cend() const noexcept { return end(); }

        inline constexpr void swap(basic_string_range& v) noexcept
        {
            __fst::mem_swap(_size, v._size);
            __fst::mem_swap(_data, v._data);
        }

        FST_NODISCARD inline constexpr basic_string_range substr(size_t pos = 0, size_t len = npos) const noexcept
        {
            const size_type max_length = pos > size() ? 0 : size() - pos;
            fst_assert(pos <= size(), "Index out of range in basic_string_range::substr");
            return basic_string_range(_data + pos, __fst::minimum(len, max_length));
        }

        FST_NODISCARD inline constexpr basic_string_range last(size_type __count) const noexcept FST_UNIT_TESTED
        {
            __count = __fst::minimum(size(), __count);
            return { data() + size() - __count, __count };
        }

        FST_NODISCARD inline constexpr int compare(basic_string_range v) const noexcept
        {
            if (size() != v.size()) { return size() < v.size() ? -1 : size() > v.size() ? 1 : 0; }

            return traits_type::compare(data(), v.data(), __fst::minimum(size(), v.size()));
        }

        constexpr void remove_prefix(const size_type _Count) noexcept
        {
            _data += _Count;
            _size -= _Count;
        }

        inline constexpr size_type find(value_type c) const noexcept
        {
            const value_type* ptr = traits_type::find(data(), size(), c);
            return ptr == nullptr ? npos : (size_type) __fst::pdistance(begin(), ptr);
        }

        inline constexpr size_type find(basic_string_range str) const noexcept
        {
            size_type n = size();
            size_type m = str.size();

            for (size_type i = 0; i <= n - m; ++i)
            {
                size_type j = 0;
                while (j < m && (*this)[i + j] == str[j])
                {
                    ++j;
                }
                if (j == m)
                { // match found
                    return i;
                }
            }
            return npos;
        }

        inline constexpr size_type find_first_of(value_type c) const noexcept { return find(c); }
        inline constexpr size_type find_first_of(basic_string_range str) const noexcept { return find(str); }

        inline constexpr size_type find_last_of(value_type c) const noexcept
        {
            if (empty()) { return npos; }

            for (size_type i = 0, k = _size - 1; i < _size; i++, k--)
            {
                if (_data[k] == c) { return k; }
            }

            return npos;
        }

        _CharT null_last() noexcept { return __fst::exchange(*end(), _CharT(0)); }

        void unnull_last(_CharT c) noexcept { *end() = c; }

        struct scoped_null
        {
            scoped_null(basic_string_range s)
                : str(s)
                , last(s.null_last())
            {}
            ~scoped_null() { str.unnull_last(last); }
            basic_string_range str;
            _CharT last;
        };

        inline scoped_null null_scope() { return scoped_null(*this); }

      private:
        pointer _data = nullptr;
        size_type _size = 0;
    };

    ///
    ///
    ///
    template <typename CharT, class SType, __fst::enable_if_t<is_utf_string_type<SType>::value && is_utf_char_type<CharT>::value, __fst::nullptr_t> = nullptr>
    inline size_t utf_cvt_size(const SType& str) noexcept;

    //
    template <class SType, __fst::enable_if_t<is_utf_string_type<SType>::value, __fst::nullptr_t> = nullptr>
    class utf_cvt;

    //
    template <typename _StringType, typename SType,
        __fst::enable_if_t<__fst::is_utf_basic_string_type<_StringType>::value && is_utf_string_type<SType>::value, __fst::nullptr_t> = nullptr>
    inline _StringType utf_cvt_as(const SType& str) noexcept;

    //
    template <class SType, class _OutputContainer,
        __fst::enable_if_t<is_utf_string_type<SType>::value && __fst::is_container_v<_OutputContainer>, __fst::nullptr_t> = nullptr>
    inline void utf_append_to(const SType& str, _OutputContainer& c_output) noexcept;

    ///
    ///
    ///
    template <class SType, class OutputIt, __fst::enable_if_t<is_utf_string_type<SType>::value, __fst::nullptr_t> = nullptr>
    inline OutputIt utf_copy(const SType& str, OutputIt outputIt) noexcept;

    ///
    ///
    ///
    template <class SType, __fst::enable_if_utf_string_type_t<SType> = nullptr>
    inline size_t utf_length(const SType& str) noexcept;

    template <typename CharT, typename SType, __fst::enable_if_t<is_utf_string_type<SType>::value && is_utf_char_type<CharT>::value, __fst::nullptr_t>>
    inline size_t utf_cvt_size(const SType& str) noexcept
    {
        using input_char_type = string_char_type_t<SType>;
        constexpr char_encoding input_encoding = __fst::utf_encoding_of<input_char_type>::value;

        using output_char_type = CharT;
        constexpr char_encoding output_encoding = __fst::utf_encoding_of<output_char_type>::value;

        __fst::basic_string_view<input_char_type> input_view(str);

        if constexpr (input_encoding == char_encoding::utf8)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return input_view.size(); }
            else if constexpr (output_encoding == char_encoding::utf16) { return __fst::utf::u8_to_u16_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf32) { return __fst::utf::u8_to_u32_length(input_view.begin(), input_view.end()); }
            else { return 0; }
        }
        else if constexpr (input_encoding == char_encoding::utf16)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return __fst::utf::u16_to_u8_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf16) { return input_view.size(); }
            else if constexpr (output_encoding == char_encoding::utf32) { return __fst::utf::u16_to_u32_length(input_view.begin(), input_view.end()); }
            else { return 0; }
        }
        else if constexpr (input_encoding == char_encoding::utf32)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return __fst::utf::u32_to_u8_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf16) { return __fst::utf::u32_to_u16_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf32) { return input_view.size(); }
            else { return 0; }
        }
        else { return 0; }
    }

    //
    template <class SType, class _OutputContainer, __fst::enable_if_t<is_utf_string_type<SType>::value && __fst::is_container_v<_OutputContainer>, __fst::nullptr_t>>
    inline void utf_append_to(const SType& str, _OutputContainer& c_output) noexcept
    {
        using input_char_type = string_char_type_t<SType>;
        constexpr char_encoding input_encoding = __fst::utf_encoding_of<input_char_type>::value;

        using output_char_type = __fst::container_value_type_t<_OutputContainer>;
        constexpr char_encoding output_encoding = __fst::utf_encoding_of<output_char_type>::value;

        __fst::basic_string_view<input_char_type> input_view(str);
        const size_t input_size = input_view.size();

        if constexpr (input_encoding == output_encoding)
        {
            const size_t output_size = c_output.size();
            c_output.resize(output_size + input_size);
            __fst::memmove(c_output.data() + output_size, input_view.data(), input_size * sizeof(output_char_type));
            return;
        }
        else if constexpr (input_encoding == char_encoding::utf8)
        {
            // u8 -> u16
            if constexpr (output_encoding == char_encoding::utf16)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = __fst::utf::u8_to_u16_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;
                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();

                while (cit_begin < cit_end)
                {
                    uint32_t cp = __fst::utf::next_u8_to_u32(cit_begin);

                    if (cp > 0xFFFF)
                    { // make a surrogate pair
                        *output_it++ = __fst::utf::cast_16((cp >> 10) + __fst::utf::k_lead_offset);
                        *output_it++ = __fst::utf::cast_16((cp & 0x3FF) + __fst::utf::k_trail_surrogate_min);
                    }
                    else { *output_it++ = __fst::utf::cast_16(cp); }
                }

                return;
            }

            // u8 ->u32
            else if constexpr (output_encoding == char_encoding::utf32)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = __fst::utf::u8_to_u32_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;
                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();
                while (cit_begin < cit_end)
                {
                    *output_it++ = static_cast<output_char_type>(__fst::utf::next_u8_to_u32(cit_begin));
                }
                return;
            }
        }
        else if constexpr (input_encoding == char_encoding::utf16)
        {
            // u16 -> u8
            if constexpr (output_encoding == char_encoding::utf8)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = __fst::utf::u16_to_u8_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;

                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();
                while (cit_begin != cit_end)
                {
                    uint32_t cp = __fst::utf::cast_16(*cit_begin++);

                    // Take care of surrogate pairs first.
                    if (__fst::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                    {
                        cp = (cp << 10) + static_cast<uint32_t>(__fst::utf::cast_16(*cit_begin++)) + __fst::utf::k_surrogate_offset;
                    }

                    output_it = __fst::utf::append_u32_to_u8(cp, output_it);
                }
                return;
            }

            // u16 -> u32
            else if constexpr (output_encoding == char_encoding::utf32)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = __fst::utf::u16_to_u32_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;
                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();
                while (cit_begin != cit_end)
                {
                    uint32_t cp = __fst::utf::cast_16(*cit_begin++);

                    // Take care of surrogate pairs first.
                    if (__fst::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                    {
                        cp = (cp << 10) + static_cast<uint32_t>(__fst::utf::cast_16(*cit_begin++)) + __fst::utf::k_surrogate_offset;
                    }

                    *output_it++ = static_cast<output_char_type>(cp);
                }
                return;
            }
        }
        else if constexpr (input_encoding == char_encoding::utf32)
        {
            // u32 -> u8
            if constexpr (output_encoding == char_encoding::utf8)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = __fst::utf::u32_to_u8_length(input_view.begin(), input_view.end()); //utf_cvt_size<output_char_type>(input_view);
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;

                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();

                while (cit_begin != cit_end)
                {
                    output_it = __fst::utf::append_u32_to_u8(static_cast<uint32_t>(*cit_begin++), output_it);
                }

                return;
            }

            // u32 -> u16
            else if constexpr (output_encoding == char_encoding::utf16)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = __fst::utf::u32_to_u16_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;

                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();

                while (cit_begin != cit_end)
                {
                    uint32_t cp = static_cast<uint32_t>(*cit_begin++);

                    if (cp <= 0x0000FFFF)
                    {
                        // UTF-16 surrogate values are illegal in UTF-32
                        // 0xFFFF or 0xFFFE are both reserved values.
                        if (cp >= 0xD800 && cp <= 0xDFFF) { *output_it++ = 0x0000FFFD; }
                        else
                        {
                            // BMP character.
                            *output_it++ = static_cast<output_char_type>(cp);
                        }
                    }
                    else if (cp > 0x0010FFFF)
                    {
                        // U+10FFFF is the largest code point of Unicode character set.
                        *output_it++ = static_cast<output_char_type>(0x0000FFFD);
                    }
                    else
                    {
                        // c32 is a character in range 0xFFFF - 0x10FFFF.
                        cp -= 0x0010000UL;
                        *output_it++ = static_cast<output_char_type>(((cp >> 10) + 0xD800));
                        *output_it++ = static_cast<output_char_type>(((cp & 0x3FFUL) + 0xDC00));
                    }
                }

                return;
            }
        }
    }

    template <class SType, class OutputIt, __fst::enable_if_t<__fst::is_utf_string_type<SType>::value, __fst::nullptr_t>>
    inline OutputIt utf_copy(const SType& str, OutputIt outputIt) noexcept
    {

        using input_char_type = string_char_type_t<SType>;
        constexpr char_encoding input_encoding = __fst::utf_encoding_of<input_char_type>::value;

        using output_char_type = __fst::output_iterator_value_type_t<OutputIt>;
        constexpr char_encoding output_encoding = __fst::utf_encoding_of<output_char_type>::value;

        __fst::basic_string_view<input_char_type> input_view(str);

        if constexpr (input_encoding == char_encoding::utf8)
        {
            if constexpr (output_encoding == char_encoding::utf8)
            {
                const size_t _size = input_view.size();
                for (size_t i = 0; i < _size; i++)
                {
                    *outputIt++ = static_cast<output_char_type>(input_view[i]);
                }
                return outputIt;
            }
            else if constexpr (output_encoding == char_encoding::utf16) { return __fst::utf::u8_to_u16(input_view.begin(), input_view.end(), outputIt); }
            else if constexpr (output_encoding == char_encoding::utf32) { return __fst::utf::u8_to_u32(input_view.begin(), input_view.end(), outputIt); }
        }
        else if constexpr (input_encoding == char_encoding::utf16)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return __fst::utf::u16_to_u8(input_view.begin(), input_view.end(), outputIt); }
            else if constexpr (output_encoding == char_encoding::utf16)
            {
                for (size_t i = 0; i < input_view.size(); i++)
                {
                    *outputIt++ = static_cast<output_char_type>(input_view[i]);
                }
                return outputIt;
            }
            else if constexpr (output_encoding == char_encoding::utf32) { return __fst::utf::u16_to_u32(input_view.begin(), input_view.end(), outputIt); }
        }
        else if constexpr (input_encoding == char_encoding::utf32)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return __fst::utf::u32_to_u8(input_view.begin(), input_view.end(), outputIt); }
            else if constexpr (output_encoding == char_encoding::utf16) { return __fst::utf::u32_to_u16(input_view.begin(), input_view.end(), outputIt); }
            else if constexpr (output_encoding == char_encoding::utf32)
            {
                for (size_t i = 0; i < input_view.size(); i++)
                {
                    *outputIt++ = static_cast<output_char_type>(input_view[i]);
                }
                return outputIt;
            }
        }
    }

    template <class SType, __fst::enable_if_utf_string_type_t<SType>>
    inline size_t utf_length(const SType& str) noexcept
    {
        using input_char_type = __fst::string_char_type_t<SType>;
        constexpr char_encoding input_encoding = __fst::utf_encoding_of<input_char_type>::value;

        __fst::basic_string_view<input_char_type> input_view(str);

        if constexpr (input_encoding == char_encoding::utf8) { return utf8_length(input_view.data(), input_view.size()); }
        else if constexpr (input_encoding == char_encoding::utf16) { return utf16_length(input_view.data(), input_view.size()); }
        else if constexpr (input_encoding == char_encoding::utf32) { return utf32_length(input_view.data(), input_view.size()); }
        else { return 0; }
    }

    ///
    ///
    ///
    template <class SType, __fst::enable_if_t<__fst::is_utf_string_type<SType>::value, __fst::nullptr_t>>
    class utf_cvt
    {
      public:
        using input_char_type = __fst::string_char_type_t<SType>;
        static constexpr char_encoding input_encoding = __fst::utf_encoding_of<input_char_type>::value;

        inline utf_cvt(const SType& str) noexcept
            : _input_view(str)
        {}

        template <typename _StringType, __fst::enable_if_t<__fst::is_utf_basic_string_type<_StringType>::value, __fst::nullptr_t> = nullptr>
        inline operator _StringType() const noexcept
        {
            return utf_cvt_as<_StringType>(_input_view);
        }

      private:
        __fst::basic_string_view<input_char_type> _input_view;
    };

    template <class SType>
    utf_cvt(const SType&) -> utf_cvt<SType>;

    template <typename _StringType, typename SType,
        __fst::enable_if_t<__fst::is_utf_basic_string_type<_StringType>::value && __fst::is_utf_string_type<SType>::value, __fst::nullptr_t>>
    inline _StringType utf_cvt_as(const SType& str) noexcept
    {
        _StringType out;
        __fst::utf_append_to(str, out);
        return out;
    }

    template <class SType, __fst::enable_if_utf_string_type_t<SType>>
    inline constexpr typename __fst::string_view_type<SType>::size_type find_string(const SType& s, __fst::string_view_type<SType> str) noexcept
    {
        using char_type = __fst::string_char_type_t<SType>;
        using view_type = __fst::basic_string_view<char_type>;
        using size_type = typename view_type::size_type;

        view_type ss(s);
        size_type n = ss.size();
        size_type m = str.size();

        for (size_type i = 0; i <= n - m; ++i)
        {
            size_type j = 0;
            while (j < m && ss[i + j] == str[j])
            {
                ++j;
            }

            if (j == m)
            { // match found
                return i;
            }
        }
        return view_type::npos;
    }

    template <class SType, __fst::enable_if_utf_string_type_t<SType>>
    size_t count_lines(const SType& str) noexcept
    {
        using char_type = string_char_type_t<SType>;
        using view_type = __fst::basic_string_view<char_type>;

        size_t count = 0;

        view_type strv(str);
        const char_type* ptr = strv.data(); //
        while (ptr)
        {
            count++;
            ptr = __fst::char_traits<char_type>::find(ptr + 1, __fst::pdistance(ptr + 1, strv.end()), '\n');
        }
        return count;
    }

    //
    template <class _CharT>
    struct line_range;

    //
    template <class _CharT>
    class line_iterator
    {
      public:
        using difference_type = ptrdiff_t;
        using iterator_category = __fst::forward_iterator_tag;
        using pointer = const _CharT*;

        inline line_iterator() noexcept = default;

        inline line_iterator(pointer en) noexcept
            : _begin(en)
            , _line_end(en)
            , _end(en)
        {}

        inline line_iterator(pointer b, pointer en) noexcept
            : _begin(b)
            , _line_end(nullptr)
            , _end(en)
        {
            _line_end = __fst::char_traits<_CharT>::find(b, __fst::pdistance(b, _end), '\n');
            if (!_line_end) { _line_end = _end; }
        }

        inline __fst::basic_string_view<_CharT> operator*() const noexcept { return __fst::basic_string_view<_CharT>(_begin, __fst::pdistance(_begin, _line_end)); }

        inline bool operator==(const line_iterator& rhs) const noexcept { return (_begin == rhs._begin) && (_line_end == rhs._line_end) && (_end == rhs._end); }

        inline bool operator!=(const line_iterator& rhs) const noexcept { return !(operator==(rhs)); }

        inline line_iterator& operator++() noexcept
        {
            _begin = ++_line_end;
            if (_line_end < _end)
            {
                if (pointer line_end = __fst::char_traits<_CharT>::find(_line_end, __fst::pdistance(_line_end, _end), '\n')) { _line_end = line_end; }

                else { _line_end = _end; }
                return *this;
            }

            _begin = _line_end = _end;
            return *this;
        }

        inline line_iterator operator++(int) noexcept
        {
            line_iterator temp = *this;
            _begin = ++_line_end;
            if (_line_end < _end)
            {
                if (pointer line_end = ::__fst::char_traits<_CharT>::find(_line_end, __fst::pdistance(_line_end, _end), '\n')) { _line_end = line_end; }
                else { _line_end = _end; }
                return temp;
            }

            _begin = _line_end = _end;
            return temp;
        }

      private:
        pointer _begin = nullptr;
        pointer _line_end = nullptr;
        pointer _end = nullptr;

        template <class>
        friend struct line_range;

        inline line_iterator end() const noexcept { return line_iterator(_end); }
    };

    //
    template <class _CharT>
    struct line_range
    {
        using iterator_type = __fst::line_iterator<_CharT>;
        iterator_type _begin_iterator;

        inline line_range(__fst::basic_string_view<_CharT> c) noexcept
            : _begin_iterator(c.begin(), c.end())
        {}

        template <class _Container, __fst::enable_if_t<__fst::is_container_v<_Container>, int> = 0>
        inline line_range(const _Container& c) noexcept
            : line_range(__fst::basic_string_view<_CharT>(c))
        {}

        inline iterator_type begin() const noexcept { return _begin_iterator; }
        inline iterator_type end() const noexcept { return _begin_iterator.end(); }
        inline bool empty() const noexcept { return _begin_iterator == _begin_iterator.end(); }
    };

    template <class _Container, __fst::enable_if_t<__fst::is_container_v<_Container>, int> = 0>
    line_range(const _Container&) -> line_range<__fst::container_data_type_t<_Container>>;
   

FST_END_NAMESPACE
