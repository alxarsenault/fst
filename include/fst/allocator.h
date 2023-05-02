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
#include "fst/memory.h"

FST_BEGIN_NAMESPACE

    ///
    class allocator_proxy
    {
      public:
        using allocate_callback = void* (*) (size_t, void*);
        using deallocate_callback = void (*)(void*, void*);

        inline allocator_proxy(void* data, allocate_callback alloc, deallocate_callback dealloc, size_t type_size) noexcept
            : _data(data)
            , _alloc_cb(alloc)
            , _dealloc_cb(dealloc)
            , _type_size(type_size)
        {}

        template <class _Allocator>
        inline allocator_proxy(_Allocator* a)
            : _data((void*) a)
            , _alloc_cb([](size_t s, void* content) { return (void*) reinterpret_cast<_Allocator*>(content)->allocate(s); })
            , _dealloc_cb([](void* ptr, void* content) { reinterpret_cast<_Allocator*>(content)->deallocate((typename _Allocator::value_type*) ptr, 0); })
            , _type_size(sizeof(typename _Allocator::value_type))
        {}

        FST_NODISCARD inline void* allocate(size_t size) const noexcept { return _alloc_cb(size, _data); }

        inline void deallocate(void* ptr) const noexcept { _dealloc_cb(ptr, _data); }

      private:
        void* _data;
        allocate_callback _alloc_cb;
        deallocate_callback _dealloc_cb;
        size_t _type_size;
    };

    ///
    template <class _Allocator>
    inline allocator_proxy make_allocator_proxy(_Allocator * a)
    {
        return allocator_proxy{ (void*) a, [](size_t s, void* content) { return (void*) reinterpret_cast<_Allocator*>(content)->allocate(s); },
            [](void* ptr, void* content) { reinterpret_cast<_Allocator*>(content)->deallocate((typename _Allocator::value_type*) ptr, 0); },
            sizeof(typename _Allocator::value_type) };
    }

    // memory_zone_allocator
    // non-static memory zone
    template <class T, class _MemoryZone, class _MemoryCategory>
    class memory_zone_allocator<T, _MemoryZone, _MemoryCategory,
        __fst::enable_if_t<!__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value>>
    {
      public:
        static_assert(!__fst::is_const<T>::value, "The C++ Standard forbids containers of const elements because "
                                                 "allocator<const T> is ill-formed.");

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using propagate_on_container_move_assignment = __fst::true_t;
        using is_always_equal = __fst::false_t;

        template <class U>
        struct rebind
        {
            using other = memory_zone_allocator<U, _MemoryZone, _MemoryCategory>;
        };

        static constexpr size_t alignment = 16 > alignof(T) ? 16 : alignof(T);
        static constexpr size_t max_count = static_cast<size_t>(-1) / sizeof(T);

        FST_NODISCARD inline constexpr T* address(T& t) const noexcept { return __fst::addressof(t); }

        FST_NODISCARD inline constexpr const T* address(const T& t) const noexcept { return __fst::addressof(t); }

        constexpr memory_zone_allocator() noexcept = default;
        constexpr memory_zone_allocator(const memory_zone_allocator&) noexcept = default;
        constexpr memory_zone_allocator(memory_zone_allocator&&) noexcept = default;

        template <class U>
        inline constexpr memory_zone_allocator(const memory_zone_allocator<U, _MemoryZone, _MemoryCategory>&) noexcept
        {}

        template <class... Args, __fst::enable_if_t<__fst::is_constructible_v<_MemoryZone, Args...>, int> = 0>
        inline memory_zone_allocator(Args&&... args) noexcept
            : _zone(__fst::forward<Args>(args)...)
        {}

        inline ~memory_zone_allocator() noexcept = default;

        inline constexpr memory_zone_allocator& operator=(const memory_zone_allocator&) noexcept = default;
        inline constexpr memory_zone_allocator& operator=(memory_zone_allocator&&) noexcept = default;

        FST_NODISCARD inline T* allocate(const size_t count) noexcept
        {
            fst_assert(count < max_count, "wrong allocation size");
            return static_cast<T*>(_zone.aligned_allocate(sizeof(T) * count, alignment, _MemoryCategory::id()));
        }

        inline void deallocate(T* const ptr) noexcept { _zone.aligned_deallocate(ptr, _MemoryCategory::id()); }

        inline void deallocate(T* const ptr, const size_t) noexcept { _zone.aligned_deallocate(ptr, _MemoryCategory::id()); }

        FST_NODISCARD inline allocator_proxy proxy() noexcept { return make_allocator_proxy(this); }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator==(
            const memory_zone_allocator& lhs, const __fst::memory_zone_allocator<U, _MemoryZone, _MemoryCategory>& rhs) noexcept
        {
            return lhs == rhs;
        }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator!=(
            const memory_zone_allocator& lhs, const __fst::memory_zone_allocator<U, _MemoryZone, _MemoryCategory>& rhs) noexcept
        {
            return lhs != rhs;
        }

      private:
        _MemoryZone _zone;
    };

    // memory_zone_allocator
    // static memory zone
    template <class T, class _MemoryZone, class _MemoryCategory>
    class memory_zone_allocator<T, _MemoryZone, _MemoryCategory,
        __fst::enable_if_t<__fst::is_static_memory_zone<_MemoryZone>::value && __fst::is_memory_category<_MemoryCategory>::value>>
    {
      public:
        static_assert(!__fst::is_const<T>::value, "The C++ Standard forbids containers of const elements because "
                                                 "allocator<const T> is ill-formed.");

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using propagate_on_container_move_assignment = __fst::true_t;
        using is_always_equal = __fst::true_t;

        template <class U>
        struct rebind
        {
            using other = memory_zone_allocator<U, _MemoryZone, _MemoryCategory>;
        };

        static constexpr size_t alignment = 16 > alignof(T) ? 16 : alignof(T);
        static constexpr size_t max_count = static_cast<size_t>(-1) / sizeof(T);

        FST_NODISCARD inline constexpr T* address(T& t) const noexcept { return __fst::addressof(t); }

        FST_NODISCARD inline constexpr const T* address(const T& t) const noexcept { return __fst::addressof(t); }

        constexpr memory_zone_allocator() noexcept = default;
        constexpr memory_zone_allocator(const memory_zone_allocator&) noexcept = default;
        constexpr memory_zone_allocator(memory_zone_allocator&&) noexcept = default;

        template <class U>
        inline constexpr memory_zone_allocator(const memory_zone_allocator<U, _MemoryZone, _MemoryCategory>&) noexcept
        {}

        inline ~memory_zone_allocator() noexcept = default;

        inline constexpr memory_zone_allocator& operator=(const memory_zone_allocator&) noexcept = default;
        inline constexpr memory_zone_allocator& operator=(memory_zone_allocator&&) noexcept = default;

        FST_NODISCARD inline T* allocate(const size_t count) noexcept
        {
            fst_assert(count < max_count, "wrong allocation size");
            return static_cast<T*>(_MemoryZone::aligned_allocate(sizeof(T) * count, alignment, _MemoryCategory::id()));
        }

        inline void deallocate(T* const ptr) noexcept { _MemoryZone::aligned_deallocate(ptr, _MemoryCategory::id()); }

        inline void deallocate(T* const ptr, const size_t) noexcept { _MemoryZone::aligned_deallocate(ptr, _MemoryCategory::id()); }

        FST_NODISCARD inline allocator_proxy proxy() noexcept { return make_allocator_proxy(this); }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator==(memory_zone_allocator, __fst::memory_zone_allocator<U, _MemoryZone, _MemoryCategory>) noexcept
        {
            return true;
        }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator!=(memory_zone_allocator, __fst::memory_zone_allocator<U, _MemoryZone, _MemoryCategory>) noexcept
        {
            return false;
        }
    };

    // memory_zone_proxy
    /*template <class T, class _MemoryZone, class _MemoryCategory>
    class memory_zone_allocator<T, _MemoryZone, _MemoryCategory,
        __fst::enable_if_t<__fst::is_memory_zone_proxy_v<_MemoryZone> && __fst::is_memory_category<_MemoryCategory>::value>>
    {
      public:
        static_assert(!__fst::is_const<T>::value, "The C++ Standard forbids containers of const elements because "
                                                 "allocator<const T> is ill-formed.");

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using propagate_on_container_move_assignment = __fst::true_t;
        using is_always_equal = __fst::false_t;

        template <class U>
        struct rebind
        {
            using other = memory_zone_allocator<U, _MemoryZone, _MemoryCategory>;
        };

        static constexpr size_t alignment = 16 > alignof(T) ? 16 : alignof(T);
        static constexpr size_t max_count = static_cast<size_t>(-1) / sizeof(T);

        FST_NODISCARD inline constexpr T* address(T& t) const noexcept { return __fst::addressof(t); }

        FST_NODISCARD inline constexpr const T* address(const T& t) const noexcept { return __fst::addressof(t); }

        constexpr memory_zone_allocator() noexcept = default;
        constexpr memory_zone_allocator(const memory_zone_allocator&) noexcept = default;
        constexpr memory_zone_allocator(memory_zone_allocator&&) noexcept = default;

        template <class U>
        inline constexpr memory_zone_allocator(const memory_zone_allocator<U, _MemoryZone, _MemoryCategory>&) noexcept
        {}

        inline memory_zone_allocator(__fst::memory_zone_proxy zone) noexcept
            : _zone(zone)
        {}

        inline ~memory_zone_allocator() noexcept = default;

        inline constexpr memory_zone_allocator& operator=(const memory_zone_allocator&) noexcept = default;
        inline constexpr memory_zone_allocator& operator=(memory_zone_allocator&&) noexcept = default;

        FST_NODISCARD inline T* allocate(const size_t count) noexcept
        {
            fst_assert(count < max_count, "wrong allocation size");
            return static_cast<T*>(_zone.aligned_allocate(sizeof(T) * count, alignment, _MemoryCategory::id()));
        }

        inline void deallocate(T* const ptr) noexcept { _zone.aligned_deallocate(ptr, _MemoryCategory::id()); }

        inline void deallocate(T* const ptr, const size_t) noexcept { _zone.aligned_deallocate(ptr, _MemoryCategory::id()); }

        FST_NODISCARD inline allocator_proxy proxy() noexcept { return make_allocator_proxy(this); }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator==(
            const memory_zone_allocator& lhs, const __fst::memory_zone_allocator<U, _MemoryZone, _MemoryCategory>& rhs) noexcept
        {
            return lhs._zone.id() == rhs._zone.id();
        }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator!=(
            const memory_zone_allocator& lhs, const __fst::memory_zone_allocator<U, _MemoryZone, _MemoryCategory>& rhs) noexcept
        {
            return lhs._zone.id() != rhs._zone.id();
        }

      private:
        __fst::memory_zone_proxy _zone;
    };*/

    template <class T, class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    using allocator = __fst::memory_zone_allocator<T, _MemoryZone, _MemoryCategory>;

    template <class T, class _MemoryCategory = __fst::default_memory_category>
    using default_allocator = __fst::memory_zone_allocator<T, __fst::default_memory_zone, _MemoryCategory>;

    template <class T, class _MemoryCategory = __fst::dsp_memory_category>
    using simd_allocator = __fst::memory_zone_allocator<T, __fst::simd_memory_zone, _MemoryCategory>;
FST_END_NAMESPACE

//
/*template <class T>
    class memory_allocator
    {
      public:
        static_assert(!__fst::is_const<T>::value, "The C++ Standard forbids containers of const elements because "
                                                 "allocator<const T> is ill-formed.");

        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using propagate_on_container_move_assignment = __fst::true_t;
        using is_always_equal = __fst::false_t;

        template <class U>
        struct rebind
        {
            using other = memory_allocator<U>;
        };

        static constexpr size_t alignment = 16 > alignof(T) ? 16 : alignof(T);
        static constexpr size_t max_count = static_cast<size_t>(-1) / sizeof(T);

        FST_NODISCARD inline constexpr T* address(T& t) const noexcept { return __fst::addressof(t); }

        FST_NODISCARD inline constexpr const T* address(const T& t) const noexcept { return __fst::addressof(t); }

        constexpr memory_allocator() noexcept = default;
        constexpr memory_allocator(const memory_allocator&) noexcept = default;
        constexpr memory_allocator(memory_allocator&&) noexcept = default;

        template <class U>
        inline constexpr memory_allocator(const memory_allocator<U>& other) noexcept
            : _zone(other._zone)
            , _mid(other._mid)
        {}

        inline memory_allocator(__fst::memory_zone_proxy z, __fst::memory_category_id mid) noexcept
            : _zone(z)
            , _mid(mid)
        {}

        template <class _Zone, __fst::enable_if_t<__fst::is_memory_zone<_Zone>::value, int> = 0>
        inline memory_allocator(_Zone& z, __fst::memory_category_id mid) noexcept
            : _zone(z.proxy())
            , _mid(mid)
        {}

        inline ~memory_allocator() noexcept = default;

        inline constexpr memory_allocator& operator=(const memory_allocator&) noexcept = default;
        inline constexpr memory_allocator& operator=(memory_allocator&&) noexcept = default;

        FST_NODISCARD inline T* allocate(const size_t count) noexcept
        {
            fst_assert(count < max_count, "wrong allocation size");
            return static_cast<T*>(_zone.aligned_allocate(sizeof(T) * count, alignment, _mid));
        }

        inline void deallocate(T* const ptr) noexcept { _zone.aligned_deallocate(ptr, _mid); }

        inline void deallocate(T* const ptr, const size_t) noexcept { _zone.aligned_deallocate(ptr, _mid); }

        FST_NODISCARD inline allocator_proxy proxy() noexcept { return make_allocator_proxy(this); }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator==(const memory_allocator& lhs, const __fst::memory_allocator<U>& rhs) noexcept
        {
            return lhs._zone.zone_id() == rhs._zone.zone_id() && lhs._mid == rhs.mid;
        }

        template <class U>
        FST_NODISCARD friend inline constexpr bool operator!=(const memory_allocator& lhs, const __fst::memory_allocator<U>& rhs) noexcept
        {
            return lhs._zone.zone_id() != rhs._zone.zone_id() || lhs._mid != rhs.mid;
        }

      private:
        __fst::memory_zone_proxy _zone;
        __fst::memory_category_id _mid;
    };*/
