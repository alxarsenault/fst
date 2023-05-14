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
#include "fst/iterator.h"
#include "fst/math.h"

FST_BEGIN_NAMESPACE

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
    FST_ALWAYS_INLINE constexpr _T clamp(_T v, _T min, _T max) noexcept
    {
        const _T t = v < min ? min : v;
        return t > max ? max : t;
    }

    /// Clamps a value (inplace) between a pair of boundary values.
    ///
    /// @param v the value to clamp
    /// @param min the left boundary to clamp v to
    /// @param max the right boundary to clamp v to
    ///
    /// @note The behavior is undefined if the value of min is greater than max.
    template <typename _T>
    FST_ALWAYS_INLINE constexpr void clamp_inplace(_T & v, _T min, _T max) noexcept
    {
        v = __fst::clamp(v, min, max);
    }

    ///
    template <typename _T>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool clamp_inplace_r(_T & v, _T min, _T max) noexcept
    {
        _T old_v = v;
        const _T t = v < min ? min : v;
        return old_v != (v = (t > max ? max : t));
    }

    ///
    template <typename _T>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr _T clamp_to(_T v, _T in_min, _T in_max, _T min_if_below_min, _T max_if_above_max) noexcept
    {
        const _T t = v < in_min ? min_if_below_min : v;
        return t > in_max ? max_if_above_max : t;
    }

    /// Get the maximum of n values.
    /// @code
    ///   int a = fst::maximum(1, 2, 3, 4, 5);
    /// @endcode
    template <typename _T0, typename _T1, typename... _Ts>
    inline constexpr __fst::common_type_t<_T0, _T1, _Ts...> maximum(_T0 && v1, _T1 && v2, _Ts && ... vs) noexcept
    {
        if constexpr (sizeof...(_Ts) == 0) { return v2 > v1 ? v2 : v1; }
        else { return v2 > v1 ? maximum(__fst::forward<_T1>(v2), __fst::forward<_Ts>(vs)...) : maximum(__fst::forward<_T0>(v1), __fst::forward<_Ts>(vs)...); }
    }

    ///
    template <typename _T0, typename _T1, typename... _Ts>
    inline constexpr __fst::common_type_t<_T0, _T1, _Ts...> minimum(_T0 && v1, _T1 && v2, _Ts && ... vs) noexcept
    {
        if constexpr (sizeof...(_Ts) == 0) { return v2 < v1 ? v2 : v1; }
        else { return v2 < v1 ? minimum(__fst::forward<_T1>(v2), __fst::forward<_Ts>(vs)...) : minimum(__fst::forward<_T0>(v1), __fst::forward<_Ts>(vs)...); }
    }

    ///
    template <typename _T, typename _T1, typename... _Ts>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_one_of(_T t, _T1 t1, _Ts... ts) noexcept
    {
        if constexpr (sizeof...(_Ts) == 0) { return t == t1; }
        else { return (t == t1) || __fst::is_one_of(t, ts...); }
    }

    ///
    template <typename _T, typename _T1, typename... _Ts>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool all_equals(_T t, _T1 t1, _Ts... ts) noexcept
    {
        if constexpr (sizeof...(_Ts) == 0) { return t == t1; }
        else { return (t == t1) && __fst::all_equals(t, ts...); }
    }

    /// Boolean only one true.
    template <class... _Ts>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_only_one_true(_Ts... ts) noexcept
    {
        return (ts ^ ...);
    }

    /// Boolean only one false.
    template <class... _Ts>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_only_one_false(_Ts... ts) noexcept
    {
        return (!ts ^ ...);
    }

    ///
    template <typename _T>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_in_range(_T x, _T left, _T right) noexcept
    {
        return x >= left && x <= right;
    }

    ///
    template <typename _T>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_out_of_range(_T x, _T left, _T right) noexcept
    {
        return __fst::is_in_range(x, left, right);
    }

    template <class _It1, class _It2>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_equal_range(_It1 first1, _It1 last1, _It2 first2)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (!(*first1 == *first2)) { return false; }
        }

        return true;
    }

    template <class _It1, class _It2>
    bool lexicographical_compare(_It1 first1, _It1 last1, _It2 first2, _It2 last2)
    {
        for (; (first1 != last1) && (first2 != last2); ++first1, (void) ++first2)
        {
            if (*first1 < *first2) return true;
            if (*first2 < *first1) return false;
        }

        return (first1 == last1) && (first2 != last2);
    }

    template <typename T1, typename T2, __fst::enable_if_t<__fst::is_floating_point_v<__fst::common_type_t<T1, T2>>, int> = 0>
    FST_NODISCARD inline constexpr bool fcompare(T1 a, T2 b) noexcept
    {
        using ftype = __fst::common_type_t<T1, T2>;
        const ftype fa = static_cast<ftype>(a);
        const ftype fb = static_cast<ftype>(b);
        const ftype t = static_cast<ftype>(__fst::numeric_limits<ftype>::epsilon());
        const ftype dt = __fst::fabs(fa - fb);
        return dt <= t || dt < __fst::maximum(__fst::fabs(fa), __fst::fabs(fb)) * t;

        //  using T = __fst::common_type_t<T1, T2>;
        //  return __fst::abs(static_cast<T>(a) - static_cast<T>(b)) <= __fst::numeric_limits<T>::epsilon();
    }

    template <class _T = void>
    struct less
    {
        FST_NODISCARD inline constexpr bool operator()(__fst::cref_t<_T> left, __fst::cref_t<_T> right) const noexcept { return left < right; }
    };

    template <>
    struct less<void>
    {
        template <class _T1, class _T2>
        FST_NODISCARD constexpr auto operator()(_T1&& left, _T2&& right) const noexcept
        {
            return static_cast<_T1&&>(left) < static_cast<_T2&&>(right);
        }

        using is_transparent = int;
    };

    /*template <class _T, class _Comp>
    inline void sort(_T * data, size_t size, _Comp cmp) noexcept
    {
        ::qsort_s(
            data, size, sizeof(_T),
            [](void* c, const void* a, const void* b)
            {
                _Comp& cmp = *(_Comp*) c;
                return cmp(*(const _T*) a, *(const _T*) b) ? -1 : cmp(*(const _T*) b, *(const _T*) a) ? 1 : 0;
            },
            &cmp);
    }

    template <class _T>
    inline void sort(_T * data, size_t size) noexcept
    {
        ::qsort(data, size, sizeof(_T), [](const void* a, const void* b) { return (*(const _T*) a == *(const _T*) b) ? 0 : (*(const _T*) a < *(const _T*) b) ? -1 : 1; });
    }
    template <class _Iterator, __fst::enable_if_t<__fst::is_contiguous_iterator<_Iterator>::value, int> = 0>
    inline void sort(_Iterator first, _Iterator last) noexcept
    {
        using value_type = typename __fst::iterator_traits<_Iterator>::value_type;
        ::qsort(first, __fst::distance(first, last), sizeof(value_type),
            [](const void* a, const void* b) {
                return (*(const value_type*) a == *(const value_type*) b) ? 0 : (*(const value_type*) a < *(const value_type*) b) ? -1 : 1;
            });
    }

    template <class _Iterator, class _Comp, __fst::enable_if_t<__fst::is_contiguous_iterator<_Iterator>::value, int> = 0>
    inline void sort(_Iterator first, _Iterator last, _Comp cmp) noexcept
    {
        using value_type = typename __fst::iterator_traits<_Iterator>::value_type;
        __fst::sort(first, __fst::distance(first, last), cmp);
    }

    template <class _T, __fst::enable_if_t<__fst::is_container_v<_T>, int> = 0>
    inline void sort(_T & range) noexcept
    {
        __fst::sort(range.data(), range.size());
    }

    template <class _T, class _Comp, __fst::enable_if_t<__fst::is_container_v<_T>, int> = 0>
    inline void sort(_T & range, _Comp cmp) noexcept
    {
        __fst::sort(range.data(), range.size(), cmp);
    }*/

    //
    // unroll
    //

    template <size_t N, size_t _Mul = 1>
    struct unroller;

#define FOP(N) fct.template operator()<_Mul * N>()
#define FOP0() \
    FOP(0);    \
    FOP(1);    \
    FOP(2);    \
    FOP(3);    \
    FOP(4);    \
    FOP(5);    \
    FOP(6);    \
    FOP(7);    \
    FOP(8);    \
    FOP(9)

#define FOPB(B, N) fct.template operator()<_Mul * FST_CONCAT(B, N)>()
#define FOPL(B) \
    FOPB(B, 0); \
    FOPB(B, 1); \
    FOPB(B, 2); \
    FOPB(B, 3); \
    FOPB(B, 4); \
    FOPB(B, 5); \
    FOPB(B, 6); \
    FOPB(B, 7); \
    FOPB(B, 8); \
    FOPB(B, 9)

    template <size_t _Mul>
    struct unroller<1, _Mul>
    {
        static constexpr size_t size = 1;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP(0);
        }
    };

    template <size_t _Mul>
    struct unroller<2, _Mul>
    {
        static constexpr size_t size = 2;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP(0);
            FOP(1);
        }
    };

    template <size_t _Mul>
    struct unroller<4, _Mul>
    {
        static constexpr size_t size = 4;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP(0);
            FOP(1);
            FOP(2);
            FOP(3);
        }
    };

    template <size_t _Mul>
    struct unroller<8, _Mul>
    {
        static constexpr size_t size = 8;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP(0);
            FOP(1);
            FOP(2);
            FOP(3);
            FOP(4);
            FOP(5);
            FOP(6);
            FOP(7);
        }
    };

    template <size_t _Mul>
    struct unroller<16, _Mul>
    {
        static constexpr size_t size = 16;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP0();
            FOP(10);
            FOP(11);
            FOP(12);
            FOP(13);
            FOP(14);
            FOP(15);
        }
    };

    template <size_t _Mul>
    struct unroller<32, _Mul>
    {
        static constexpr size_t size = 32;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP0();
            FOPL(1);
            FOPL(2);
            FOP(30);
            FOP(31);
        }
    };

    template <size_t _Mul>
    struct unroller<64, _Mul>
    {
        static constexpr size_t size = 64;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP0();
            FOPL(1);
            FOPL(2);
            FOPL(3);
            FOPL(4);
            FOPL(5);
            FOP(60);
            FOP(61);
            FOP(62);
            FOP(63);
        }
    };

    template <size_t _Mul>
    struct unroller<128, _Mul>
    {
        static constexpr size_t size = 128;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP0();
            FOPL(1);
            FOPL(2);
            FOPL(3);
            FOPL(4);
            FOPL(5);
            FOPL(6);
            FOPL(7);
            FOPL(8);
            FOPL(9);
            FOPL(10);
            FOPL(11);
            FOP(120);
            FOP(121);
            FOP(122);
            FOP(123);
            FOP(124);
            FOP(125);
            FOP(126);
            FOP(127);
        }
    };

    template <size_t _Mul>
    struct unroller<256, _Mul>
    {
        static constexpr size_t size = 256;
        static constexpr size_t mul = _Mul;

        template <class _Fct>
        FST_ALWAYS_INLINE static constexpr void unroll(_Fct&& fct) noexcept
        {
            FOP0();
            FOPL(1);
            FOPL(2);
            FOPL(3);
            FOPL(4);
            FOPL(5);
            FOPL(6);
            FOPL(7);
            FOPL(8);
            FOPL(9);
            FOPL(10);
            FOPL(11);
            FOPL(12);
            FOPL(13);
            FOPL(14);
            FOPL(15);
            FOPL(16);
            FOPL(17);
            FOPL(18);
            FOPL(19);
            FOPL(20);
            FOPL(21);
            FOPL(22);
            FOPL(23);
            FOPL(24);
            FOP(250);
            FOP(251);
            FOP(252);
            FOP(253);
            FOP(254);
            FOP(255);
        }
    };

    template <size_t N, class _Fct>
    FST_ALWAYS_INLINE constexpr void unroll(_Fct && fct) noexcept
    {
        unroller<N>::unroll(__fst::forward<_Fct>(fct));
    }

    template <size_t N, size_t _Mul, class _Fct>
    FST_ALWAYS_INLINE constexpr void unroll(_Fct && fct) noexcept
    {
        unroller<N, _Mul>::unroll(__fst::forward<_Fct>(fct));
    }

    //
    // Hash
    //

// These FNV-1a utility functions are extremely performance sensitive,
// check examples like that in VSO-653642 before making changes.
#if defined(_WIN64)
    FST_INLINE_VAR constexpr size_t _FNV_offset_basis = 14695981039346656037ULL;
    FST_INLINE_VAR constexpr size_t _FNV_prime = 1099511628211ULL;
#else // defined(_WIN64)
    FST_INLINE_VAR constexpr size_t _FNV_offset_basis = 2166136261U;
    FST_INLINE_VAR constexpr size_t _FNV_prime = 16777619U;
#endif // defined(_WIN64)

    FST_NODISCARD inline size_t _Fnv1a_append_bytes(size_t _Val, const unsigned char* const _First, const size_t _Count) noexcept
    {
        for (size_t _Idx = 0; _Idx < _Count; ++_Idx)
        {
            _Val ^= static_cast<size_t>(_First[_Idx]);
            _Val *= _FNV_prime;
        }

        return _Val;
    }

    template <class _Ty>
    FST_NODISCARD size_t _Fnv1a_append_range(const size_t _Val, const _Ty* const _First, const _Ty* const _Last) noexcept
    {
        static_assert(__fst::is_trivial_v<_Ty>, "Only trivial types can be directly hashed.");
        const auto _Firstb = reinterpret_cast<const unsigned char*>(_First);
        const auto _Lastb = reinterpret_cast<const unsigned char*>(_Last);
        return __fst::_Fnv1a_append_bytes(_Val, _Firstb, static_cast<size_t>(_Lastb - _Firstb));
    }

    template <class _Kty>
    FST_NODISCARD size_t _Fnv1a_append_value(const size_t _Val, const _Kty& _Keyval) noexcept
    {
        static_assert(__fst::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
        return __fst::_Fnv1a_append_bytes(_Val, &reinterpret_cast<const unsigned char&>(_Keyval), sizeof(_Kty));
    }

    template <class _Kty>
    FST_NODISCARD size_t _Hash_representation(const _Kty& _Keyval) noexcept
    {
        return __fst::_Fnv1a_append_value(_FNV_offset_basis, _Keyval);
    }

    template <class _Kty>
    FST_NODISCARD size_t _Hash_array_representation(const _Kty* const _First, const size_t _Count) noexcept
    {
        static_assert(__fst::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
        return _Fnv1a_append_bytes(__fst::_FNV_offset_basis, reinterpret_cast<const unsigned char*>(_First), _Count * sizeof(_Kty));
    }

    template <class _Kty>
    struct hash;

    template <class _Kty, bool _Enabled>
    struct _Conditionally_enabled_hash
    {
        FST_NODISCARD size_t operator()(const _Kty& _Keyval) const noexcept(noexcept(hash<_Kty>::_Do_hash(_Keyval))) { return hash<_Kty>::_Do_hash(_Keyval); }
    };

    template <class _Kty>
    struct _Conditionally_enabled_hash<_Kty, false>
    {
        _Conditionally_enabled_hash() = delete;
        _Conditionally_enabled_hash(const _Conditionally_enabled_hash&) = delete;
        _Conditionally_enabled_hash(_Conditionally_enabled_hash&&) = delete;
        _Conditionally_enabled_hash& operator=(const _Conditionally_enabled_hash&) = delete;
        _Conditionally_enabled_hash& operator=(_Conditionally_enabled_hash&&) = delete;
    };

    template <class _Kty>
    struct hash : _Conditionally_enabled_hash<_Kty, !__fst::is_const_v<_Kty> && (__fst::is_enum_v<_Kty> || __fst::is_integral_v<_Kty> || __fst::is_pointer_v<_Kty>)>
    {
        // hash functor primary template (handles enums, integrals, and pointers)
        static size_t _Do_hash(const _Kty& _Keyval) noexcept { return __fst::_Hash_representation(_Keyval); }
    };

    template <>
    struct hash<float>
    {
        FST_NODISCARD size_t operator()(const float _Keyval) const noexcept
        {
            return __fst::_Hash_representation(_Keyval == 0.0f ? 0.0f : _Keyval); // map -0 to 0
        }
    };

    template <>
    struct hash<double>
    {
        FST_NODISCARD size_t operator()(const double _Keyval) const noexcept { return __fst::_Hash_representation(_Keyval == 0.0 ? 0.0 : _Keyval); }
    };

    template <>
    struct hash<__fst::nullptr_t>
    {
        FST_NODISCARD size_t operator()(__fst::nullptr_t) const noexcept
        {
            void* _Null{};
            return __fst::_Hash_representation(_Null);
        }
    };

    template <typename _Fct>
    class final_action
    {
      public:
        using function_type = _Fct;
        static_assert(!__fst::is_reference<function_type>::value //
                          && !__fst::is_const<function_type>::value //
                          && !__fst::is_volatile<function_type>::value, //
            "final_action should store its callable by value");

        inline final_action(function_type&& f) noexcept
            : _fct(__fst::move(f))
        {}

        inline final_action(const function_type& f)
            : _fct(f)
        {}

        inline final_action(final_action&& other) noexcept
            : _fct(__fst::move(other._fct))
            , _need_invoke(__fst::exchange(other._need_invoke, false))
        {}

        final_action(const final_action&) = delete;
        final_action& operator=(const final_action&) = delete;
        final_action& operator=(final_action&&) = delete;

        inline ~final_action() noexcept
        {
            if (_need_invoke) { _fct(); }
        }

        inline void cancel()
        {
            _fct = function_type();
            _need_invoke = false;
        }

      private:
        function_type _fct;
        bool _need_invoke = true;
    };

    template <class _Fct>
    final_action(_Fct) -> final_action<_Fct>;
    /*if (const char* begin = ::strchr(header_str.data(), '\n'))
            {
                const char* end = header_str.cend();

                while (++begin < end)
                {
                    const char* name_split = ::strchr(begin, ':');
                    if (!name_split) { return; }

                    const char* line_end = ::strchr(name_split, '\n');
                    if (!line_end) { return; }

                    headers.push_back({ string_type(begin, name_split - begin), string_type(name_split + 2, line_end - name_split - 3) });
                    begin = line_end;
                }
            }*/

FST_END_NAMESPACE
