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

#if __FST_CLANG__
#define fst_alloca(size) __builtin_alloca(size)

#else
#if __FST_WINDOWS__
#include <malloc.h>
#define fst_alloca(size) alloca(size)
#else
#define fst_alloca(size) alloca(size)

#endif //
#endif
FST_BEGIN_NAMESPACE

    ///
    using byte = uint8_t;

    ///
    enum class memory_zone_id : uint64_t {
        invalid = (_FST::numeric_limits<uint64_t>::max)()
    };

    ///
    enum class memory_category_id : uint64_t {
        invalid = (_FST::numeric_limits<uint64_t>::max)()
    };

    FST_INLINE_VAR constexpr memory_zone_id invalid_memory_zone = memory_zone_id::invalid;
    FST_INLINE_VAR constexpr memory_category_id invalid_memory_category = memory_category_id::invalid;

    FST_DECLARE_ENUM_CLASS_OPERATORS(memory_zone_id)
    FST_DECLARE_ENUM_CLASS_OPERATORS(memory_category_id)

    namespace simd
    {
        template <class T>
        struct vector_type_base
        {
            using type = T;
        };

        template <class T>
        struct vector_type
        {
            using type = typename vector_type_base<_FST::remove_const_t<T>>::type;
        };

        ///
        template <class T>
        using vector_type_t = typename vector_type<T>::type;

        ///
        template <class T>
        using vector_type_alignment = _FST::integral_constant<size_t, alignof(_FST::simd::vector_type_t<T>)>;

        template <class T>
        FST_INLINE_VAR constexpr size_t vector_type_alignment_v = vector_type_alignment<T>::value;
    } // namespace simd.

    ///
    extern size_t mem_cache_size() noexcept;

    ///
    extern size_t mem_page_size() noexcept;

    ///
    template <class T>
    struct memory_category;

    template <class T>
    struct is_memory_category;

    template <class T>
    struct memory_zone;

    template <class T>
    struct is_memory_zone;

    template <class T>
    struct is_static_memory_zone;

    namespace memory
    {
        ///
        FST_NODISCARD extern _FST::memory_zone_id get_next_memory_zone_id() noexcept;

        ///
        FST_NODISCARD extern _FST::memory_category_id get_next_memory_category_id() noexcept;

        struct category_base
        {
          private:
            template <class T>
            friend struct _FST::memory_category;

            template <class T>
            friend struct _FST::is_memory_category;

            struct restricted_base
            {};
        };

        ///
        struct zone_base
        {
          private:
            template <class T>
            friend struct _FST::memory_zone;

            template <class T>
            friend struct _FST::is_memory_zone;

            template <class T>
            friend struct _FST::is_static_memory_zone;

            struct restricted_base
            {
                static void move_allocation(void* ptr, _FST::memory_zone_id zid, _FST::memory_category_id from_mid, _FST::memory_category_id to_mid) noexcept;
            };
        };

    } // namespace memory.

#if FST_USE_PROFILER
    extern bool register_memory_category(_FST::memory_category_id mid, const char* name) noexcept;
    extern void register_memory_zone(_FST::memory_zone_id zid, const char* name) noexcept;

    template <class _MemoryCategory>
    struct memory_category_register
    {
        inline memory_category_register() noexcept { register_memory_category(_MemoryCategory::id(), _MemoryCategory::name); }
    };

    template <class _MemoryZone>
    struct memory_zone_register
    {
        inline memory_zone_register() noexcept { register_memory_zone(_MemoryZone::id(), _MemoryZone::name); }
    };
#endif
    ///
    template <class T>
    struct memory_category : memory::category_base::restricted_base
    {
        FST_NODISCARD static inline _FST::memory_category_id id() noexcept
        {
            static const _FST::memory_category_id value = memory::get_next_memory_category_id();
#if FST_USE_PROFILER
            FST_ATTRIBUTE_UNUSED static const bool st = register_memory_category(value, T::name);
#endif
            return value;
        }

        FST_NODISCARD FST_ALWAYS_INLINE operator _FST::memory_category_id() const noexcept { return id(); }
    };

    /*struct memory_category_t
    {
        _FST::memory_category_id id;
        const char* name;
    };*/

    ///
    struct default_memory_category : _FST::memory_category<default_memory_category>
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
#define FST_DECLARE_MEMORY_CATEGORY(SNAME, NAME) using SNAME = _FST::default_memory_category
#endif // FST_USE_PROFILER

    ///
    FST_DECLARE_MEMORY_CATEGORY(simd_memory_category, "simd");
    FST_DECLARE_MEMORY_CATEGORY(dsp_memory_category, "dsp");
    FST_DECLARE_MEMORY_CATEGORY(ui_memory_category, "ui");
    FST_DECLARE_MEMORY_CATEGORY(profiler_memory_category, "profiler");

    struct memory_zone_proxy
    {
        using data_type = void*;
        using allocate_callback = void* (*) (size_t, _FST::memory_category_id, data_type);
        using deallocate_callback = void (*)(void*, _FST::memory_category_id, data_type);
        using aligned_allocate_callback = void* (*) (size_t, size_t, _FST::memory_category_id, data_type);
        using aligned_deallocate_callback = void (*)(void*, _FST::memory_category_id, data_type);

        FST_ALWAYS_INLINE memory_zone_proxy(allocate_callback alloc_cb, deallocate_callback dealloc_cb, aligned_allocate_callback a_alloc_cb,
            aligned_deallocate_callback a_dealloc_cb, data_type data, _FST::memory_zone_id zid) noexcept
            : _allocate(alloc_cb)
            , _deallocate(dealloc_cb)
            , _aligned_allocate(a_alloc_cb)
            , _aligned_deallocate(a_dealloc_cb)
            , _data(data)
            , _zone_id(zid)
        {}

        FST_NODISCARD FST_ALWAYS_INLINE void* allocate(size_t size, _FST::memory_category_id mid) const noexcept { return _allocate(size, mid, _data); }

        FST_ALWAYS_INLINE void deallocate(void* ptr, _FST::memory_category_id mid) const noexcept { _deallocate(ptr, mid, _data); }

        FST_NODISCARD FST_ALWAYS_INLINE void* aligned_allocate(size_t size, size_t alignment, _FST::memory_category_id mid) const noexcept
        {
            return _aligned_allocate(size, alignment, mid, _data);
        }

        FST_ALWAYS_INLINE void aligned_deallocate(void* ptr, _FST::memory_category_id mid) const noexcept { _aligned_deallocate(ptr, mid, _data); }

        FST_NODISCARD FST_ALWAYS_INLINE _FST::memory_zone_id id() const noexcept { return _zone_id; }

      private:
        allocate_callback _allocate;
        deallocate_callback _deallocate;
        aligned_allocate_callback _aligned_allocate;
        aligned_deallocate_callback _aligned_deallocate;
        data_type _data;
        _FST::memory_zone_id _zone_id;
    };

    template <class T>
    struct is_memory_zone_proxy : _FST::false_t
    {};

    template <>
    struct is_memory_zone_proxy<_FST::memory_zone_proxy> : _FST::true_t
    {};

    template <class _T>
    FST_INLINE_VAR constexpr bool is_memory_zone_proxy_v = _FST::is_memory_zone_proxy<_T>::value;

    namespace detail
    {
        template <typename T>
        using has_name_t = decltype(T::name);

        template <typename T>
        using allocate_t = decltype(_FST::declval<T>().allocate(_FST::declval<size_t>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using static_allocate_t = decltype(T::allocate(_FST::declval<size_t>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using deallocate_t = decltype(_FST::declval<T>().deallocate(_FST::declval<void*>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using static_deallocate_t = decltype(T::deallocate(_FST::declval<void*>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using aligned_allocate_t
            = decltype(_FST::declval<T>().aligned_allocate(_FST::declval<size_t>(), _FST::declval<size_t>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using static_aligned_allocate_t = decltype(T::aligned_allocate(_FST::declval<size_t>(), _FST::declval<size_t>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using aligned_deallocate_t = decltype(_FST::declval<T>().aligned_deallocate(_FST::declval<void*>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using static_aligned_deallocate_t = decltype(T::aligned_deallocate(_FST::declval<void*>(), _FST::declval<_FST::memory_category_id>()));

        template <typename T>
        using has_allocate = _FST::is_detected<allocate_t, T>;

        template <typename T>
        using has_static_allocate = _FST::is_detected<static_allocate_t, T>;

        template <typename T>
        using has_deallocate = _FST::is_detected<deallocate_t, T>;

        template <typename T>
        using has_static_deallocate = _FST::is_detected<static_deallocate_t, T>;

        template <typename T>
        using has_aligned_allocate = _FST::is_detected<aligned_allocate_t, T>;

        template <typename T>
        using has_static_aligned_allocate = _FST::is_detected<static_aligned_allocate_t, T>;

        template <typename T>
        using has_aligned_deallocate = _FST::is_detected<aligned_deallocate_t, T>;

        template <typename T>
        using has_static_aligned_deallocate = _FST::is_detected<static_aligned_deallocate_t, T>;
    } // namespace detail.

    ///
    template <class T>
    struct is_memory_category : _FST::bool_t<_FST::is_base_of_v<_FST::memory::category_base::restricted_base, T> && _FST::is_detected_v<detail::has_name_t, T>>
    {};

    ///
    template <class T>
    struct is_memory_zone
        : _FST::bool_t<_FST::is_base_of<_FST::memory::zone_base::restricted_base, T>::value
                       && _FST::are_detected<T, detail::allocate_t, detail::deallocate_t, detail::aligned_allocate_t, detail::aligned_deallocate_t>::value>
    {};

    ///
    template <class T>
    struct is_static_memory_zone
        : _FST::bool_t<_FST::is_base_of<_FST::memory::zone_base::restricted_base, T>::value
                       && _FST::are_detected<T, detail::static_allocate_t, detail::static_deallocate_t, detail::static_aligned_allocate_t,
                           detail::static_aligned_deallocate_t>::value>
    {};

    ///
    template <class T>
    struct memory_zone : memory::zone_base::restricted_base
    {
        FST_NODISCARD static inline _FST::memory_zone_id id() noexcept
        {
            static const _FST::memory_zone_id value = memory::get_next_memory_zone_id();
            return value;
        }

        static inline void move_allocation(void* ptr, _FST::memory_category_id from_mid, _FST::memory_category_id to_mid)
        {
            memory::zone_base::restricted_base::move_allocation(ptr, T::id(), from_mid, to_mid);
        }

        //template <class _FromMemoryCategory, class _ToMemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<_FST::is_memory_category<_FromMemoryCategory>::value && _FST::is_memory_category<_ToMemoryCategory>::value, int> = 0>
        //static inline void move_allocation(void* ptr)
        //{
        //    memory::zone_base::restricted_base::move_allocation(ptr, _MemoryZone::id(), _FromMemoryCategory::id(), _ToMemoryCategory::id());
        //}

        ///*template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD static void* allocate(size_t size)
        //{
        //    return T::allocate(size, _MemoryCategory::id());
        //}*/

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<!_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD void* allocate(size_t size)
        //{
        //    return ((T*) this)->allocate(size, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment)
        //{
        //    return T::aligned_allocate(size, alignment, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<!_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //FST_NODISCARD void* aligned_allocate(size_t size, size_t alignment)
        //{
        //    return ((T*) this)->aligned_allocate(size, alignment, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //static void deallocate(void* ptr)
        //{
        //    T::deallocate(ptr, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<!_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //void deallocate(void* ptr)
        //{
        //    ((T*) this)->deallocate(ptr, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //static void aligned_deallocate(void* ptr)
        //{
        //    T::aligned_deallocate(ptr, _MemoryCategory::id());
        //}

        //template <class _MemoryCategory, class _MemoryZone = T,
        //    _FST::enable_if_t<!_FST::is_static_memory_zone<_MemoryZone>::value && _FST::is_memory_category<_MemoryCategory>::value, int> = 0>
        //void aligned_deallocate(void* ptr)
        //{
        //    ((T*) this)->aligned_deallocate(ptr, _MemoryCategory::id());
        //}

        template <class _MemoryZone = T, _FST::enable_if_t<_FST::is_static_memory_zone<_MemoryZone>::value, int> = 0>
        FST_NODISCARD static inline _FST::memory_zone_proxy proxy() noexcept
        {
            return make_proxy<T>(nullptr);
        }

        template <class _MemoryZone = T, _FST::enable_if_t<!_FST::is_static_memory_zone<_MemoryZone>::value, int> = 0>
        FST_NODISCARD inline _FST::memory_zone_proxy proxy() noexcept
        {
            return make_proxy<T>((T*) this);
        }

      private:
        template <class _MemoryZone>
        FST_NODISCARD static inline _FST::memory_zone_proxy make_proxy(_MemoryZone* z)
        {
            if constexpr (_FST::is_static_memory_zone<_MemoryZone>::value)
            {
                _FST::unused(z);
                return _FST::memory_zone_proxy{ [](size_t size, _FST::memory_category_id mid, void*) { return _MemoryZone::allocate(size, mid); },
                    [](void* ptr, _FST::memory_category_id mid, void*) { _MemoryZone::deallocate(ptr, mid); },
                    [](size_t size, size_t alignment, _FST::memory_category_id mid, void*) { return _MemoryZone::aligned_allocate(size, alignment, mid); },
                    [](void* ptr, _FST::memory_category_id mid, void*) { _MemoryZone::aligned_deallocate(ptr, mid); }, nullptr, _MemoryZone::id() };
            }
            else
            {

                return _FST::memory_zone_proxy{ [](size_t size, _FST::memory_category_id mid, void* data) { return ((_MemoryZone*) data)->allocate(size, mid); },
                    [](void* ptr, _FST::memory_category_id mid, void* data) { ((_MemoryZone*) data)->deallocate(ptr, mid); },
                    [](size_t size, size_t alignment, _FST::memory_category_id mid, void* data) { return ((_MemoryZone*) data)->aligned_allocate(size, alignment, mid); },
                    [](void* ptr, _FST::memory_category_id mid, void* data) { ((_MemoryZone*) data)->aligned_deallocate(ptr, mid); }, (void*) z, _MemoryZone::id() };
            }
        }

#if FST_USE_PROFILER
        static memory_zone_register<T> __memory_zone_registration;
#endif
    };

    template <class T, class _MemoryZone, class _MemoryCategory, class = void>
    class memory_zone_allocator;

#if FST_USE_PROFILER
    template <class T>
    memory_zone_register<T> memory_zone<T>::__memory_zone_registration = {};
#endif

    /// void_memory_zone
    struct void_memory_zone : _FST::memory_zone<void_memory_zone>
    {
        static constexpr const char* name = "void";

        template <class T = void>
        FST_ALWAYS_INLINE static void* allocate(size_t, _FST::memory_category_id) noexcept
        {
            static_assert(_FST::always_false<T>, "can't allocate in void_memory_zone");
            return nullptr;
        }

        template <class T = void>
        FST_ALWAYS_INLINE static void deallocate(void*, _FST::memory_category_id) noexcept
        {
            static_assert(_FST::always_false<T>, "can't deallocate in void_memory_zone");
        }

        template <class T = void>
        FST_ALWAYS_INLINE static void* aligned_allocate(size_t, size_t, _FST::memory_category_id) noexcept
        {
            static_assert(_FST::always_false<T>, "can't aligned_allocate in void_memory_zone");
            return nullptr;
        }

        template <class T = void>
        FST_ALWAYS_INLINE static void aligned_deallocate(void*, _FST::memory_category_id) noexcept
        {
            static_assert(_FST::always_false<T>, "can't aligned_deallocate in void_memory_zone");
        }
    };

    FST_INLINE_VAR constexpr void_memory_zone _global_void_memory_zone = {};

    ///
    struct default_memory_zone : _FST::memory_zone<default_memory_zone>
    {
        static constexpr const char* name = "default";

        //
        FST_NODISCARD static void* allocate(size_t size, _FST::memory_category_id mid) noexcept;

        //
        static void deallocate(void* ptr, _FST::memory_category_id mid) noexcept;

        //
        FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment, _FST::memory_category_id mid) noexcept;

        //
        static void aligned_deallocate(void* ptr, _FST::memory_category_id mid) noexcept;
    };

    ///
    struct simd_memory_zone : public _FST::memory_zone<simd_memory_zone>
    {
        static constexpr const char* name = "simd";

        FST_NODISCARD static size_t default_alignment() noexcept;

        //
        FST_NODISCARD static void* allocate(size_t size, _FST::memory_category_id mid) noexcept;

        //
        static void deallocate(void* ptr, _FST::memory_category_id mid) noexcept;

        //
        FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment, _FST::memory_category_id mid) noexcept;

        //
        static void aligned_deallocate(void* ptr, _FST::memory_category_id mid) noexcept;
    };

    ///
    struct profiler_memory_zone : _FST::memory_zone<profiler_memory_zone>
    {
        static constexpr const char* name = "profiler";

        //
        FST_NODISCARD static void* allocate(size_t size, _FST::memory_category_id mid) noexcept;

        //
        static void deallocate(void* ptr, _FST::memory_category_id mid) noexcept;

        //
        FST_NODISCARD static void* aligned_allocate(size_t size, size_t alignment, _FST::memory_category_id mid) noexcept;

        //
        static void aligned_deallocate(void* ptr, _FST::memory_category_id mid) noexcept;
    };

    template <class _MemoryCategory = _FST::default_memory_category, _FST::enable_if_t<_FST::is_memory_category<_MemoryCategory>::value, int> = 0>
    FST_NODISCARD inline void* allocate(size_t size) noexcept
    {
        return _FST::default_memory_zone::allocate(size, _MemoryCategory::id());
    }

    template <class _MemoryCategory = _FST::default_memory_category, _FST::enable_if_t<_FST::is_memory_category<_MemoryCategory>::value, int> = 0>
    inline void deallocate(void* ptr) noexcept
    {
        _FST::default_memory_zone::deallocate(ptr, _MemoryCategory::id());
    }

    template <class _MemoryCategory = _FST::default_memory_category, _FST::enable_if_t<_FST::is_memory_category<_MemoryCategory>::value, int> = 0>
    FST_NODISCARD inline void* aligned_allocate(size_t size, size_t alignment) noexcept
    {
        return _FST::default_memory_zone::aligned_allocate(size, alignment, _MemoryCategory::id());
    }

    template <class _MemoryCategory = _FST::default_memory_category, _FST::enable_if_t<_FST::is_memory_category<_MemoryCategory>::value, int> = 0>
    inline void aligned_deallocate(void* ptr) noexcept
    {
        _FST::default_memory_zone::aligned_deallocate(ptr, _MemoryCategory::id());
    }

FST_END_NAMESPACE
