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

    template <class _Ty>
    void _Refwrap_ctor_fun(_FST::type_identity_t<_Ty&>) noexcept;
    template <class _Ty>
    void _Refwrap_ctor_fun(_FST::type_identity_t<_Ty&&>) = delete;
    template <class _Ty, class _Uty, class = void>
    struct _Refwrap_has_ctor_from : _FST::false_t
    {};

    template <class _Ty, class _Uty>
    struct _Refwrap_has_ctor_from<_Ty, _Uty, _FST::void_t<decltype(_Refwrap_ctor_fun<_Ty>(_FST::declval<_Uty>()))>> : _FST::true_t
    {};
    //
    template <class _T>
    class reference_wrapper
    {
      public:
        static_assert(_FST::is_object_v<_T> || _FST::is_function_v<_T>, "reference_wrapper<T> requires T to be an object type or a "
                                                                        "function type.");

        using type = _T;
        using value_type = _T;
        using pointer = _T*;
        using reference = _T&;

        template <class _Uty,
            _FST::enable_if_t<_FST::conjunction_v<_FST::negation<_FST::is_same<_FST::remove_cvref_t<_Uty>, reference_wrapper>>, _Refwrap_has_ctor_from<_T, _Uty>>, int> = 0>
        FST_ALWAYS_INLINE constexpr reference_wrapper(_Uty&& v) noexcept
        {
            reference ref = static_cast<_Uty&&>(v);
            _ptr = _FST::addressof(ref);
        }

        FST_ALWAYS_INLINE constexpr reference_wrapper(pointer ptr) noexcept
            : _ptr(ptr)
        {}

        FST_ALWAYS_INLINE constexpr void set(pointer p) noexcept { _ptr = p; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr operator reference() const noexcept { return *_ptr; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference get() const noexcept { return *_ptr; }

        template <class... _Types>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr _FST::invoke_result_t<_T&, _Types&&...> operator()(_Types&&... _Args) const noexcept
        {
            return _FST::invoke(*_ptr, static_cast<_Types&&>(_Args)...);
        }

      private:
        pointer _ptr = nullptr;
    };
FST_END_NAMESPACE
