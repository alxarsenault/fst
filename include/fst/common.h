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
#define _FST FST_NAMESPACE::__FST_ABI_NAMESPACE__

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

FST_NODISCARD inline void* operator new(size_t, void* memory, _FST::placement_new_tag) noexcept
{
    //fst::printf("klklkklk %d\n", int(jj));

    return memory;
}
inline void operator delete(void*, void*, _FST::placement_new_tag) noexcept {}

// FST_DECLARE_ENUM_CLASS_OPERATORS
#include "fst/detail/__flags.h"

#include "fst/detail/__assert.h"
#include "fst/detail/__container.h"

#include "fst/detail/__builtin_traits.h"
#include "fst/detail/__traits.h"
#include "fst/detail/__initializer_list.h"

//
FST_PRAGMA_DISABLE_WARNING_MSVC(4505)
