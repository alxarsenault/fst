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

#include "fst/memory.h"
#include "fst/stream.h"
#include "fst/vector.h"

#if FST_USE_PROFILER
#define FST_TRACE(...) FST_NAMESPACE::profiler::trace(__VA_ARGS__)
#define FST_STRACE(...) FST_NAMESPACE::profiler::scoped_trace FST_CONCAT(_ssss, __LINE__) = FST_NAMESPACE::profiler::strace(__VA_ARGS__)
#else
#define FST_TRACE(...) ((void) 0)
#define FST_STRACE(...) ((void) 0)
#endif

FST_BEGIN_NAMESPACE

    class profiler
    {
      public:
        struct result
        {
            __fst::memory_zone_id zid;
            __fst::memory_category_id mid;
            size_t size;
        };

        static __fst::output_stream<char>& tracer(const char* section);

        template <typename... Ts>
        static inline void trace(const char* section, const Ts&... ts)
        {

            __fst::output_stream<char>& stream = tracer(section);
            if constexpr (sizeof...(ts) > 1)
            {
                basic_print<__fst::output_stream<char>, __fst::stream_detail::sep_t, __fst::stream_detail::sep_endl_t>(stream << __fst::separator, ts...);
            }
            else if constexpr (sizeof...(ts) == 1)
            {
                print_element(stream << __fst::separator, ts...);
                stream << __fst::separator_endl;
            }
            else { stream << __fst::separator_endl; }
        }

        struct scoped_trace
        {
            const char* section;

            ~scoped_trace() { trace(section, "end"); }
        };

        template <typename... Ts>
        FST_NODISCARD static inline scoped_trace strace(const char* section, const Ts&... ts)
        {

            __fst::output_stream<char>& stream = tracer("section")
                                                << __fst::stream_detail::sep_t{} << __fst::padded_right<32>(section) << __fst::stream_detail::sep_t{} << " begin";

            if constexpr (sizeof...(ts) > 1)
            {
                basic_print<__fst::output_stream<char>, __fst::stream_detail::sep_t, __fst::stream_detail::sep_endl_t>(stream << __fst::stream_detail::sep_t{}, ts...);
            }
            else if constexpr (sizeof...(ts) == 1)
            {
                print_element(stream << __fst::stream_detail::sep_t{}, ts...);
                stream << __fst::stream_detail::sep_endl_t{};
            }
            else { stream << __fst::stream_detail::sep_endl_t{}; }

            return scoped_trace{ section };
        }

        using result_type = __fst::vector<result, alignof(result), __fst::profiler_memory_category,  __fst::profiler_memory_zone>;

#if FST_USE_PROFILER

        static result_type report();
        static void output(__fst::output_stream<char>& stream);

        template <class _MemoryCategory, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        static inline void register_name()
        {
            allocated(nullptr, 0, __fst::invalid_memory_zone, _MemoryCategory::id(), _MemoryCategory::name);
        }

        static inline void register_name(__fst::memory_category_id mid, const char* name) { allocated(nullptr, 0, __fst::invalid_memory_zone, mid, name); }
        static inline void register_name(__fst::memory_zone_id zid, const char* name) { allocated(nullptr, 0, zid, __fst::invalid_memory_category, name); }

        template <class _MemoryZone, __fst::enable_if_t<__fst::is_memory_zone<_MemoryZone>::value, int> = 0>
        static inline void register_name()
        {
            allocated(nullptr, 0, _MemoryZone::id(), __fst::invalid_memory_category, nullptr);
        }

        template <class _MemoryZone, class _MemoryCategory, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        static inline void allocated(void* ptr, size_t size)
        {
            allocated(ptr, size, _MemoryZone::id(), _MemoryCategory::id(), _MemoryCategory::name);
        }

        template <class _MemoryZone, class _FromMemoryCategory, class _ToMemoryCategory,
            __fst::enable_if_t<__fst::is_memory_category<_FromMemoryCategory>::value && __fst::is_memory_category<_ToMemoryCategory>::value, int> = 0>
        static inline void move_allocation(void* ptr)
        {
            move_allocation(ptr, _MemoryZone::id(), _FromMemoryCategory::id(), _ToMemoryCategory::id());
        }

        template <class _MemoryZone, class _MemoryCategory, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        static inline void deallocated(void* ptr)
        {
            deallocated(ptr, _MemoryZone::id(), _MemoryCategory::id());
        }

        static void move_allocation(void* ptr, __fst::memory_zone_id zid, __fst::memory_category_id from_mid, __fst::memory_category_id to_mid);

        static void allocated(void* ptr, size_t size, __fst::memory_zone_id zid, __fst::memory_category_id mid, const char* name = nullptr);

        static void deallocated(void* ptr, __fst::memory_zone_id zid, __fst::memory_category_id mid);

#else

        static result_type report() { return {}; }
        FST_ALWAYS_INLINE static constexpr void output(__fst::output_stream<char>&) {}

        template <class _MemoryCategory, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        FST_ALWAYS_INLINE static constexpr void register_name()
        {}

        template <class _MemoryZone, __fst::enable_if_t<__fst::is_memory_zone<_MemoryZone>::value, int> = 0>
        FST_ALWAYS_INLINE static constexpr void register_name()
        {}

        template <class _MemoryZone, class _MemoryCategory, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        FST_ALWAYS_INLINE static constexpr void allocated(void* ptr, size_t size)
        {}

        template <class _MemoryZone, class _FromMemoryCategory, class _ToMemoryCategory,
            __fst::enable_if_t<__fst::is_memory_category<_FromMemoryCategory>::value && __fst::is_memory_category<_ToMemoryCategory>::value, int> = 0>
        FST_ALWAYS_INLINE static constexpr void move_allocation(void* ptr)
        {}

        template <class _MemoryZone, class _MemoryCategory, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        FST_ALWAYS_INLINE static constexpr void deallocated(void* ptr)
        {}

        FST_ALWAYS_INLINE static constexpr void move_allocation(void*, __fst::memory_zone_id, __fst::memory_category_id, __fst::memory_category_id) {}

        FST_ALWAYS_INLINE static constexpr void allocated(void*, size_t, __fst::memory_zone_id, __fst::memory_category_id, const char* = nullptr) {}

        FST_ALWAYS_INLINE static constexpr void deallocated(void*, __fst::memory_zone_id, __fst::memory_category_id) {}
#endif

      private:
        profiler() = default;
    };

FST_END_NAMESPACE
