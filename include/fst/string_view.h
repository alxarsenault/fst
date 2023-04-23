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
#include "fst/memory.h"
#include "fst/utility.h"
#include "fst/sutils.h"

FST_BEGIN_NAMESPACE

    using string_view = basic_string_view<char>;
    using wstring_view = basic_string_view<wchar_t>;
    using u16string_view = basic_string_view<char16_t>;
    using u32string_view = basic_string_view<char32_t>;

#if FST_HAS_CHAR8_T
    using u8string_view = basic_string_view<char8_t>;
#endif // FST_HAS_CHAR8_T

    template <typename _CharT>
    inline constexpr void swap(_FST::basic_string_view<_CharT> & lhs, _FST::basic_string_view<_CharT> & rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <typename _CharT>
    inline constexpr bool operator==(const _FST::basic_string_view<_CharT>& lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    template <typename _CharT>
    inline constexpr bool operator==(_FST::basic_string_view<_CharT> lhs, const _CharT* rhs) noexcept
    {
        return lhs == _FST::basic_string_view<_CharT>(rhs);
    }

    template <typename _CharT>
    inline constexpr bool operator==(const _CharT* lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return _FST::basic_string_view<_CharT>(lhs) == rhs;
    }

    template <typename _CharT>
    inline constexpr bool operator!=(const _FST::basic_string_view<_CharT>& lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    template <typename _CharT>
    inline constexpr bool operator!=(const _FST::basic_string_view<_CharT>& lhs, const _CharT* rhs) noexcept
    {
        return lhs != _FST::basic_string_view<_CharT>(rhs);
    }

    template <typename _CharT>
    inline constexpr bool operator!=(const _CharT* lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return _FST::basic_string_view<_CharT>(lhs) != rhs;
    }

    template <typename _CharT>
    inline constexpr bool operator<(const _FST::basic_string_view<_CharT>& lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    template <typename _CharT>
    inline constexpr bool operator<(const _FST::basic_string_view<_CharT>& lhs, const _CharT* rhs) noexcept
    {
        return lhs < _FST::basic_string_view<_CharT>(rhs);
    }

    template <typename _CharT>
    inline constexpr bool operator<(const _CharT* lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return _FST::basic_string_view<_CharT>(lhs) < rhs;
    }

    template <typename _CharT>
    inline constexpr bool operator>(const _FST::basic_string_view<_CharT>& lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    template <typename _CharT>
    inline constexpr bool operator>(const _FST::basic_string_view<_CharT>& lhs, const _CharT* rhs) noexcept
    {
        return lhs > _FST::basic_string_view<_CharT>(rhs);
    }

    template <typename _CharT>
    inline constexpr bool operator>(const _CharT* lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return _FST::basic_string_view<_CharT>(lhs) > rhs;
    }

    template <typename _CharT>
    inline constexpr bool operator<=(const _FST::basic_string_view<_CharT>& lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    template <typename _CharT>
    inline bool operator<=(const _FST::basic_string_view<_CharT>& lhs, const _CharT* rhs) noexcept
    {
        return lhs <= _FST::basic_string_view<_CharT>(rhs);
    }

    template <typename _CharT>
    inline constexpr bool operator<=(const _CharT* lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return _FST::basic_string_view<_CharT>(lhs) <= rhs;
    }

    template <typename _CharT>
    inline constexpr bool operator>=(const _FST::basic_string_view<_CharT>& lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    template <typename _CharT>
    inline constexpr bool operator>=(const _FST::basic_string_view<_CharT>& lhs, const _CharT* rhs) noexcept
    {
        return lhs >= _FST::basic_string_view<_CharT>(rhs);
    }

    template <typename _CharT>
    inline constexpr bool operator>=(const _CharT* lhs, const _FST::basic_string_view<_CharT>& rhs) noexcept
    {
        return _FST::basic_string_view<_CharT>(lhs) >= rhs;
    }

    template <class _CharT>
    inline _FST::output_stream<_CharT>& operator<<(_FST::output_stream<_CharT>& stream, _FST::basic_string_view<_CharT> str) noexcept
    {
        stream.write(str.data(), str.size());
        return stream;
    }
FST_END_NAMESPACE

#if FST_USE_STL
#include <ostream>
template <class _CharT, class _Traits>
inline std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& stream, _FST::basic_string_view<_CharT> str)
{
    return stream.write(str.data(), str.size());
}
#endif
