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

///
/// @file fst/memory_utils.h
/// @fst core
/// @author Alexandre Arsenault (alx.arsenault@gmail.com)
/// @date 2023
///

#include "fst/common.h"
#include "fst/traits.h"
#include "fst/iterator.h"
#include "fst/utility.h"

#include <string.h>

#if __FST_MSVC__ && __FST_INTEL__
#include <intrin.h>
#endif

FST_BEGIN_NAMESPACE

    /// @name cpu
    /// CPU info
    /// @{

    /// Get the cpu cache size (in bytes).
    size_t mem_cache_size() noexcept;

    /// Get the memory page size (in bytes).
    size_t mem_page_size() noexcept;

    /// @}

    ///
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

    ///
    template <class... Ts>
    FST_NODISCARD inline constexpr size_t max_align_size()
    {
        return max_align_size_s<Ts...>::size;
    }

    /// Informs the implementation that the object ptr points to is aligned to at least _Alignment.
    /// The implementation may use this information to generate more efficient code, but it might
    /// only make this assumption if the object is accessed via the return value of assume_aligned.
    ///
    /// The program is ill-formed if _Alignment is not a power of 2.
    /// The behavior is undefined if ptr does not point to an object of type _T
    /// (ignoring cv-qualification at every level), or if the object's alignment is not at least _Alignment.
    template <size_t _Alignment, class _T>
    FST_NODISCARD constexpr _T* assume_aligned(_T * ptr) noexcept
    {
        if (__fst::is_constant_evaluated()) { return ptr; }

#if __has_builtin(__builtin_assume_aligned) || __FST_MSVC__
        return static_cast<_T*>(__builtin_assume_aligned(ptr, _Alignment));
#else
        return ptr;
#endif
    }

    ///
    template <class _T>
    FST_NODISCARD constexpr _T* launder(_T * ptr) noexcept
    {
#if __has_builtin(__builtin_launder) || __FST_MSVC__
        return __builtin_launder(ptr);
#else
        return ptr;
#endif
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE bool is_aligned(const void* ptr, uintptr_t alignment) noexcept
    {
        return !(uintptr_t(ptr) & (alignment - 1));
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_aligned(uintptr_t addr, size_t alignment) noexcept
    {
        return !(addr & (alignment - 1));
    }

    ///
    template <class T>
    FST_NODISCARD FST_ALWAYS_INLINE bool is_type_aligned(const void* ptr) noexcept
    {
        return is_aligned(ptr, alignof(T));
    }

    ///
    FST_ALWAYS_INLINE constexpr size_t clip_alignment(size_t alignment) noexcept
    {
        fst_assert(__fst::is_power_of_two(alignment), "alignment must be a power of two");
        if (alignment < default_alignment) { alignment = default_alignment; }
        fst_assert(__fst::is_multiple_of<__fst::platform_alignment>(alignment), "alignment must be a multiple of fst::platform_alignment");

        return alignment;
    }

    ///
    FST_ALWAYS_INLINE constexpr size_t clip_vectorized_alignment(size_t alignment) noexcept
    {
        fst_assert(__fst::is_power_of_two(alignment), "alignment must be a power of two");
        if (alignment < default_vectorized_alignment) { alignment = default_vectorized_alignment; }
        fst_assert(__fst::is_multiple_of<__fst::platform_alignment>(alignment), "alignment must be a multiple of fst::platform_alignment");
        return alignment;
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t align(uintptr_t ptr, size_t alignment) noexcept
    {
        return (ptr + (alignment - 1)) & ~(alignment - 1);
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE char* align(char* ptr, size_t alignment) noexcept
    {
        return (char*) (align((uintptr_t) ptr, alignment));
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE void* align(void* ptr, size_t alignment) noexcept
    {
        return (void*) (align((uintptr_t) ptr, alignment));
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE char* align_range(char* begin, char* end, size_t size, size_t alignment) noexcept
    {
        fst_assert(size_t(end - begin) >= size, "size must be greater than range size (end - begin)");

        char* ptr = (char*) (align((uintptr_t) begin, alignment));
        return ptr + size > end ? nullptr : ptr;
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE void* align_range(void* begin, void* end, size_t size, size_t alignment) noexcept
    {
        return align_range((char*) begin, (char*) end, size, alignment);
    }

    ///
    /// Given a pointer ptr to a buffer of size avail_space, returns a pointer aligned by the specified alignment
    /// for size number of bytes and decreases avail_space argument by the number of bytes used for alignment.
    /// The first aligned address is returned.
    ///
    /// The function modifies the pointer only if it would be possible to fit the wanted number of bytes aligned
    /// by the given alignment into the buffer. If the buffer is too small, the function does nothing and returns nullptr.
    ///
    /// @note The behavior is undefined if alignment is not a power of two.
    ///
    /// @param alignment the desired alignment
    /// @param size the size of the storage to be aligned
    /// @param ptr pointer to contiguous storage (a buffer) of at least avail_space bytes
    /// @param avail_space the size of the buffer in which to operate
    ///
    /// @returns the adjusted value of ptr, or nullptr if the space provided is too small.
    FST_NODISCARD FST_ALWAYS_INLINE void* align(size_t alignment, size_t size, void* ptr, size_t& avail_space) noexcept
    {
        fst_assert(avail_space >= size);
        fst_assert(size > 0, "invalid size");
        fst_assert(alignment && !(alignment & (alignment - 1)), "alignment must be a power of two");
        fst_assert(!(alignment & (sizeof(void*) - 1)), "alignment must be a multiple of sizeof(void*)");

        if (size_t offset = static_cast<size_t>(reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)))
        {
            offset = alignment - offset;
            if (avail_space < offset || avail_space - offset < size) { return nullptr; }

            avail_space -= offset;
            return static_cast<char*>(ptr) + offset;
        }

        return ptr;
    }

    /// Get the minimum required size to allocate one element of size `size` aligned as `alignment`
    /// given an a pointer aligned as `input_alignment`.
    ///
    /// @param input_alignment The minimum alignment of the input pointer.
    /// @param size The size of each element.
    /// @param alignment The required alignment of each element.
    /// @param align_end When true, the total size will be aligned on both size.
    ///
    /// @returns the minimum size required.
    FST_NODISCARD FST_ALWAYS_INLINE size_t required_aligned_size(size_t input_alignment, size_t size, size_t alignment, bool align_end) noexcept
    {
        fst_assert(input_alignment > 0, "invalid input alignment");
        fst_assert(size > 0, "invalid size");
        fst_assert(alignment && !(alignment & (alignment - 1)), "alignment must be a power of two");
        fst_assert(!(alignment & (sizeof(void*) - 1)), "alignment must be a multiple of sizeof(void*)");

        const size_t sz = input_alignment >= alignment ? size : alignment - input_alignment + size;
        return align_end ? align(sz, alignment) : sz;
    }

    /// Get the minimum required size to allocate `count` elements of size `size` aligned as `alignment`
    /// given an a pointer aligned as `input_alignment`.
    ///
    /// @param input_alignment The minimum alignment of the input pointer.
    /// @param size The size of each element.
    /// @param alignment The required alignment of each element.
    /// @param count The number of elements required.
    /// @param align_end When true, the total size will be aligned on both size.
    ///
    /// @returns the minimum size required.
    ///
    /// @note A count of one, calls `required_aligned_size(size_t, size_t, size_t, bool)` internally.
    /// @note A count of zero, is an invalid value and will assert.
    FST_NODISCARD FST_ALWAYS_INLINE size_t required_aligned_size(size_t input_alignment, size_t size, size_t alignment, size_t count, bool align_end) noexcept
    {
        fst_assert(input_alignment > 0, "invalid input alignment");
        fst_assert(size > 0, "invalid size");
        fst_assert(alignment && !(alignment & (alignment - 1)), "alignment must be a power of two");
        fst_assert(!(alignment & (sizeof(void*) - 1)), "alignment must be a multiple of sizeof(void*)");
        fst_assert(count > 0, "invalid count");

        if (count == 1) { return required_aligned_size(input_alignment, size, alignment, align_end); }

        const size_t sz = required_aligned_size(input_alignment, size, alignment, true) + align(size, alignment) * count;
        return align_end ? align(sz, alignment) : sz;
    }

    /// Copies `size` bytes from the object pointed to by `src` to the object pointed to by `dst`.
    /// Both objects are reinterpreted as arrays of `unsigned char`.
    ///
    /// @param dst pointer to the memory location to copy to
    /// @param src pointer to the memory location to copy from
    /// @param size number of bytes to copy
    ///
    /// @note If the objects overlap, the behavior is undefined.
    ///.@note If either dest or src is an invalid or null pointer, the behavior is undefined, even if count is zero.
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

    /// Copies `size` bytes from the object pointed to by `src` to the object pointed to by `dst`.
    /// Both objects are reinterpreted as arrays of unsigned char.
    ///
    /// @param dst pointer to the memory location to copy to
    /// @param src pointer to the memory location to copy from
    /// @param size number of bytes to copy
    ///
    /// @note The objects may overlap: copying takes place as if the bytes were copied to a temporary byte
    ///       array and then the bytes were copied from the array to `dst`.
    ///
    /// @note If either `dst` or src is an invalid or null pointer, the behavior is undefined, even if count is zero.
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

    ///
    template <class T>
    FST_ALWAYS_INLINE void memfill(T * dst, __fst::cref_t<T> value, size_t size) noexcept
    {
        for (size_t i = 0; i < size; i++)
        {
            dst[i] = value;
        }
    }

    ///
    template <size_t _Alignment, class T>
    FST_ALWAYS_INLINE void memfill(T * dst, __fst::cref_t<T> value, size_t size) noexcept
    {
        fst_assert(__fst::is_aligned(dst, _Alignment));
        for (size_t i = 0; i < size; i++)
        {
            dst[i] = value;
        }
    }

    ///
    FST_ALWAYS_INLINE void memzero(void* dst, size_t size) noexcept
    {
        __fst::memset(dst, 0, size);
    }

    ///
    template <class T>
    FST_ALWAYS_INLINE void mem_zero(T * dst, size_t size) noexcept
    {
        __fst::memset(dst, 0, size * sizeof(T));
    }

    ///
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

    template <class _SrcIterator, class _DstIterator>
    inline constexpr _DstIterator swap_ranges(const _SrcIterator src_first, const _SrcIterator src_last, _DstIterator dst) noexcept
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
    inline _DstIterator copy_backward(_SrcIterator first, _SrcIterator last, _DstIterator d_last) noexcept
    {
        while (first != last)
            *(--d_last) = *(--last);

        return d_last;
    }

    template <class _SrcIterator, class _DstIterator,
        __fst::enable_if_t<__fst::is_contiguous_iterator<_DstIterator>::value && __fst::is_contiguous_iterator<_SrcIterator>::value, int> = 0>
    inline _DstIterator move_backward(_SrcIterator first, _SrcIterator last, _DstIterator d_last) noexcept
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
    _DstIterator move(_SrcIterator first, _SrcIterator last, _DstIterator dst_first) noexcept
    {
        for (; first != last; ++dst_first, ++first)
            *dst_first = __fst::move(*first);

        return dst_first;
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
FST_END_NAMESPACE
