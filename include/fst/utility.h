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
#include <string.h>
#include <ctype.h>
#include <wchar.h>

#if __FST_MSVC__ && __FST_INTEL__
#include <intrin.h>
#endif

FST_BEGIN_NAMESPACE

    template <typename T>
    FST_NODISCARD FST_ALWAYS_INLINE constexpr T clamp(T d, T min, T max) noexcept
    {
        const T t = d < min ? min : d;
        return t > max ? max : t;
    }

    /// Get the maximum of n values.
    /// @code
    ///   int a = mts::maximum(1, 2, 3, 4, 5);
    /// @endcode
    template <typename T0, typename T1, typename... Ts>
    inline constexpr __fst::common_type_t<T0, T1, Ts...> maximum(T0 && v1, T1 && v2, Ts && ... vs)
    {
        if constexpr (sizeof...(Ts) == 0) { return v2 > v1 ? v2 : v1; }
        else { return v2 > v1 ? maximum(__fst::forward<T1>(v2), __fst::forward<Ts>(vs)...) : maximum(__fst::forward<T0>(v1), __fst::forward<Ts>(vs)...); }
    }

    template <typename T0, typename T1, typename... Ts>
    inline constexpr __fst::common_type_t<T0, T1, Ts...> minimum(T0 && v1, T1 && v2, Ts && ... vs)
    {
        if constexpr (sizeof...(Ts) == 0) { return v2 < v1 ? v2 : v1; }
        else { return v2 < v1 ? minimum(__fst::forward<T1>(v2), __fst::forward<Ts>(vs)...) : minimum(__fst::forward<T0>(v1), __fst::forward<Ts>(vs)...); }
    }

    template <typename T, typename T1>
    FST_NODISCARD inline constexpr bool is_one_of(T t, T1 t1) noexcept
    {
        return t == t1;
    }

    template <typename T, typename T1, typename... Ts>
    FST_NODISCARD inline constexpr bool is_one_of(T t, T1 t1, Ts... ts) noexcept
    {
        return (t == t1) || __fst::is_one_of(t, ts...);
    }

    ///
    FST_NODISCARD inline bool is_aligned(const void* ptr, uintptr_t alignment) noexcept
    {
        return !(uintptr_t(ptr) & (alignment - 1));
    }

    ///
    template <class T>
    FST_NODISCARD inline bool is_aligned(const void* ptr) noexcept
    {
        return is_aligned(ptr, alignof(T));
    }

    ///
    FST_NODISCARD inline uint32_t get_alignment(const void* ptr) noexcept
    {
        const uintptr_t paddr = (uintptr_t) ptr;
        uintptr_t exp = 64;
        while (paddr & (exp - 1))
        {
            exp >>= 1;
        }
        return (uint32_t) exp;
    }

    FST_NODISCARD FST_ALWAYS_INLINE size_t align(size_t cursor, size_t alignment) noexcept
    {
        return (cursor + (alignment - 1)) & ~(alignment - 1);
    }

    FST_NODISCARD FST_ALWAYS_INLINE void* align(size_t alignment, size_t size, void*& ptr, size_t& avail_space) noexcept
    {
        fst_assert(avail_space >= size);

        if (size_t offset = static_cast<size_t>(reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)))
        {
            offset = alignment - offset;
            if (avail_space < offset || avail_space - offset < size) { return nullptr; }

            avail_space -= offset;
            return ptr = static_cast<char*>(ptr) + offset;
        }

        return ptr = static_cast<char*>(ptr);
    }

    template <class... Ts>
    struct max_align_size_s;

    template <class T>
    struct max_align_size_s<T>
    {
        static constexpr size_t size = alignof(T);
    };

    template <class T, class... Ts>
    struct max_align_size_s<T, Ts...>
    {
        static constexpr size_t size = __fst::maximum(alignof(T), max_align_size_s<Ts...>::size);
    };

    template <class... Ts>
    FST_NODISCARD inline constexpr size_t max_align_size()
    {
        return max_align_size_s<Ts...>::size;
    }

    // Endianness
    struct little_endian_tag
    {};
    struct big_endian_tag
    {};

    template <class T>
    struct is_endian_tag : __fst::bool_t<__fst::is_same_v<T, little_endian_tag> || __fst::is_same_v<T, big_endian_tag>>
    {};

    template <class T>
    FST_INLINE_VAR constexpr bool is_endian_tag_v = is_endian_tag<T>::value;

    ///
    FST_ALWAYS_INLINE void memcpy(void* dst, const void* src, size_t size) noexcept
    {
#if __FST_CLANG__
        __builtin_memcpy(dst, src, size);
#else

#if __FST_MSVC__ && __FST_INTEL__
#pragma intrinsic(memcpy)
#endif

        ::memcpy(dst, src, size);

#if __FST_MSVC__ && __FST_INTEL__
#pragma function(memcpy)
#endif

#endif
    }

    template <class T>
    FST_ALWAYS_INLINE void mem_copy(T * dst, const T* src, size_t size) noexcept
    {
        if constexpr (__fst::is_trivially_copyable_v<T>) { __fst::memcpy(dst, src, size * sizeof(T)); }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                dst[i] = src[i];
            }
        }
    }

    template <size_t _Alignment, class T>
    FST_ALWAYS_INLINE void mem_copy(T * dst, const T* src, size_t size) noexcept
    {
        fst_assert(__fst::is_aligned(dst, _Alignment));
        fst_assert(__fst::is_aligned(src, _Alignment));

        if constexpr (__fst::is_trivially_copyable_v<T>) { __fst::memcpy(dst, src, size * sizeof(T)); }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                dst[i] = src[i];
            }
        }
    }

    ///

    FST_ALWAYS_INLINE void memmove(void* dst, const void* src, size_t size) noexcept
    {
#if __FST_CLANG__
        __builtin_memmove(dst, src, size);
#else

#if __FST_MSVC__ && __FST_INTEL__
#pragma intrinsic(memmove)
#endif

        ::memmove(dst, src, size);

#if __FST_MSVC__ && __FST_INTEL__
#pragma function(memmove)
#endif
#endif
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE int memcmp(const void* lhs, const void* rhs, size_t size) noexcept
    {
#if __FST_MSVC__ && __FST_INTEL__
#pragma intrinsic(memcmp)
#endif

        return ::memcmp(lhs, rhs, size);

#if __FST_MSVC__ && __FST_INTEL__
#pragma function(memcmp)
#endif
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t strlen(const char* str) noexcept
    {
#if __FST_MSVC__ || __FST_CLANG__
        return __builtin_strlen(str);
#else
        return ::strlen(str);
#endif
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

    ///
    FST_ALWAYS_INLINE void memset(void* dst, int value, size_t size) noexcept
    {

#if __FST_MSVC__ && __FST_INTEL__
#pragma intrinsic(memset)
#endif

        ::memset(dst, value, size);

#if __FST_MSVC__ && __FST_INTEL__
#pragma function(memset)
#endif
    }

    template <class T>
    FST_ALWAYS_INLINE void memfill(T * dst, __fst::cref_t<T> value, size_t size) noexcept
    {
        for (size_t i = 0; i < size; i++)
        {
            dst[i] = value;
        }
    }

    template <size_t _Alignment, class T>
    FST_ALWAYS_INLINE void memfill(T * dst, __fst::cref_t<T> value, size_t size) noexcept
    {
        fst_assert(__fst::is_aligned(dst, _Alignment));
        for (size_t i = 0; i < size; i++)
        {
            dst[i] = value;
        }
    }

    FST_ALWAYS_INLINE void memzero(void* dst, size_t size) noexcept
    {
        __fst::memset(dst, 0, size);
    }

    template <class T>
    FST_ALWAYS_INLINE void mem_zero(T * dst, size_t size) noexcept
    {
        __fst::memset(dst, 0, size * sizeof(T));
    }

    template <size_t _Alignment, class T>
    FST_ALWAYS_INLINE void mem_zero(T * dst, size_t size) noexcept
    {
        fst_assert(__fst::is_aligned(dst, _Alignment));
        __fst::memset(dst, 0, size * sizeof(T));
    }

    template <class _T>
    FST_ALWAYS_INLINE void copy_element(_T & dst, __fst::cref_t<_T> src) noexcept
    {
        if constexpr (__fst::is_trivial_cref_v<_T>) { dst = src; }
        else { __fst::mem_copy(&dst, &src, 1); }
    }

    template <class _T>
    FST_ALWAYS_INLINE void move_element(_T & dst, _T && src) noexcept
    {
        if constexpr (__fst::is_trivial_cref_v<_T>) { dst = src; }
        else { __fst::mem_copy(&dst, &src, 1); }
    }

    ////
    //template <class T>
    //inline constexpr __fst::pair<const T&, const T&> minmax(const T& a, const T& b)
    //{
    //    return (b < a) ? __fst::pair<const T&, const T&>(b, a) : __fst::pair<const T&, const T&>(a, b);
    //}

    template <class _Iterator, __fst::enable_if_t<__fst::is_random_access_iterator<_Iterator>::value, int> = 0>
    constexpr ptrdiff_t distance(_Iterator first, _Iterator last)
    {
        return last - first;
    }

    template <class _Iterator, __fst::enable_if_t<__fst::is_random_access_iterator<_Iterator>::value, int> = 0>
    constexpr size_t pdistance(_Iterator first, _Iterator last)
    {
        ptrdiff_t diff = __fst::distance(first, last);
        fst_assert(diff >= 0);
        return static_cast<size_t>(diff);
    }

    template <class _SrcIterator, class _DstIterator>
    inline constexpr _DstIterator swap_ranges(const _SrcIterator src_first, const _SrcIterator src_last, _DstIterator dst)
    {
        const size_t size = src_last - src_first;
        for (size_t i = 0; i < size; i++)
        {
            __fst::memswap(dst[i], src_first[i]);
        }

        return dst;
    }

    template <class _SrcIterator, class _DstIterator,
        __fst::enable_if_t<__fst::is_contiguous_iterator<_DstIterator>::value && __fst::is_contiguous_iterator<_SrcIterator>::value, int> = 0>
    _DstIterator copy_backward(_SrcIterator first, _SrcIterator last, _DstIterator d_last)
    {
        while (first != last)
            *(--d_last) = *(--last);

        return d_last;
    }

    template <class _SrcIterator, class _DstIterator,
        __fst::enable_if_t<__fst::is_contiguous_iterator<_DstIterator>::value && __fst::is_contiguous_iterator<_SrcIterator>::value, int> = 0>
    _DstIterator move_backward(_SrcIterator first, _SrcIterator last, _DstIterator d_last)
    {
        while (first != last)
            *(--d_last) = __fst::move(*(--last));

        return d_last;
    }

    template <class _SrcIterator, class _DstIterator,
        __fst::enable_if_t<__fst::is_contiguous_iterator<_DstIterator>::value && __fst::is_contiguous_iterator<_SrcIterator>::value, int> = 0>
    inline constexpr _DstIterator copy(_SrcIterator first, _SrcIterator last, _DstIterator dst_first) noexcept
    {
        for (; first != last; (void) ++first, (void) ++dst_first)
            *dst_first = *first;

        return dst_first;
    }

    template <class _SrcIterator, class _DstIterator,
        __fst::enable_if_t<__fst::is_contiguous_iterator<_DstIterator>::value && __fst::is_contiguous_iterator<_SrcIterator>::value, int> = 0>
    _DstIterator move(_SrcIterator first, _SrcIterator last, _DstIterator dst_first)
    {
        for (; first != last; ++dst_first, ++first)
            *dst_first = __fst::move(*first);

        return dst_first;
    }

    template <class _It1, class _It2>
    constexpr bool equal(_It1 first1, _It1 last1, _It2 first2)
    {
        for (; first1 != last1; ++first1, ++first2)
            if (!(*first1 == *first2)) return false;

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

    template <class _T>
    inline void relocate(_T * dst, _T * src, size_t size) noexcept
    {
        if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::memcpy(dst, src, size * sizeof(_T)); }
        else if constexpr (__fst::is_trivially_destructible_v<_T>)
        {
            for (size_t i = 0; i < size; i++)
            {
                fst_placement_new(dst + i) _T(__fst::move(src[i]));
            }
        }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                fst_placement_new(dst + i) _T(__fst::move(src[i]));
                src[i].~_T();
            }
        }
    }

    template <class _T>
    inline void default_construct_range(_T * dst, size_t size) noexcept
    {
        if constexpr (!__fst::is_trivially_default_constructible_v<_T>)
        {
            for (size_t i = 0; i < size; i++)
            {
                fst_placement_new(dst + i) _T();
            }
        }
        else { __fst::unused(dst, size); }
    }

    template <class _T>
    inline void copy_construct_range(_T * dst, const _T* src, size_t size) noexcept
    {
        if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::memcpy(dst, src, size * sizeof(_T)); }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                fst_placement_new(dst + i) _T(src[i]);
            }
        }
    }

    template <class _T>
    inline void copy_range(_T * dst, const _T* src, size_t size) noexcept
    {
        if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::memcpy(dst, src, size * sizeof(_T)); }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                dst[i] = src[i];
            }
        }
    }

    template <class _T>
    inline void move_construct_range(_T * dst, _T * src, size_t size) noexcept
    {
        if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::memcpy(dst, src, size * sizeof(_T)); }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                fst_placement_new(dst + i) _T(__fst::move(src[i]));
            }
        }
    }

    template <class _T>
    inline void move_range(_T * dst, _T * src, size_t size) noexcept
    {
        if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::memcpy(dst, src, size * sizeof(_T)); }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                fst_placement_new(dst + i) _T(__fst::move(src[i]));
            }
        }
    }

    template <class _T>
    FST_ALWAYS_INLINE void destruct_range(FST_ATTRIBUTE_UNUSED _T * dst, FST_ATTRIBUTE_UNUSED size_t size) noexcept
    {
        if constexpr (!__fst::is_trivially_destructible_v<_T>)
        {
            for (size_t i = 0; i < size; i++)
            {
                dst[i].~_T();
            }
        }

        else { __fst::unused(dst, size); }
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

    ///
    template <class T>
    FST_NODISCARD inline constexpr T byte_swap(T value) noexcept
    {
        static_assert(__fst::is_trivially_copyable_v<T>, "fst::byte_swap T must be trivially copyable");

        constexpr size_t end_offset = sizeof(T) - 1;
        uint8_t* data = (uint8_t*) &value;
        for (uint8_t *begin = data, *end = data + end_offset; begin < end;)
        {
            __fst::memswap(*begin++, *end--);
        }

        return *(T*) data;
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
        static_assert(!__fst::is_reference<function_type>::value && !__fst::is_const<function_type>::value && !__fst::is_volatile<function_type>::value, "final_action "
                                                                                                                                                         "should store "
                                                                                                                                                         "its "
                                                                                                                                                         "callable by "
                                                                                                                                                         "value");

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

    template <typename IteratorT>
    class iterator_range
    {
        IteratorT _begin_iterator;
        IteratorT _end_iterator;

      public:
        template <typename Container>
        inline iterator_range(Container&& c)
            : _begin_iterator(c.begin())
            , _end_iterator(c.end())
        {}

        inline iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
            : _begin_iterator(__fst::move(begin_iterator))
            , _end_iterator(__fst::move(end_iterator))
        {}

        inline IteratorT begin() const { return _begin_iterator; }
        inline IteratorT end() const { return _end_iterator; }
        inline bool empty() const { return _begin_iterator == _end_iterator; }
    };

FST_END_NAMESPACE
