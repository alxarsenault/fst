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
#include "fst/iterator.h"

#include "fst/utility.h"

FST_BEGIN_NAMESPACE

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
    struct is_utf_char_type : _FST::disjunction<
          _FST::is_same_rcv<char, CharT>
        , _FST::is_same_rcv<char16_t, CharT>
        , _FST::is_same_rcv<char32_t, CharT>
        , _FST::is_same_rcv<wchar_t, CharT>
        FST_IF_CHAR8_T(, _FST::is_same_rcv<char8_t, CharT>)>
    {};
    // clang-format on

    ///
    ///
    template <size_t _CSize>
    struct is_utf_char_size : _FST::bool_t<_CSize == sizeof(char) || _CSize == sizeof(char16_t) || _CSize == sizeof(char32_t)>
    {};

    template <class _CharT, class _T>
    struct is_basic_string_view_convertible : _FST::is_convertible<const _T&, _FST::basic_string_view<_CharT>>
    {};

    template <class _CharT, class _T>
    FST_INLINE_VAR constexpr bool is_basic_string_view_convertible_v = _FST::is_basic_string_view_convertible<_CharT, _T>::value;

    template <class _CharT, class _T>
    struct is_basic_string_view_constructible : _FST::is_constructible<_FST::basic_string_view<_CharT>, _T>
    {};

    template <class _CharT, class _T>
    struct is_basic_string_view_convertible_not_char_buffer
        : _FST::bool_t<_FST::is_basic_string_view_convertible<_CharT, _T>::value && !_FST::is_convertible<const _T&, const _CharT*>::value>
    {};

    template <class _CharT, class _T>
    FST_INLINE_VAR constexpr bool is_basic_string_view_convertible_not_char_buffer_v = _FST::is_basic_string_view_convertible_not_char_buffer<_CharT, _T>::value;

    // clang-format off
    template <class _T> struct is_string_view_convertible : _FST::is_convertible<const _T&, _FST::basic_string_view<char>> {};
    template <class _T> struct is_wstring_view_convertible : _FST::is_convertible<const _T&, _FST::basic_string_view<wchar_t>> {};
    template <class _T> struct is_u16string_view_convertible : _FST::is_convertible<const _T&, _FST::basic_string_view<char16_t>> {};
    template <class _T> struct is_u32string_view_convertible : _FST::is_convertible<const _T&, _FST::basic_string_view<char32_t>> {};

    template <class _T> struct is_string_view_convertible_not_char_buffer : _FST::is_basic_string_view_convertible_not_char_buffer<char, _T> {};
    template <class _T> struct is_wstring_view_convertible_not_char_buffer : _FST::is_basic_string_view_convertible_not_char_buffer<wchar_t, _T> {};
    template <class _T> struct is_u16string_view_convertible_not_char_buffer : _FST::is_basic_string_view_convertible_not_char_buffer<char16_t, _T> {};
    template <class _T> struct is_u32string_view_convertible_not_char_buffer : _FST::is_basic_string_view_convertible_not_char_buffer<char32_t, _T> {};

#if FST_HAS_CHAR8_T
    template <class _T> struct is_u8string_view_convertible : _FST::is_convertible<const _T&, _FST::basic_string_view<char8_t>> {};
    template <class _T> struct is_u8string_view_convertible_not_char_buffer : _FST::is_basic_string_view_convertible_not_char_buffer<char8_t, _T> {};
#endif // FST_HAS_CHAR8_T
    // clang-format on

    template <class SType>
    struct is_utf_string_type
        : _FST::bool_t<_FST::is_constructible_v<_FST::basic_string_view<char>, SType> //
                       || _FST::is_constructible_v<_FST::basic_string_view<char16_t>, SType> //
                       || _FST::is_constructible_v<_FST::basic_string_view<char32_t>, SType> //
                       || _FST::is_constructible_v<_FST::basic_string_view<wchar_t>, SType> //
                           FST_IF_CHAR8_T(|| _FST::is_constructible_v<_FST::basic_string_view<char8_t>, SType>)>
    {};

    template <class SType>
    using enable_if_utf_string_type_t = _FST::enable_if_t<_FST::is_utf_string_type<SType>::value, _FST::nullptr_t>;

    template <class SType>
    struct is_utf_basic_string_type : _FST::bool_t<_FST::is_utf_string_type<SType>::value && _FST::has_push_back_v<SType>>
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

    template <class SType, _FST::enable_if_t<_FST::is_utf_string_type<SType>::value, _FST::nullptr_t> = nullptr>
    using string_view_type = _FST::basic_string_view<_FST::string_char_type_t<SType>>;

    ///
    ///
    ///
    template <typename CharT, _FST::enable_if_t<_FST::is_utf_char_type<CharT>::value, _FST::nullptr_t> = nullptr>
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

    template <typename CharT, _FST::enable_if_t<sizeof(CharT) == sizeof(char), _FST::nullptr_t> = nullptr>
    size_t utf8_length(const CharT* str, size_t size) noexcept;

    template <typename CharT, _FST::enable_if_t<sizeof(CharT) == sizeof(char16_t), _FST::nullptr_t> = nullptr>
    size_t utf16_length(const CharT* str, size_t size) noexcept;

    template <typename CharT, _FST::enable_if_t<sizeof(CharT) == sizeof(char32_t), _FST::nullptr_t> = nullptr>
    size_t utf32_length(const CharT*, size_t size) noexcept;

    template <class SType, class _CharT>
    using enable_if_string_view_ctor_t = _FST::enable_if_t<_FST::is_basic_string_view_constructible<_CharT, _FST::remove_cvref_t<SType>>::value>;

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
    template <class SType, _FST::enable_if_utf_string_type_t<SType> = nullptr>
    inline constexpr typename _FST::string_view_type<SType>::size_type find_string(const SType& s, _FST::string_view_type<SType> str) noexcept;

    ///
    template <class SType, _FST::enable_if_utf_string_type_t<SType> = nullptr>
    size_t count_lines(const SType& str) noexcept;

    ///
    template <class _CharT>
    struct char_traits
    {
        using char_type = _CharT;
        using int_type = int;
        using pos_type = ::ptrdiff_t;
        using off_type = ::ptrdiff_t;

        static inline constexpr size_t length(const char_type* s) { return _FST::strlen(s); }

        static inline constexpr int compare(const char_type* s1, const char_type* s2, size_t count) { return _FST::strncmp(s1, s2, count); }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, const char_type _Ch) noexcept { return ::strchr(_First, _Ch); }

        FST_NODISCARD static constexpr const char_type* find(const char_type* const _First, size_t _Count, const char_type _Ch) noexcept
        {

#if __has_builtin(__builtin_char_memchr)
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
            using ctype = _FST::output_iterator_value_type_t<u8_iterator>;

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
            uint32_t cp = _FST::utf::cast_8(*it);

            using difference_type = typename _FST::iterator_traits<u8_iterator>::difference_type;
            difference_type length = static_cast<difference_type>(_FST::utf::sequence_length(static_cast<uint8_t>(*it)));

            switch (length)
            {
            case 1: break;
            case 2:
                it++;
                cp = ((cp << 6) & 0x7FF) + ((*it) & 0x3F);
                break;
            case 3:
                ++it;
                cp = ((cp << 12) & 0xFFFF) + ((_FST::utf::cast_8(*it) << 6) & 0xFFF);
                ++it;
                cp += (*it) & 0x3F;
                break;
            case 4:
                ++it;
                cp = ((cp << 18) & 0x1FFFFF) + ((_FST::utf::cast_8(*it) << 12) & 0x3FFFF);
                ++it;
                cp += (_FST::utf::cast_8(*it) << 6) & 0xFFF;
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
            while (_FST::utf::is_trail(*(--it)))
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
                    *outputIt++ = _FST::utf::cast_16((cp >> 10) + _FST::utf::k_lead_offset);
                    *outputIt++ = _FST::utf::cast_16((cp & 0x3FF) + _FST::utf::k_trail_surrogate_min);
                }
                else { *outputIt++ = _FST::utf::cast_16(cp); }
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
            using ctype = _FST::output_iterator_value_type_t<u32_iterator>;

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
                switch (_FST::utf::sequence_length(static_cast<uint8_t>(*start++)))
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
                uint32_t cp = _FST::utf::cast_16(*start++);
                //

                // Take care of surrogate pairs first.
                if (_FST::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(_FST::utf::cast_16(*start++)) + _FST::utf::k_surrogate_offset;
                }

                outputIt = append_u32_to_u8(cp, outputIt);
            }

            return outputIt;
        }

        template <typename u16_iterator, typename u32_iterator>
        u32_iterator u16_to_u32(u16_iterator start, u16_iterator end, u32_iterator outputIt) noexcept
        {
            using ctype = _FST::output_iterator_value_type_t<u32_iterator>;

            while (start != end)
            {
                uint32_t cp = _FST::utf::cast_16(*start++);

                // Take care of surrogate pairs first.
                if (_FST::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(_FST::utf::cast_16(*start++)) + _FST::utf::k_surrogate_offset;
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
                uint32_t cp = _FST::utf::cast_16(*start++);

                // Take care of surrogate pairs first.
                if (_FST::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(_FST::utf::cast_16(*start++)) + _FST::utf::k_surrogate_offset;
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
                uint32_t cp = _FST::utf::cast_16(*start++);

                // Take care of surrogate pairs first.
                if (_FST::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                {
                    cp = (cp << 10) + static_cast<uint32_t>(_FST::utf::cast_16(*start++)) + _FST::utf::k_surrogate_offset;
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

                using value_type = _FST::output_iterator_value_type_t<u16_iterator>;

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

    template <typename CharT, _FST::enable_if_t<sizeof(CharT) == sizeof(char), _FST::nullptr_t>>
    size_t utf8_length(const CharT* str, size_t size) noexcept
    {
        size_t dist = 0;

        for (size_t i = 0; i < size; i += _FST::utf::sequence_length(static_cast<uint8_t>(str[i])))
        {
            dist++;
        }

        return dist;
    }

    template <typename CharT, _FST::enable_if_t<sizeof(CharT) == sizeof(char16_t), _FST::nullptr_t>>
    size_t utf16_length(const CharT* str, size_t size) noexcept
    {
        size_t dist = 0;

        for (size_t i = 0; i < size; i++)
        {
            if (_FST::utf::is_high_surrogate(_FST::utf::cast_16(str[i]))) { i++; }

            dist++;
        }

        return dist;
    }

    template <typename CharT, _FST::enable_if_t<sizeof(CharT) == sizeof(char32_t), _FST::nullptr_t>>
    size_t utf32_length(const CharT*, size_t size) noexcept
    {
        return size;
    }

    template <typename _CharT>
    class basic_string_view
    {
      public:
        using value_type = _CharT;
        using traits_type = _FST::char_traits<_CharT>;
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
        inline constexpr basic_string_view(const _FST::array<value_type, _Size>& arr) noexcept
            : _data{ arr.data() }
            , _size{ (size_type) _Size }
        {}

        template <class _Container, _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_FST::remove_cvref_t<_Container>, basic_string_view>
                                                          && _FST::is_same_v<_FST::container_value_type_t<_Container>, value_type>,
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
            _FST::memswap(_size, v._size);
            _FST::memswap(_data, v._data);
        }

        FST_NODISCARD inline constexpr basic_string_view substr(size_t pos = 0, size_t len = npos) const noexcept
        {
            const size_type max_length = pos > size() ? 0 : size() - pos;
            fst_assert(pos <= size(), "Index out of range in basic_string_view::substr");
            return basic_string_view(_data + pos, _FST::minimum(len, max_length));
        }

        FST_NODISCARD inline constexpr int compare(basic_string_view v) const noexcept
        {
            if (size() != v.size()) { return size() < v.size() ? -1 : size() > v.size() ? 1 : 0; }

            return traits_type::compare(data(), v.data(), _FST::minimum(size(), v.size()));
        }

        constexpr void remove_prefix(const size_type _Count) noexcept
        {
            _data += _Count;
            _size -= _Count;
        }

        inline constexpr size_type find(value_type c) const noexcept
        {
            const value_type* ptr = traits_type::find(data(), size(), c);
            return ptr == nullptr ? npos : (size_type) _FST::pdistance(begin(), ptr);
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

        inline constexpr size_type find_first_of(value_type c) const noexcept { return find(c); }
        inline constexpr size_type find_first_of(basic_string_view str) const noexcept { return find(str); }

      private:
        const_pointer _data = nullptr;
        size_type _size = 0;
    };

    template <class SType, class _CharT, _FST::enable_if_t<_FST::is_utf_string_type<SType>::value, _FST::nullptr_t> = nullptr>
    inline _FST::array<_FST::basic_string_view<string_char_type_t<SType>>, 2> strsplit(const SType& str, _CharT sp) noexcept
    {
        using char_type = string_char_type_t<SType>;
        using view_type = _FST::basic_string_view<char_type>;
        using ret_type = _FST::array<view_type, 2>;

        view_type strv(str);
        const char_type* ptr = _FST::char_traits<char_type>::find(strv.begin(), strv.size(), sp);
        if (ptr)
        {
            size_t delta = _FST::pdistance(strv.begin(), ptr);
            return ret_type{ view_type(strv.substr(0, delta)), strv.substr(delta + 1) };
        }

        return ret_type{ strv, view_type() };
    }
    ///
    ///
    ///

    ///
    ///
    ///
    template <typename CharT, class SType, _FST::enable_if_t<is_utf_string_type<SType>::value && is_utf_char_type<CharT>::value, _FST::nullptr_t> = nullptr>
    inline size_t utf_cvt_size(const SType& str) noexcept;

    //
    template <class SType, _FST::enable_if_t<is_utf_string_type<SType>::value, _FST::nullptr_t> = nullptr>
    class utf_cvt;

    //
    template <typename _StringType, typename SType,
        _FST::enable_if_t<_FST::is_utf_basic_string_type<_StringType>::value && is_utf_string_type<SType>::value, _FST::nullptr_t> = nullptr>
    inline _StringType utf_cvt_as(const SType& str) noexcept;

    //
    template <class SType, class _OutputContainer, _FST::enable_if_t<is_utf_string_type<SType>::value && _FST::is_container_v<_OutputContainer>, _FST::nullptr_t> = nullptr>
    inline void utf_append_to(const SType& str, _OutputContainer& c_output) noexcept;

    ///
    ///
    ///
    template <class SType, class OutputIt, _FST::enable_if_t<is_utf_string_type<SType>::value, _FST::nullptr_t> = nullptr>
    inline OutputIt utf_copy(const SType& str, OutputIt outputIt) noexcept;

    ///
    ///
    ///
    template <class SType, _FST::enable_if_utf_string_type_t<SType> = nullptr>
    inline size_t utf_length(const SType& str) noexcept;

    template <typename CharT, typename SType, _FST::enable_if_t<is_utf_string_type<SType>::value && is_utf_char_type<CharT>::value, _FST::nullptr_t>>
    inline size_t utf_cvt_size(const SType& str) noexcept
    {
        using input_char_type = string_char_type_t<SType>;
        constexpr char_encoding input_encoding = _FST::utf_encoding_of<input_char_type>::value;

        using output_char_type = CharT;
        constexpr char_encoding output_encoding = _FST::utf_encoding_of<output_char_type>::value;

        _FST::basic_string_view<input_char_type> input_view(str);

        if constexpr (input_encoding == char_encoding::utf8)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return input_view.size(); }
            else if constexpr (output_encoding == char_encoding::utf16) { return _FST::utf::u8_to_u16_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf32) { return _FST::utf::u8_to_u32_length(input_view.begin(), input_view.end()); }
            else { return 0; }
        }
        else if constexpr (input_encoding == char_encoding::utf16)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return _FST::utf::u16_to_u8_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf16) { return input_view.size(); }
            else if constexpr (output_encoding == char_encoding::utf32) { return _FST::utf::u16_to_u32_length(input_view.begin(), input_view.end()); }
            else { return 0; }
        }
        else if constexpr (input_encoding == char_encoding::utf32)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return _FST::utf::u32_to_u8_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf16) { return _FST::utf::u32_to_u16_length(input_view.begin(), input_view.end()); }
            else if constexpr (output_encoding == char_encoding::utf32) { return input_view.size(); }
            else { return 0; }
        }
        else { return 0; }
    }

    //
    template <class SType, class _OutputContainer, _FST::enable_if_t<is_utf_string_type<SType>::value && _FST::is_container_v<_OutputContainer>, _FST::nullptr_t>>
    inline void utf_append_to(const SType& str, _OutputContainer& c_output) noexcept
    {
        using input_char_type = string_char_type_t<SType>;
        constexpr char_encoding input_encoding = _FST::utf_encoding_of<input_char_type>::value;

        using output_char_type = _FST::container_value_type_t<_OutputContainer>;
        constexpr char_encoding output_encoding = _FST::utf_encoding_of<output_char_type>::value;

        _FST::basic_string_view<input_char_type> input_view(str);
        const size_t input_size = input_view.size();

        if constexpr (input_encoding == output_encoding)
        {
            const size_t output_size = c_output.size();
            c_output.resize(output_size + input_size);
            _FST::memmove(c_output.data() + output_size, input_view.data(), input_size * sizeof(output_char_type));
            return;
        }
        else if constexpr (input_encoding == char_encoding::utf8)
        {
            // u8 -> u16
            if constexpr (output_encoding == char_encoding::utf16)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = _FST::utf::u8_to_u16_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;
                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();

                while (cit_begin < cit_end)
                {
                    uint32_t cp = _FST::utf::next_u8_to_u32(cit_begin);

                    if (cp > 0xFFFF)
                    { // make a surrogate pair
                        *output_it++ = _FST::utf::cast_16((cp >> 10) + _FST::utf::k_lead_offset);
                        *output_it++ = _FST::utf::cast_16((cp & 0x3FF) + _FST::utf::k_trail_surrogate_min);
                    }
                    else { *output_it++ = _FST::utf::cast_16(cp); }
                }

                return;
            }

            // u8 ->u32
            else if constexpr (output_encoding == char_encoding::utf32)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = _FST::utf::u8_to_u32_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;
                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();
                while (cit_begin < cit_end)
                {
                    *output_it++ = static_cast<output_char_type>(_FST::utf::next_u8_to_u32(cit_begin));
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
                const size_t conv_size = _FST::utf::u16_to_u8_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;

                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();
                while (cit_begin != cit_end)
                {
                    uint32_t cp = _FST::utf::cast_16(*cit_begin++);

                    // Take care of surrogate pairs first.
                    if (_FST::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                    {
                        cp = (cp << 10) + static_cast<uint32_t>(_FST::utf::cast_16(*cit_begin++)) + _FST::utf::k_surrogate_offset;
                    }

                    output_it = _FST::utf::append_u32_to_u8(cp, output_it);
                }
                return;
            }

            // u16 -> u32
            else if constexpr (output_encoding == char_encoding::utf32)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = _FST::utf::u16_to_u32_length(input_view.begin(), input_view.end());
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;
                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();
                while (cit_begin != cit_end)
                {
                    uint32_t cp = _FST::utf::cast_16(*cit_begin++);

                    // Take care of surrogate pairs first.
                    if (_FST::utf::is_high_surrogate(static_cast<char16_t>(cp)))
                    {
                        cp = (cp << 10) + static_cast<uint32_t>(_FST::utf::cast_16(*cit_begin++)) + _FST::utf::k_surrogate_offset;
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
                const size_t conv_size = _FST::utf::u32_to_u8_length(input_view.begin(), input_view.end()); //utf_cvt_size<output_char_type>(input_view);
                c_output.resize(output_size + conv_size);
                output_char_type* output_it = c_output.data() + output_size;

                auto cit_begin = input_view.begin();
                auto cit_end = input_view.end();

                while (cit_begin != cit_end)
                {
                    output_it = _FST::utf::append_u32_to_u8(static_cast<uint32_t>(*cit_begin++), output_it);
                }

                return;
            }

            // u32 -> u16
            else if constexpr (output_encoding == char_encoding::utf16)
            {
                const size_t output_size = c_output.size();
                const size_t conv_size = _FST::utf::u32_to_u16_length(input_view.begin(), input_view.end());
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

    template <class SType, class OutputIt, _FST::enable_if_t<_FST::is_utf_string_type<SType>::value, _FST::nullptr_t>>
    inline OutputIt utf_copy(const SType& str, OutputIt outputIt) noexcept
    {

        using input_char_type = string_char_type_t<SType>;
        constexpr char_encoding input_encoding = _FST::utf_encoding_of<input_char_type>::value;

        using output_char_type = _FST::output_iterator_value_type_t<OutputIt>;
        constexpr char_encoding output_encoding = _FST::utf_encoding_of<output_char_type>::value;

        _FST::basic_string_view<input_char_type> input_view(str);

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
            else if constexpr (output_encoding == char_encoding::utf16) { return _FST::utf::u8_to_u16(input_view.begin(), input_view.end(), outputIt); }
            else if constexpr (output_encoding == char_encoding::utf32) { return _FST::utf::u8_to_u32(input_view.begin(), input_view.end(), outputIt); }
        }
        else if constexpr (input_encoding == char_encoding::utf16)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return _FST::utf::u16_to_u8(input_view.begin(), input_view.end(), outputIt); }
            else if constexpr (output_encoding == char_encoding::utf16)
            {
                for (size_t i = 0; i < input_view.size(); i++)
                {
                    *outputIt++ = static_cast<output_char_type>(input_view[i]);
                }
                return outputIt;
            }
            else if constexpr (output_encoding == char_encoding::utf32) { return _FST::utf::u16_to_u32(input_view.begin(), input_view.end(), outputIt); }
        }
        else if constexpr (input_encoding == char_encoding::utf32)
        {
            if constexpr (output_encoding == char_encoding::utf8) { return _FST::utf::u32_to_u8(input_view.begin(), input_view.end(), outputIt); }
            else if constexpr (output_encoding == char_encoding::utf16) { return _FST::utf::u32_to_u16(input_view.begin(), input_view.end(), outputIt); }
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

    template <class SType, _FST::enable_if_utf_string_type_t<SType>>
    inline size_t utf_length(const SType& str) noexcept
    {
        using input_char_type = _FST::string_char_type_t<SType>;
        constexpr char_encoding input_encoding = _FST::utf_encoding_of<input_char_type>::value;

        _FST::basic_string_view<input_char_type> input_view(str);

        if constexpr (input_encoding == char_encoding::utf8) { return utf8_length(input_view.data(), input_view.size()); }
        else if constexpr (input_encoding == char_encoding::utf16) { return utf16_length(input_view.data(), input_view.size()); }
        else if constexpr (input_encoding == char_encoding::utf32) { return utf32_length(input_view.data(), input_view.size()); }
        else { return 0; }
    }

    ///
    ///
    ///
    template <class SType, _FST::enable_if_t<_FST::is_utf_string_type<SType>::value, _FST::nullptr_t>>
    class utf_cvt
    {
      public:
        using input_char_type = _FST::string_char_type_t<SType>;
        static constexpr char_encoding input_encoding = _FST::utf_encoding_of<input_char_type>::value;

        inline utf_cvt(const SType& str) noexcept
            : _input_view(str)
        {}

        template <typename _StringType, _FST::enable_if_t<_FST::is_utf_basic_string_type<_StringType>::value, _FST::nullptr_t> = nullptr>
        inline operator _StringType() const noexcept
        {
            return utf_cvt_as<_StringType>(_input_view);
        }

      private:
        _FST::basic_string_view<input_char_type> _input_view;
    };

    template <class SType>
    utf_cvt(const SType&) -> utf_cvt<SType>;

    template <typename _StringType, typename SType,
        _FST::enable_if_t<_FST::is_utf_basic_string_type<_StringType>::value && _FST::is_utf_string_type<SType>::value, _FST::nullptr_t>>
    inline _StringType utf_cvt_as(const SType& str) noexcept
    {
        _StringType out;
        _FST::utf_append_to(str, out);
        return out;
    }

    template <class SType, _FST::enable_if_utf_string_type_t<SType>>
    inline constexpr typename _FST::string_view_type<SType>::size_type find_string(const SType& s, _FST::string_view_type<SType> str) noexcept
    {
        using char_type = _FST::string_char_type_t<SType>;
        using view_type = _FST::basic_string_view<char_type>;
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

    template <class SType, _FST::enable_if_utf_string_type_t<SType>>
    size_t count_lines(const SType& str) noexcept
    {
        using char_type = string_char_type_t<SType>;
        using view_type = _FST::basic_string_view<char_type>;

        size_t count = 0;

        view_type strv(str);
        const char_type* ptr = strv.data(); //
        while (ptr)
        {
            count++;
            ptr = _FST::char_traits<char_type>::find(ptr + 1, _FST::pdistance(ptr + 1, strv.end()), '\n');
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
        using iterator_category = _FST::forward_iterator_tag;
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
            _line_end = _FST::char_traits<_CharT>::find(b, _FST::pdistance(b, _end), '\n');
            if (!_line_end) { _line_end = _end; }
        }

        inline _FST::basic_string_view<_CharT> operator*() const noexcept { return _FST::basic_string_view<_CharT>(_begin, _FST::pdistance(_begin, _line_end)); }

        inline bool operator==(const line_iterator& rhs) const noexcept { return (_begin == rhs._begin) && (_line_end == rhs._line_end) && (_end == rhs._end); }

        inline bool operator!=(const line_iterator& rhs) const noexcept { return !(operator==(rhs)); }

        inline line_iterator& operator++() noexcept
        {
            _begin = ++_line_end;
            if (_line_end < _end)
            {
                if (pointer line_end = _FST::char_traits<_CharT>::find(_line_end, _FST::pdistance(_line_end, _end), '\n')) { _line_end = line_end; }

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
                if (pointer line_end = ::_FST::char_traits<_CharT>::find(_line_end, _FST::pdistance(_line_end, _end), '\n')) { _line_end = line_end; }
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
        using iterator_type = _FST::line_iterator<_CharT>;
        iterator_type _begin_iterator;

        inline line_range(_FST::basic_string_view<_CharT> c) noexcept
            : _begin_iterator(c.begin(), c.end())
        {}

        template <class _Container, _FST::enable_if_t<_FST::is_container_v<_Container>, int> = 0>
        inline line_range(const _Container& c) noexcept
            : line_range(_FST::basic_string_view<_CharT>(c))
        {}

        inline iterator_type begin() const noexcept { return _begin_iterator; }
        inline iterator_type end() const noexcept { return _begin_iterator.end(); }
        inline bool empty() const noexcept { return _begin_iterator == _begin_iterator.end(); }
    };

    template <class _Container, _FST::enable_if_t<_FST::is_container_v<_Container>, int> = 0>
    line_range(const _Container&) -> line_range<_FST::container_data_type_t<_Container>>;
    //

FST_END_NAMESPACE
