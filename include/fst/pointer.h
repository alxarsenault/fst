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
#include "fst/traits.h"
#include "fst/tuple.h"

FST_BEGIN_NAMESPACE

    struct make_tag
    {};
    FST_INLINE_VAR constexpr make_tag make_t = {};

    /// packed_pointer
    class packed_pointer
    {
      public:
        packed_pointer() noexcept = default;
        packed_pointer(const packed_pointer&) noexcept = default;
        packed_pointer(packed_pointer&&) noexcept = default;

        template <typename TPointer, typename TInt>
        FST_ALWAYS_INLINE packed_pointer(TPointer ptr, TInt value) noexcept
        {
            set_pointer(ptr);
            set_int(value);
        }

        packed_pointer& operator=(const packed_pointer&) noexcept = default;
        packed_pointer& operator=(packed_pointer&&) noexcept = default;

        template <typename T>
        FST_NODISCARD FST_ALWAYS_INLINE T get_int() const noexcept
        {
            return static_cast<T>(_data & k_int_mask);
        }

        template <typename T>
        FST_NODISCARD FST_ALWAYS_INLINE T get_pointer() const noexcept
        {
            return static_cast<T>(reinterpret_cast<void*>(_data & k_pointer_mask));
        }

        template <typename T>
        FST_ALWAYS_INLINE void set_pointer(T ptr) noexcept
        {
            intptr_t ptrWord = reinterpret_cast<intptr_t>((void*) (ptr));
            fst_assert((ptrWord & ~k_pointer_mask) == 0, "Pointer is not sufficiently aligned");

            // Preserve all low bits, just update the pointer.
            _data = ptrWord | (_data & ~k_pointer_mask);
        }

        template <typename T>
        FST_ALWAYS_INLINE void set_int(T value) noexcept
        {
            intptr_t intWord = static_cast<intptr_t>(value);
            fst_assert((intWord & ~k_int_mask) == 0, "Integer too large for field");

            // Preserve all bits other than the ones we are updating.
            _data = (_data & ~k_int_mask) | intWord;
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool operator==(packed_pointer other) const noexcept { return _data == other._data; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool operator!=(packed_pointer other) const noexcept { return _data != other._data; }

      private:
        intptr_t _data = 0;

        static constexpr uintptr_t k_pointer_mask = (uintptr_t) -4;
        static constexpr uintptr_t k_int_mask = (uintptr_t) 3;
    };

    ///
    template <class _T, class _MemoryZone = _FST::default_memory_zone, class _MemoryCategory = _FST::default_memory_category>
    class unique_ptr
    {
      public:
        using element_type = _T;
        using pointer = _T*;
        using memory_zone_type = _MemoryZone;

        template <class _Zone>
        using enable_if_zone_default_constructible_t = _FST::enable_if_t<!_FST::is_pointer_v<_Zone> && _FST::is_default_constructible_v<_Zone>, int>;

        template <class _Zone>
        using enable_if_zone_copy_constructible_t = _FST::enable_if_t<_FST::is_copy_constructible_v<_Zone>, int>;

        template <class _Zone>
        using enable_if_zone_move_constructible_t = _FST::enable_if_t<!_FST::is_reference_v<_Zone> && _FST::is_move_constructible_v<_Zone>, int>;

        template <class... _Args>
        inline constexpr unique_ptr(make_tag, _Args&&... args)
            : _data{ nullptr, memory_zone_type{} }
        {
            _T* ptr = (_T*) memory_zone_type::aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
            fst_assert(ptr, "allocation failed");
            _data.first() = fst_placement_new(ptr) _T(_FST::forward<_Args>(args)...);
        }

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr() noexcept
            : _data{ nullptr, memory_zone_type{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr(_FST::nullptr_t) noexcept
            : _data{ nullptr, memory_zone_type{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline explicit unique_ptr(pointer ptr) noexcept
            : _data{ ptr, memory_zone_type{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_copy_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr(pointer ptr, const memory_zone_type& mem_zone) noexcept
            : _data{ ptr, mem_zone }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_move_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr(pointer ptr, memory_zone_type&& mem_zone) noexcept
            : _data{ ptr, _FST::move(mem_zone) }
        {}

        template <class _Dx2 = memory_zone_type,
            _FST::enable_if_t<_FST::conjunction_v<_FST::is_reference<_Dx2>, _FST::is_constructible<_Dx2, _FST::remove_reference_t<_Dx2>>>, int> = 0>
        unique_ptr(pointer, _FST::remove_reference_t<memory_zone_type>&&) = delete;

        template <class _Zone = memory_zone_type, _FST::enable_if_t<_FST::is_move_constructible_v<_Zone>, int> = 0>
        unique_ptr(unique_ptr&& _Right) noexcept
            : _data{ _Right.get(), _FST::forward<memory_zone_type>(_Right.get_memory_zone()) }
        {
            (void) _Right.release();
        }

        template <class _Ty2, class _Dx2,
            _FST::enable_if_t<
                _FST::conjunction_v<_FST::negation<_FST::is_c_array<_Ty2>>, _FST::is_convertible<typename unique_ptr<_Ty2, _Dx2>::pointer, pointer>,
                    _FST::conditional_t<_FST::is_reference_v<memory_zone_type>, _FST::is_same<_Dx2, memory_zone_type>, _FST::is_convertible<_Dx2, memory_zone_type>>>,
                int>
            = 0>
        unique_ptr(unique_ptr<_Ty2, _Dx2>&& _Right) noexcept
            : _data{ _Right.get(), _FST::forward<_Dx2>(_Right.get_memory_zone()) }
        {

            _Right.release();
        }

        unique_ptr(const unique_ptr&) = delete;

        template <class... _Args>
        static inline unique_ptr make(_Args&&... args) noexcept
        {
            _T* ptr = (_T*) memory_zone_type::aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
            fst_assert(ptr, "allocation failed");

            ptr = fst_placement_new(ptr) _T(_FST::forward<_Args>(args)...);
            return unique_ptr(ptr);
        }

        inline ~unique_ptr() noexcept
        {
            if (_data.first())
            {
                _data.first()->~element_type();
                _data.second().aligned_deallocate(_data.first(), _MemoryCategory::id());
            }
        }

        unique_ptr& operator=(const unique_ptr&) = delete;

        inline constexpr unique_ptr& operator=(_FST::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        /*template <class _Ty2, class _Dx2, class _C2,
            _FST::enable_if_t<_FST::conjunction_v<_FST::negation<_FST::is_c_array<_Ty2>>, _FST::is_assignable<memory_zone_type&, _Dx2>,
                                  _FST::is_convertible<typename unique_ptr<_Ty2, _Dx2, _C2>::pointer, pointer>>,
                int> = 0>
        inline unique_ptr& operator=(unique_ptr<_Ty2, _Dx2, _C2>&& _Right) noexcept
        {
            reset(_Right.release());
            _data.second() = _FST::forward<_Dx2>(_Right._data.second());
            return *this;
        }*/

        template <class _Zone = memory_zone_type, _FST::enable_if_t<_FST::is_move_assignable_v<_Zone>, int> = 0>
        unique_ptr& operator=(unique_ptr&& _Right) noexcept
        {
            if (this != _FST::addressof(_Right))
            {
                reset(_Right.get());
                _data.second() = _FST::forward<memory_zone_type>(_Right._data.second());
                (void) _Right.release();
            }
            return *this;
        }

        FST_NODISCARD memory_zone_type& get_memory_zone() noexcept { return _data.second(); }
        FST_NODISCARD const memory_zone_type& get_memory_zone() const noexcept { return _data.second(); }

        FST_NODISCARD _FST::add_lvalue_reference_t<_T> operator*() const noexcept { return *_data.first(); }

        FST_NODISCARD pointer operator->() const noexcept { return _data.first(); }

        FST_NODISCARD pointer get() const noexcept { return _data.first(); }

        FST_NODISCARD inline explicit operator bool() const noexcept { return static_cast<bool>(_data.first()); }

        FST_NODISCARD inline pointer release() noexcept { return _FST::exchange(_data.first(), nullptr); }

        inline void reset(pointer ptr = nullptr) noexcept
        {
            if (pointer old_ptr = _FST::exchange(_data.first(), ptr)) { _data.second().aligned_deallocate(old_ptr, _MemoryCategory::id()); }
        }

      private:
        _FST::pair<pointer, _MemoryZone> _data;
    };

    ///
    template <class _T, class _MemoryZone = _FST::default_memory_zone, class _MemoryCategory = _FST::default_memory_category, class... _Args,
        _FST::enable_if_t<_FST::conjunction_v<_FST::is_static_memory_zone<_MemoryZone>, _FST::is_memory_category<_MemoryCategory>>, int> = 0>
    inline _T* make(_Args && ... args) noexcept
    {
        _T* ptr = (_T*) _MemoryZone::aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
        fst_assert(ptr, "allocation failed");
        return fst_placement_new(ptr) _T(_FST::forward<_Args>(args)...);
    }

    ///
    template <class _T, class _MemoryZone, class _MemoryCategory = _FST::default_memory_category, class... _Args,
        _FST::enable_if_t<_FST::conjunction_v<_FST::is_memory_zone<_MemoryZone>, _FST::is_memory_category<_MemoryCategory>>, int> = 0>
    inline _T* make(_MemoryZone & mem_zone, _Args && ... args) noexcept
    {
        _T* ptr = (_T*) mem_zone.aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
        fst_assert(ptr, "allocation failed");
        return fst_placement_new(ptr) _T(_FST::forward<_Args>(args)...);
    }

    ///
    template <class _T, class _MemoryZone, class... _Args, _FST::enable_if_t<_FST::is_memory_zone<_MemoryZone>::value, int> = 0>
    inline _T* make(_MemoryZone & mem_zone, _FST::memory_category_id mid, _Args && ... args) noexcept
    {
        _T* ptr = (_T*) mem_zone.aligned_allocate(sizeof(_T), alignof(_T), mid);
        fst_assert(ptr, "allocation failed");
        return fst_placement_new(ptr) _T(_FST::forward<_Args>(args)...);
    }

    ///
    template <class _MemoryZone = _FST::default_memory_zone, class _MemoryCategory = _FST::default_memory_category, class _T,
        _FST::enable_if_t<_FST::conjunction_v<_FST::is_static_memory_zone<_MemoryZone>, _FST::is_memory_category<_MemoryCategory>>, int> = 0>
    inline void destroy(_T * ptr) noexcept
    {
        fst_assert(ptr, "tried to destroy nullptr");
        _MemoryZone::aligned_deallocate(ptr, _MemoryCategory::id());
    }

    ///
    template <class _MemoryCategory = _FST::default_memory_category, class _MemoryZone, class _T,
        _FST::enable_if_t<_FST::conjunction_v<_FST::is_memory_zone<_MemoryZone>, _FST::is_memory_category<_MemoryCategory>>, int> = 0>
    inline void destroy(_T * ptr, _MemoryZone & mem_zone) noexcept
    {
        fst_assert(ptr, "tried to destroy nullptr");
        mem_zone.aligned_deallocate(ptr, _MemoryCategory::id());
    }

    ///
    template <class _MemoryZone, class _T, _FST::enable_if_t<_FST::is_memory_zone<_MemoryZone>::value, int> = 0>
    inline void destroy(_T * ptr, _MemoryZone & mem_zone, _FST::memory_category_id mid) noexcept
    {
        fst_assert(ptr, "tried to destroy nullptr");
        mem_zone.aligned_deallocate(ptr, mid);
    }

FST_END_NAMESPACE
