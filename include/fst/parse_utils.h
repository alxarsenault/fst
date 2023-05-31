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
#include "fst/string_view.h"
#include "fst/utility.h"

FST_BEGIN_NAMESPACE
    FST_NODISCARD inline constexpr bool is_end_of_line(char c) noexcept
    {
        return c == '\n' || c == '\r';
    }

    FST_NODISCARD inline constexpr char distance_between_lower_and_upper_case() noexcept
    {
        return 'a' - 'A';
    }

    FST_NODISCARD inline constexpr bool is_char(char c) noexcept
    {
        return c >= 0;
    }

    FST_NODISCARD inline constexpr bool is_null(char c) noexcept
    {
        return c == 0;
    }

    FST_NODISCARD inline constexpr bool is_space(char c) noexcept
    {
        return c == ' ';
    }

    FST_NODISCARD inline constexpr bool is_tab(char c) noexcept
    {
        return c == '\t';
    }

    FST_NODISCARD inline constexpr bool is_space_or_tab(char c) noexcept
    {
        return is_space(c) || is_tab(c);
    }

    FST_NODISCARD inline constexpr bool is_digit(char c) noexcept
    {
        return c >= '0' && c <= '9';
    }

    FST_NODISCARD inline constexpr bool is_letter(char c) noexcept
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    FST_NODISCARD inline constexpr bool is_lower_case_letter(char c) noexcept
    {
        return c >= 'a' && c <= 'z';
    }

    FST_NODISCARD inline constexpr bool is_upper_case_letter(char c) noexcept
    {
        return c >= 'A' && c <= 'Z';
    }

    FST_NODISCARD inline constexpr bool is_alphanumeric(char c) noexcept
    {
        return is_digit(c) || is_letter(c);
    }

    FST_NODISCARD inline constexpr bool is_letter_or_underscore(char c) noexcept
    {
        return is_letter(c) || c == '_';
    }

    FST_NODISCARD inline constexpr bool is_alphanumeric_or_underscore(char c) noexcept
    {
        return is_alphanumeric(c) || c == '_';
    }

    FST_NODISCARD inline constexpr bool is_operator(char c) noexcept
    {
        return c == '<' || c == '>' || c == '=';
    }

    FST_NODISCARD inline constexpr bool is_dot(char c) noexcept
    {
        return c == '.';
    }

    FST_NODISCARD inline constexpr bool is_logical_or(char c) noexcept
    {
        return c == '|';
    } 

    FST_NODISCARD inline constexpr bool is_hyphen(char c) noexcept
    {
        return c == '-';
    }

    FST_NODISCARD inline constexpr bool is_hex(char c) noexcept
    {
        return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    FST_NODISCARD inline constexpr bool is_control(char c) noexcept
    {
        return (c >= 0 && c <= 31) || c == 127;
    }

    FST_NODISCARD inline constexpr bool is_special(char c) noexcept
    {
        // ! " # $ % & ' ( ) * + - . / : ; < = > ? @ [ \ ] ^ _ ` { | } ~
        return (c >= 32 && c <= 47) || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || (c >= 123 && c <= 126);
    }

    FST_NODISCARD inline constexpr uint8_t to_digit(char c) noexcept
    {
        fst_assert(__fst::is_digit(c), "char must be a digit");
        return c - '0';
    }

    FST_NODISCARD inline constexpr bool is_trim_char(char c) noexcept
    {
        switch (c)
        {
        case '\n':
        case '\r':
        case '\t':
        case '\v':
        case ' ': return true;
        }
        return false;
    }

    FST_NODISCARD inline constexpr char to_upper_case(char c) noexcept
    {
        return __fst::is_lower_case_letter(c) ? (c - __fst::distance_between_lower_and_upper_case()) : c;
    }

    FST_NODISCARD inline constexpr char to_lower_case(char c) noexcept
    {
        return __fst::is_upper_case_letter(c) ? (c + __fst::distance_between_lower_and_upper_case()) : c;
    }

    FST_NODISCARD inline constexpr char to_lower(char c) noexcept
    {
        return __fst::to_lower_case(c);
    }

    FST_NODISCARD inline constexpr char to_upper(char c) noexcept
    {
        return __fst::to_upper_case(c);
    }

    ///
    FST_NODISCARD inline __fst::string_view ltrim(__fst::string_view s) noexcept;
    FST_NODISCARD inline __fst::string_view ltrim(__fst::string_view s, char c_to_trim) noexcept;

    ///
    FST_NODISCARD inline __fst::string_view rtrim(__fst::string_view s) noexcept;
    FST_NODISCARD inline __fst::string_view rtrim(__fst::string_view s, char c_to_trim) noexcept;

    ///
    FST_NODISCARD inline __fst::string_view trim(__fst::string_view s) noexcept;
    FST_NODISCARD inline __fst::string_view trim(__fst::string_view s, char c_to_trim) noexcept;

    FST_NODISCARD inline size_t index_to_line(__fst::string_view str, size_t index) noexcept
    {
        fst_assert(index < str.size());

        size_t end_line_count = 0;
        for (size_t i = 0; i <= index; i++)
        {
            if (str[i] == '\n') { end_line_count++; }
        }
        return end_line_count + 1;
    }

    FST_NODISCARD inline constexpr bool is_upper_case(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        fst_assert(sz != 0);

        for (size_t i = 0; i < sz; i++)
        {
            if (__fst::is_letter(s[i]) && !__fst::is_upper_case_letter(s[i])) { return false; }
        }

        return true;
    }

    FST_NODISCARD inline constexpr bool is_lower_case(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        fst_assert(sz != 0);

        for (size_t i = 0; i < sz; i++)
        {
            if (__fst::is_letter(s[i]) && !__fst::is_lower_case_letter(s[i])) { return false; }
        }

        return true;
    }

    FST_NODISCARD inline constexpr bool is_alphanumeric(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        fst_assert(sz != 0);

        for (size_t i = 0; i < sz; i++)
        {
            if (!__fst::is_alphanumeric(s[i])) { return false; }
        }

        return true;
    }

    FST_NODISCARD inline constexpr bool is_alphanumeric_with_spaces(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        fst_assert(sz != 0);

        for (size_t i = 0; i < sz; i++)
        {
            if (!(__fst::is_alphanumeric(s[i]) || __fst::is_space(s[i]))) { return false; }
        }

        return true;
    }

    FST_NODISCARD inline constexpr bool has_leading_spaces(__fst::string_view s) noexcept
    {
        fst_assert(!s.empty());
        return __fst::is_space(s[0]);
    }

    FST_NODISCARD inline constexpr bool has_trailing_spaces(__fst::string_view s) noexcept
    {
        fst_assert(!s.empty());
        return __fst::is_space(s.back());
    }

    FST_NODISCARD inline constexpr bool has_end_of_line(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        for (size_t i = 0; i < sz; i++)
        {
            if (__fst::is_end_of_line(s[i])) { return true; }
        }

        return false;
    }

    FST_NODISCARD inline constexpr bool is_unsigned_integer(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        fst_assert(sz != 0);

        for (size_t i = 0; i < sz; i++)
        {
            if (!__fst::is_digit(s[i])) { return false; }
        }

        return true;
    }

    ///
    FST_NODISCARD inline constexpr bool is_integer(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        fst_assert(sz != 0);

        size_t start_index = 0;

        if (s[0] == '-')
        {
            if (sz == 1) { return false; }
            start_index = 1;
        }

        return __fst::is_unsigned_integer(s.substr(start_index));
    }

    FST_NODISCARD inline constexpr bool is_floating_point(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();

        fst_assert(sz != 0);

        size_t start_index = 0;

        if (s[0] == '-')
        {
            if (sz == 1) { return false; }
            start_index = 1;
        }

        // Find point index.
        const __fst::string_view::size_type point_index = s.find_first_of('.');

        // No dot.
        if (point_index == __fst::string_view::npos)
        {
            for (size_t i = start_index; i < sz; i++)
            {
                if (!__fst::is_digit(s[i])) { return false; }
            }

            return true;
        }

        // Has dots.

        // Check if dot is at the beginning or at the end.
        if ((start_index == point_index) || (point_index == sz - 1)) { return false; }

        // Check left side.
        for (size_t i = start_index; i < point_index; i++)
        {
            if (!__fst::is_digit(s[i])) { return false; }
        }

        // Check right side.
        for (size_t i = point_index + 1; i < sz; i++)
        {
            if (!__fst::is_digit(s[i])) { return false; }
        }

        return true;
    }

    FST_NODISCARD inline constexpr bool is_convertible_to_integer(__fst::string_view s) noexcept
    {
        return __fst::is_integer(__fst::trim(s, ' '));
    }

    FST_NODISCARD inline constexpr bool is_convertible_to_unsigned_integer(__fst::string_view s) noexcept
    {
        return __fst::is_unsigned_integer(__fst::trim(s, ' '));
    }

    FST_NODISCARD inline constexpr bool is_convertible_to_floating_point(__fst::string_view s) noexcept
    {
        return __fst::is_floating_point(__fst::trim(s, ' '));
    }



    //
    //
    //
    FST_NODISCARD inline __fst::string_view ltrim(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        for (size_t i = 0; i < sz; i++)
        {
            if (!__fst::is_trim_char(s[i])) { return s.substr(i); }
        }

        return __fst::string_view();
    }

    FST_NODISCARD inline __fst::string_view ltrim(__fst::string_view s, char c_to_trim) noexcept
    {
        const size_t sz = s.size();
        for (size_t i = 0; i < sz; i++)
        {
            if (s[i] != c_to_trim) { return s.substr(i); }
        }

        return __fst::string_view();
    }

    ///
    FST_NODISCARD inline __fst::string_view rtrim(__fst::string_view s) noexcept
    {
        const size_t sz = s.size();
        for (size_t i = 0, k = sz - 1; i < sz; i++, k--)
        {
            if (!is_trim_char(s[k])) { return s.substr(0, k + 1); }
        }

        return __fst::string_view();
    }

    FST_NODISCARD inline __fst::string_view rtrim(__fst::string_view s, char c_to_trim) noexcept
    {
        const size_t sz = s.size();
        for (size_t i = 0, k = sz - 1; i < sz; i++, k--)
        {
            if (s[k] != c_to_trim) { return s.substr(0, k + 1); }
        }

        return __fst::string_view();
    }

    ///
    FST_NODISCARD inline __fst::string_view trim(__fst::string_view s) noexcept
    {
        return __fst::rtrim(__fst::ltrim(s));
    }

    FST_NODISCARD inline __fst::string_view trim(__fst::string_view s, char c_to_trim) noexcept
    {
        return __fst::rtrim(__fst::ltrim(s, c_to_trim), c_to_trim);
    }
FST_END_NAMESPACE
