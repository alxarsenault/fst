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
// set_directory_property(cache_vars CACHE_VARIABLES)
// message(STATUS "List of CACHE variables: ${cache_vars}")
#pragma once
#include <iostream>

/// Clamps a value between a pair of boundary values.
///
/// @param v the value to clamp
/// @param min the left boundary to clamp v to
/// @param max the right boundary to clamp v to
///
/// @return If v compares less than min, returns min, otherwise if max compares less than v,
///         returns max, otherwise returns v.
///
/// @note The behavior is undefined if the value of min is greater than max.
///
/// @note As opposed to std::clamp, all parameters and return type are not passed as reference.
template <typename _T>
FST_NODISCARD FST_ALWAYS_INLINE constexpr _T clamp(_T v, _T _min, _T _max) noexcept
{
    const _T t = v < _min ? _min : v;
    return t > _max ? _max : t;
}


template <typename _T0, typename _T1, typename... _Ts>
FST_NODISCARD FST_ALWAYS_INLINE constexpr __fst::common_type_t<_T0, _T1, _Ts...> minimum(_T0 && v1, _T1 && v2, _Ts && ... vs) noexcept
{
    if constexpr (sizeof...(_Ts) == 0) { return v2 < v1 ? v2 : v1; }
    else { return v2 < v1 ? minimum(__fst::forward<_T1>(v2), __fst::forward<_Ts>(vs)...) : minimum(__fst::forward<_T0>(v1), __fst::forward<_Ts>(vs)...); }
}


/// @struct empty_t
template<class ...>
struct empty_t {};

/// empty_struct
using empty_struct = empty_t<>;


/// void_t
template <class... _Types>
using void_t = void;


/// Get the maximum of n values.
template <typename _T0, typename _T1, typename... _Ts>
FST_NODISCARD FST_ALWAYS_INLINE constexpr __fst::common_type_t<_T0, _T1, _Ts...> maximum(_T0 && v1, _T1 && v2, _Ts && ... vs) noexcept
{
    if constexpr (sizeof...(_Ts) == 0) { return v2 > v1 ? v2 : v1; }
    else {
        return v2 > v1
            ? maximum(__fst::forward<_T1>(v2), __fst::forward<_Ts>(vs)...)
            : maximum(__fst::forward<_T0>(v1), __fst::forward<_Ts>(vs)...); }
}


//nm,,,
/// Get the maximum of n values.
template <typename _T0, typename _T1, typename... _Ts>
FST_NODISCARD FST_ALWAYS_INLINE constexpr __fst::common_type_t<_T0, _T1, _Ts...> maximum(_T0 && v1, _T1 && v2, _Ts && ... vs) noexcept
{
    if constexpr (sizeof...(_Ts) == 0) { return v2 > v1 ? v2 : v1; }
    else {
        return v2 > v1
            ? maximum(__fst::forward<_T1>(v2), __fst::forward<_Ts>(vs)...)
            : maximum(__fst::forward<_T0>(v1), __fst::forward<_Ts>(vs)...); }
}
