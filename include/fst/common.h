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

#include "fst/config.h"
#include <stdlib.h>
#include <stddef.h>

#include "fst/detail/__macro.h"

#ifndef FST_VERSION_MAJOR
#define FST_VERSION_MAJOR 0
#define FST_VERSION_MINOR 1
#define FST_VERSION_PATCH 0
#define FST_VERSION_BUILD 0
#endif

///
#define FST_UNIT_TESTED

///
#define __FST_VERSION_ID__ FST_CONCAT(FST_CONCAT(FST_VERSION_MAJOR, FST_VERSION_MINOR), FST_VERSION_PATCH)
#define __FST_ABI_NAMESPACE__ FST_CONCAT(__, __FST_VERSION_ID__)

#define __FST_VERSION_STR__ FST_STRINGIFY(FST_VERSION_MAJOR.FST_VERSION_MINOR.FST_VERSION_PATCH)
#define __FST_LONG_VERSION_STR__ FST_STRINGIFY(FST_VERSION_MAJOR.FST_VERSION_MINOR.FST_VERSION_PATCH.FST_VERSION_BUILD)

///
#define FST_BEGIN_NAMESPACE                    \
    namespace FST_NAMESPACE                    \
    {                                          \
        inline namespace __FST_ABI_NAMESPACE__ \
        {

///
#define FST_END_NAMESPACE \
    }                     \
    }

///
#define FST_BEGIN_SUB_NAMESPACE(X) \
    FST_BEGIN_NAMESPACE            \
        namespace X                \
        {

///
#define FST_END_SUB_NAMESPACE \
    FST_END_NAMESPACE         \
    }

///
#define __fst FST_NAMESPACE::__FST_ABI_NAMESPACE__

#if FST_PLATFORM_HAS_IMMINTRIN_H

#if FST_PLATFORM_HAS_SSE2 && FST_USE_SIMD_128
#define FST_SIMD_128 1
#else
#define FST_SIMD_128 0
#endif

#if FST_PLATFORM_HAS_AVX && FST_USE_SIMD_256
#define FST_SIMD_256 1
#else
#define FST_SIMD_256 0
#endif

#if FST_PLATFORM_HAS_AVX2 && FST_USE_SIMD_512
#define FST_SIMD_512 1
#else
#define FST_SIMD_512 0
#endif

#endif //

// #if defined(FST_SIMD_128) && FST_IS_MACRO_EMPTY(FST_SIMD_128)

//
// MARK: Platform type.
//
#include "fst/detail/__platform.h"

//
//
//
#include "fst/detail/__arch.h"

//
#include "fst/detail/__int.h"

//
#include "fst/detail/__compiler.h"

//
// MARK: Build type.
//
#undef __FST_DEBUG_BUILD__
#undef __FST_RELEASE_BUILD__

#ifdef NDEBUG
// Release.
#define __FST_RELEASE_BUILD__ 1
#define __FST_DEBUG_BUILD__ 0
#else
// Debug.
#define __FST_RELEASE_BUILD__ 0
#define __FST_DEBUG_BUILD__ 1
#endif

//
#include "fst/detail/__platform_macro.h"

///
#if FST_USE_PROFILER
#define FST_IF_PROFILE(...) __VA_ARGS__
#else
#define FST_IF_PROFILE(...)
#endif

#define fst_placement_new(mem) ::new (mem, FST_NAMESPACE::placement_new_tag{})

FST_BEGIN_NAMESPACE

    void print_version() noexcept;

    using max_align_t = double; // most aligned type

    struct placement_new_tag
    {};

    template <class... Args>
    FST_ALWAYS_INLINE constexpr void unused(Args && ...) noexcept
    {}

    int printf(const char* format, ...) noexcept;
    int warnprintf(const char* format, ...) noexcept;
    size_t write_stdout(const char* data, size_t size) noexcept;
    size_t write_wstdout(const wchar_t* data, size_t size) noexcept;
    int fprintf(void* const file, const char* format, ...) noexcept;
    int snprintf(char* buffer, size_t bufsz, const char* format, ...) noexcept;

FST_END_NAMESPACE

// numeric_limits
#include "fst/detail/__numeric_limits.h"

FST_NODISCARD inline void* operator new(size_t, void* memory, __fst::placement_new_tag) noexcept
{
    return memory;
}
inline void operator delete(void*, void*, __fst::placement_new_tag) noexcept {}

// FST_DECLARE_ENUM_CLASS_OPERATORS
#include "fst/detail/__flags.h"

FST_BEGIN_NAMESPACE
    FST_NODISCARD constexpr bool is_constant_evaluated() noexcept
    {
        return __builtin_is_constant_evaluated();
    }
FST_END_NAMESPACE

#include "fst/detail/__assert.h"
#include "fst/detail/__container.h"

#include "fst/detail/__builtin_traits.h"
#include "fst/detail/__traits.h"
#include "fst/detail/__initializer_list.h"

FST_BEGIN_NAMESPACE

    FST_INLINE_VAR constexpr size_t default_alignment = 16;
    FST_INLINE_VAR constexpr size_t default_alignment_bit_index = 4;

    FST_INLINE_VAR constexpr size_t default_vectorized_alignment = 32;
    FST_INLINE_VAR constexpr size_t default_vectorized_bit_index = 5;
    FST_INLINE_VAR constexpr size_t default_vectorized_size_threshold = 4096;

    template <class _T>
    FST_INLINE_VAR constexpr size_t default_alignof = alignof(_T) < __fst::default_alignment ? __fst::default_alignment : alignof(_T);

    template <class _T>
    FST_INLINE_VAR constexpr size_t default_vectorized_alignof = alignof(_T) < __fst::default_vectorized_alignment ? __fst::default_vectorized_alignment : alignof(_T);

    FST_ALWAYS_INLINE constexpr size_t clip_alignment(size_t alignment) noexcept
    {
        fst_assert(alignment && !(alignment & (alignment - 1)), "alignment must be a power of two");
        if (alignment < default_alignment) { alignment = default_alignment; }
        fst_assert(!(alignment & (sizeof(void*) - 1)), "alignment must be a multiple of sizeof(void*)");
        return alignment;
    }

    FST_ALWAYS_INLINE constexpr size_t clip_vectorized_alignment(size_t alignment) noexcept
    {
        fst_assert(alignment && !(alignment & (alignment - 1)), "alignment must be a power of two");
        if (alignment < default_vectorized_alignment) { alignment = default_vectorized_alignment; }
        fst_assert(!(alignment & (sizeof(void*) - 1)), "alignment must be a multiple of sizeof(void*)");
        return alignment;
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
    FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t align(uintptr_t ptr, size_t alignment) noexcept
    {
        return (ptr + (alignment - 1)) & ~(alignment - 1);
    }

    FST_NODISCARD FST_ALWAYS_INLINE char* align(char* ptr, size_t alignment) noexcept
    {
        return (char*) (align((uintptr_t) ptr, alignment));
    }

    FST_NODISCARD FST_ALWAYS_INLINE void* align(void* ptr, size_t alignment) noexcept
    {
        return (void*) (align((uintptr_t) ptr, alignment));
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE char* align_range(char* begin, char* end, size_t size, size_t alignment) noexcept
    {
        fst_assert(size_t(end - begin) >= size, "wrong size");

        char* ptr = (char*) (align((uintptr_t) begin, alignment));
        return ptr + size > end ? nullptr : ptr;
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE void* align_range(void* begin, void* end, size_t size, size_t alignment) noexcept
    {
        return align_range((char*) begin, (char*) end, size, alignment);
    }

    ///
    FST_NODISCARD FST_ALWAYS_INLINE void* align_range(size_t alignment, size_t size, void*& ptr, size_t& avail_space) noexcept
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

    ///
    FST_NODISCARD FST_ALWAYS_INLINE size_t required_aligned_size(size_t input_alignment, size_t size, size_t alignment, bool align_end) noexcept
    {
        const size_t sz = input_alignment >= alignment ? size : alignment - input_alignment + size;
        return align_end ? align(sz, alignment) :sz;
    }

    FST_NODISCARD FST_ALWAYS_INLINE size_t required_aligned_size(size_t input_alignment, size_t size, size_t alignment, size_t count, bool align_end) noexcept
    {
        const size_t first = required_aligned_size(input_alignment, size, alignment, true);
        const size_t asize = align(size, alignment) * count;
        return align_end ? align(first + asize, alignment) : first + asize;
    }

    /*FST_NODISCARD void** allocate_audio_buffer(size_t channel_size, size_t buffer_size, size_t type_size) noexcept
        {
            fst_assert(buffer_size != 0 && channel_size != 0, "channel_size and buffer_size cannot be zero");
            if (!buffer_size || !channel_size) { return nullptr; }

            const size_t cache_size = __fst::mem_cache_size();

            const size_t buffer_ptr_size = channel_size * sizeof(void*);
            const size_t channel_buffer_size = buffer_size * type_size;

            const size_t total_buffer_size = channel_size * channel_buffer_size + channel_size * (cache_size - 1);
            const size_t total_size = buffer_ptr_size + total_buffer_size;

            void* bytes = _MemoryZone::allocate(total_size, _MemoryCategory::id());

            if (bytes == nullptr) { return nullptr; }

            void** buffers = (void**) bytes;

            void* raw_ptr = ((uint8_t*) bytes) + buffer_ptr_size;
            const uint8_t* raw_ptr_end = static_cast<uint8_t*>(raw_ptr) + total_buffer_size;

            size_t sp = total_buffer_size;

            for (size_t i = 0; i < channel_size; i++)
            {
                void* channel_ptr = __fst::align_range(cache_size, channel_buffer_size, raw_ptr, sp);

                fst_assert(channel_ptr != nullptr, "Can't align buffer with cache size.");

                if (channel_ptr == nullptr)
                {
                    deallocate_audio_buffer(buffers);
                    return nullptr;
                }

                buffers[i] = channel_ptr;

                raw_ptr = static_cast<uint8_t*>(channel_ptr) + channel_buffer_size;
                sp = (size_t)(raw_ptr_end - static_cast<uint8_t*>(static_cast<void*>(buffers[i])));
            }

            return buffers;
        }*/
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

    /*template <class... Ts>
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
    }*/

    struct source_location
    {
        FST_NODISCARD static consteval source_location current(
            const uint_least32_t _Line_ = __builtin_LINE(), const char* const _File_ = __builtin_FILE(), const char* const _Function_ = __builtin_FUNCTION()) noexcept
        {
            source_location _Result;
            _Result._Line = _Line_;
            _Result._File = _File_;
            _Result._Function = _Function_;
            return _Result;
        }

        FST_NODISCARD_CTOR constexpr source_location() noexcept = default;

        FST_NODISCARD constexpr uint_least32_t line() const noexcept { return _Line; }
        FST_NODISCARD constexpr const char* file_name() const noexcept { return _File; }
        FST_NODISCARD constexpr const char* function_name() const noexcept { return _Function; }

      private:
        uint_least32_t _Line{};
        const char* _File = "";
        const char* _Function = "";
    };
FST_END_NAMESPACE
//
FST_PRAGMA_DISABLE_WARNING_MSVC(4505)
