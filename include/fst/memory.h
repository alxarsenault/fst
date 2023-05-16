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
/// @file fst/memory.h
/// @fst core
/// @author Alexandre Arsenault (alx.arsenault@gmail.com)
/// @date 2023
///
/// @ref memory_management.md
///
/// Declares everything related to memory zones and memory categories.
///

#include "fst/common.h"
#include "fst/status_code.h"
#include "fst/traits.h"
#include "fst/memory_utils.h"

#if __FST_CLANG__
#define fst_alloca(size) __builtin_alloca(size)

#elif __FST_WINDOWS__
#include <malloc.h>
#define fst_alloca(size) _alloca(size)

#else
#define fst_alloca(size) alloca(size)
#endif

FST_BEGIN_NAMESPACE

    ///

    ///
    enum class memory_zone_id : uint64_t {
        invalid = (__fst::numeric_limits<uint64_t>::max)()
    };

    ///
    enum class memory_category_id : uint64_t {
        invalid = (__fst::numeric_limits<uint64_t>::max)()
    };

    FST_INLINE_VAR constexpr memory_zone_id invalid_memory_zone = memory_zone_id::invalid;
    FST_INLINE_VAR constexpr memory_category_id invalid_memory_category = memory_category_id::invalid;

    FST_DECLARE_ENUM_CLASS_OPERATORS(memory_zone_id)
    FST_DECLARE_ENUM_CLASS_OPERATORS(memory_category_id)


    ///
    struct default_memory_zone;

    ///
    struct simd_memory_zone;

    ///
    struct void_memory_zone;

    ///
    struct profiler_memory_zone;

    // clang-format off
    template <class T> struct memory_category;
    template <class T> struct memory_zone;

    template <class T> struct is_memory_category;
    template <class T> struct is_memory_zone;
    template <class T> struct is_static_memory_zone;
    // clang-format on

    namespace memory
    {
        struct category_base
        {
          private:
            template <class T>
            friend struct __fst::memory_category;

            template <class T>
            friend struct __fst::is_memory_category;

            struct restricted_base
            {
                FST_NODISCARD static __fst::memory_category_id get_next_memory_category_id() noexcept;
                FST_IF_PROFILE(static bool register_memory_category(__fst::memory_category_id mid, const char* name) noexcept;)
            };
        };

        ///
        struct zone_base
        {
          private:
            template <class T>
            friend struct __fst::memory_zone;
            template <class T>
            friend struct __fst::is_memory_zone;
            template <class T>
            friend struct __fst::is_static_memory_zone;

            struct restricted_base
            {
                FST_NODISCARD static __fst::memory_zone_id get_next_memory_zone_id() noexcept;
                FST_IF_PROFILE(static bool register_memory_zone(__fst::memory_zone_id zid, const char* name) noexcept;)
                static void move_allocation(void* ptr, __fst::memory_zone_id zid, __fst::memory_category_id from_mid, __fst::memory_category_id to_mid) noexcept;
            };
        };

    } // namespace memory.

    ///
    template <class T>
    struct memory_category : memory::category_base::restricted_base
    {
        FST_NODISCARD static inline __fst::memory_category_id id() noexcept
        {
            static const __fst::memory_category_id value = memory::category_base::restricted_base::get_next_memory_category_id();
            FST_IF_PROFILE(FST_ATTRIBUTE_UNUSED static const bool st = register_memory_category(value, T::name);)
            return value;
        }

        FST_NODISCARD FST_ALWAYS_INLINE operator __fst::memory_category_id() const noexcept { return id(); }
    };

    ///
    struct default_memory_category : __fst::memory_category<default_memory_category>
    {
        static constexpr const char* name = "default";
    };

#if FST_USE_PROFILER
#define FST_DECLARE_MEMORY_CATEGORY(SNAME, NAME)         \
    struct SNAME : FST_NAMESPACE::memory_category<SNAME> \
    {                                                    \
        static constexpr const char* name = NAME;        \
    }
#else
#define FST_DECLARE_MEMORY_CATEGORY(SNAME, NAME) using SNAME = __fst::default_memory_category
#endif // FST_USE_PROFILER

    ///
    FST_DECLARE_MEMORY_CATEGORY(simd_memory_category, "simd");
    FST_DECLARE_MEMORY_CATEGORY(dsp_memory_category, "dsp");
    FST_DECLARE_MEMORY_CATEGORY(ui_memory_category, "ui");
    FST_DECLARE_MEMORY_CATEGORY(profiler_memory_category, "profiler");

    ///
    struct memory_zone_proxy
    {
        using data_type = void*;
        using allocate_callback = void* (*) (size_t, __fst::memory_category_id, data_type);
        using deallocate_callback = void (*)(void*, __fst::memory_category_id, data_type);
        using aligned_allocate_callback = void* (*) (size_t, size_t, __fst::memory_category_id, data_type);
        using aligned_deallocate_callback = void (*)(void*, __fst::memory_category_id, data_type);

        FST_ALWAYS_INLINE memory_zone_proxy(allocate_callback alloc_cb, deallocate_callback dealloc_cb, aligned_allocate_callback a_alloc_cb,
            aligned_deallocate_callback a_dealloc_cb, data_type data, __fst::memory_zone_id zid) noexcept
            : _allocate(alloc_cb)
            , _deallocate(dealloc_cb)
            , _aligned_allocate(a_alloc_cb)
            , _aligned_deallocate(a_dealloc_cb)
            , _data(data)
            , _zone_id(zid)
        {}

        FST_NODISCARD FST_ALWAYS_INLINE void* allocate(size_t size, __fst::memory_category_id mid) const noexcept { return _allocate(size, mid, _data); }

        FST_ALWAYS_INLINE void deallocate(void* ptr, __fst::memory_category_id mid) const noexcept { _deallocate(ptr, mid, _data); }

        FST_NODISCARD FST_ALWAYS_INLINE void* aligned_allocate(size_t size, size_t alignment, __fst::memory_category_id mid) const noexcept
        {
            return _aligned_allocate(size, alignment, mid, _data);
        }

        FST_ALWAYS_INLINE void aligned_deallocate(void* ptr, __fst::memory_category_id mid) const noexcept { _aligned_deallocate(ptr, mid, _data); }

        FST_NODISCARD FST_ALWAYS_INLINE __fst::memory_zone_id id() const noexcept { return _zone_id; }

      private:
        allocate_callback _allocate;
        deallocate_callback _deallocate;
        aligned_allocate_callback _aligned_allocate;
        aligned_deallocate_callback _aligned_deallocate;
        data_type _data;
        __fst::memory_zone_id _zone_id;
    };

    // clang-format off
    template <class T> struct is_memory_zone_proxy : __fst::false_t {};
    template <> struct is_memory_zone_proxy<__fst::memory_zone_proxy> : __fst::true_t {};
    template <class _T> FST_INLINE_VAR constexpr bool is_memory_zone_proxy_v = __fst::is_memory_zone_proxy<_T>::value;
    // clang-format on

    namespace detail
    {
        template <typename T>
        using has_name_t = decltype(T::name);

        template <typename T>
        using allocate_t = decltype(__fst::declval<T>().allocate(__fst::declval<size_t>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using static_allocate_t = decltype(T::allocate(__fst::declval<size_t>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using deallocate_t = decltype(__fst::declval<T>().deallocate(__fst::declval<void*>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using static_deallocate_t = decltype(T::deallocate(__fst::declval<void*>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using aligned_allocate_t
            = decltype(__fst::declval<T>().aligned_allocate(__fst::declval<size_t>(), __fst::declval<size_t>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using static_aligned_allocate_t = decltype(T::aligned_allocate(__fst::declval<size_t>(), __fst::declval<size_t>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using aligned_deallocate_t = decltype(__fst::declval<T>().aligned_deallocate(__fst::declval<void*>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using static_aligned_deallocate_t = decltype(T::aligned_deallocate(__fst::declval<void*>(), __fst::declval<__fst::memory_category_id>()));

        template <typename T>
        using has_allocate = __fst::is_detected<allocate_t, T>;

        template <typename T>
        using has_static_allocate = __fst::is_detected<static_allocate_t, T>;

        template <typename T>
        using has_deallocate = __fst::is_detected<deallocate_t, T>;

        template <typename T>
        using has_static_deallocate = __fst::is_detected<static_deallocate_t, T>;

        template <typename T>
        using has_aligned_allocate = __fst::is_detected<aligned_allocate_t, T>;

        template <typename T>
        using has_static_aligned_allocate = __fst::is_detected<static_aligned_allocate_t, T>;

        template <typename T>
        using has_aligned_deallocate = __fst::is_detected<aligned_deallocate_t, T>;

        template <typename T>
        using has_static_aligned_deallocate = __fst::is_detected<static_aligned_deallocate_t, T>;
    } // namespace detail.

    ///
    template <class T>
    struct is_memory_category : __fst::bool_t<__fst::is_base_of_v<__fst::memory::category_base::restricted_base, T> && __fst::is_detected_v<detail::has_name_t, T>>
    {};

    ///
    template <class T>
    struct is_memory_zone
        : __fst::bool_t<__fst::is_base_of<__fst::memory::zone_base::restricted_base, T>::value
                        && __fst::are_detected<T, detail::allocate_t, detail::deallocate_t, detail::aligned_allocate_t, detail::aligned_deallocate_t>::value>
    {};

    ///
    template <class T>
    struct is_static_memory_zone
        : __fst::bool_t<__fst::is_base_of<__fst::memory::zone_base::restricted_base, T>::value
                        && __fst::are_detected<T, detail::static_allocate_t, detail::static_deallocate_t, detail::static_aligned_allocate_t,
                            detail::static_aligned_deallocate_t>::value>
    {};

    ///
    template <class T>
    struct memory_zone : memory::zone_base::restricted_base
    {
        FST_NODISCARD static inline __fst::memory_zone_id id() noexcept
        {
            static const __fst::memory_zone_id value = memory::zone_base::restricted_base::get_next_memory_zone_id();
            FST_IF_PROFILE(FST_ATTRIBUTE_UNUSED static const bool st = memory::zone_base::restricted_base::register_memory_zone(value, T::name);)
            return value;
        }

        static inline void move_allocation(void* ptr, __fst::memory_category_id from_mid, __fst::memory_category_id to_mid) noexcept
        {
            memory::zone_base::restricted_base::move_allocation(ptr, T::id(), from_mid, to_mid);
        }

        //template <class _FromMemoryCategory, class _ToMemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<__fst::is_memory_category<_FromMemoryCategory>::value && __fst::is_memory_category<_ToMemoryCategory>::value, int> = 0>
        //static inline void move_allocation(void* ptr)
        //{
        //    memory::zone_base::restricted_base::move_allocation(ptr, _MemoryZone::id(), _FromMemoryCategory::id(), _ToMemoryCategory::id());
        //}

        ///*template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD static void* allocate(size_t size)
        //{
        //    return T::allocate(size, _MemoryCategory::id());
        //}*/

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<!__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD void* allocate(size_t size)
        //{
        //    return ((T*) this)->allocate(size, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment)
        //{
        //    return T::aligned_allocate(size, alignment, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<!__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD void* aligned_allocate(size_t size, size_t alignment)
        //{
        //    return ((T*) this)->aligned_allocate(size, alignment, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //static void deallocate(void* ptr)
        //{
        //    T::deallocate(ptr, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<!__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //void deallocate(void* ptr)
        //{
        //    ((T*) this)->deallocate(ptr, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //static void aligned_deallocate(void* ptr)
        //{
        //    T::aligned_deallocate(ptr, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    __fst::enable_if_t<!__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value, int> = 0>
        //void aligned_deallocate(void* ptr)
        //{
        //    ((T*) this)->aligned_deallocate(ptr, _MemoryCategory::id());
        //}

        template <class _MemoryZone = T, __fst::enable_if_t<__fst::is_static_memory_zone<_MemoryZone>::value, int> = 0>
        FST_NODISCARD static inline __fst::memory_zone_proxy proxy() noexcept
        {
            return make_proxy<T>(nullptr);
        }

        template <class _MemoryZone = T, __fst::enable_if_t<!__fst::is_static_memory_zone<_MemoryZone>::value, int> = 0>
        FST_NODISCARD inline __fst::memory_zone_proxy proxy() noexcept
        {
            return make_proxy<T>((T*) this);
        }

      private:
        template <class _MemoryZone>
        FST_NODISCARD static inline __fst::memory_zone_proxy make_proxy(_MemoryZone* z)
        {
            if constexpr (__fst::is_static_memory_zone<_MemoryZone>::value)
            {
                __fst::unused(z);
                return __fst::memory_zone_proxy{ [](size_t size, __fst::memory_category_id mid, void*) { return _MemoryZone::allocate(size, mid); },
                    [](void* ptr, __fst::memory_category_id mid, void*) { _MemoryZone::deallocate(ptr, mid); },
                    [](size_t size, size_t alignment, __fst::memory_category_id mid, void*) { return _MemoryZone::aligned_allocate(size, alignment, mid); },
                    [](void* ptr, __fst::memory_category_id mid, void*) { _MemoryZone::aligned_deallocate(ptr, mid); }, nullptr, _MemoryZone::id() };
            }
            else
            {

                return __fst::memory_zone_proxy{ [](size_t size, __fst::memory_category_id mid, void* data) { return ((_MemoryZone*) data)->allocate(size, mid); },
                    [](void* ptr, __fst::memory_category_id mid, void* data) { ((_MemoryZone*) data)->deallocate(ptr, mid); },
                    [](size_t size, size_t alignment, __fst::memory_category_id mid, void* data)
                    { return ((_MemoryZone*) data)->aligned_allocate(size, alignment, mid); },
                    [](void* ptr, __fst::memory_category_id mid, void* data) { ((_MemoryZone*) data)->aligned_deallocate(ptr, mid); }, (void*) z, _MemoryZone::id() };
            }
        }
    };

    template <class T, class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone, class = void>
    class memory_zone_allocator;

    /// void_memory_zone
    struct void_memory_zone : __fst::memory_zone<void_memory_zone>
    {
        static constexpr const char* name = "void";

        template <class T = void>
        FST_ALWAYS_INLINE static void* allocate(size_t, __fst::memory_category_id) noexcept
        {
            fst_assert(__fst::always_false<T>, "can't allocate in void_memory_zone");
            return nullptr;
        }

        template <class T = void>
        FST_ALWAYS_INLINE static void deallocate(void*, __fst::memory_category_id) noexcept
        {
            fst_assert(__fst::always_false<T>, "can't deallocate in void_memory_zone");
        }

        template <class T = void>
        FST_ALWAYS_INLINE static void* aligned_allocate(size_t, size_t, __fst::memory_category_id) noexcept
        {
            fst_assert(__fst::always_false<T>, "can't aligned_allocate in void_memory_zone");
            return nullptr;
        }

        template <class T = void>
        FST_ALWAYS_INLINE static void aligned_deallocate(void*, __fst::memory_category_id) noexcept
        {
            fst_assert(__fst::always_false<T>, "can't aligned_deallocate in void_memory_zone");
        }
    };

    ///
    struct default_memory_zone : __fst::memory_zone<default_memory_zone>
    {
        static constexpr const char* name = "default";

        FST_NODISCARD static void* allocate(size_t size, __fst::memory_category_id mid) noexcept;
        static void deallocate(void* ptr, __fst::memory_category_id mid) noexcept;

        FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment, __fst::memory_category_id mid) noexcept;
        static void aligned_deallocate(void* ptr, __fst::memory_category_id mid) noexcept;
    };

    ///
    struct simd_memory_zone : public __fst::memory_zone<simd_memory_zone>
    {
        static constexpr const char* name = "simd";

        FST_NODISCARD static size_t default_alignment() noexcept;

        FST_NODISCARD static void* allocate(size_t size, __fst::memory_category_id mid) noexcept;
        static void deallocate(void* ptr, __fst::memory_category_id mid) noexcept;

        FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment, __fst::memory_category_id mid) noexcept;
        static void aligned_deallocate(void* ptr, __fst::memory_category_id mid) noexcept;
    };

    ///
    struct profiler_memory_zone : __fst::memory_zone<profiler_memory_zone>
    {
        static constexpr const char* name = "profiler";

        FST_NODISCARD static void* allocate(size_t size, __fst::memory_category_id mid) noexcept;
        static void deallocate(void* ptr, __fst::memory_category_id mid) noexcept;

        FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment, __fst::memory_category_id mid) noexcept;
        static void aligned_deallocate(void* ptr, __fst::memory_category_id mid) noexcept;
    };

    ///
    struct pool_memory_zone : __fst::memory_zone<pool_memory_zone>
    {
        static constexpr const char* name = "pool";

        pool_memory_zone(size_t buckets, size_t bucket_size) noexcept;
        pool_memory_zone(const pool_memory_zone& other) noexcept;
        pool_memory_zone(pool_memory_zone&& other) noexcept;

        ~pool_memory_zone() noexcept;

        pool_memory_zone& operator=(const pool_memory_zone& other) noexcept;
        pool_memory_zone& operator=(pool_memory_zone&& other) noexcept;

        FST_NODISCARD void* allocate(size_t size, __fst::memory_category_id mid) const noexcept;
        void deallocate(void* ptr, __fst::memory_category_id mid) const noexcept;

        FST_NODISCARD void* aligned_allocate(size_t size, size_t alignment, __fst::memory_category_id mid) const noexcept;
        void aligned_deallocate(void* ptr, __fst::memory_category_id mid) const noexcept;

      private:
        struct pool;
        pool* _pool;
    };

    ///
    template <class _MemoryCategory = __fst::default_memory_category, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
    FST_NODISCARD inline void* allocate(size_t size) noexcept
    {
        return __fst::default_memory_zone::allocate(size, _MemoryCategory::id());
    }
    
    ///
    template <class _MemoryCategory = __fst::default_memory_category, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
    inline void deallocate(void* ptr) noexcept
    {
        __fst::default_memory_zone::deallocate(ptr, _MemoryCategory::id());
    }
    
    ///
    template <class _MemoryCategory = __fst::default_memory_category, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
    FST_NODISCARD inline void* aligned_allocate(size_t size, size_t alignment) noexcept
    {
        return __fst::default_memory_zone::aligned_allocate(size, alignment, _MemoryCategory::id());
    }
    
    ///
    template <class _MemoryCategory = __fst::default_memory_category, __fst::enable_if_t<__fst::is_memory_category<_MemoryCategory>::value, int> = 0>
    inline void aligned_deallocate(void* ptr) noexcept
    {
        __fst::default_memory_zone::aligned_deallocate(ptr, _MemoryCategory::id());
    }

FST_END_NAMESPACE
