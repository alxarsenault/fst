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
#include "fst/math.h"
#include "fst/memory.h"
#include "fst/traits.h"
#include "fst/tuple.h"

#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    namespace detail
    {
        template <class _T, size_t _Size, size_t _Alignment>
        struct basic_array;

        template <class _T, size_t _Size = _FST::dynamic_size, size_t _Alignment = alignof(_T)>
        class basic_array_range;

        template <typename _T, size_t _Size = _FST::dynamic_size, size_t _Alignment = alignof(_T), class _MemoryZone = _FST::default_memory_zone,
            class _MemoryCategory = _FST::default_memory_category>
        class basic_heap_array;
    } // namespace detail

    template <typename _T, size_t _Size, size_t _Alignment>
    using array = _FST::detail::basic_array<_T, _Size, _Alignment>;

    /// fixed_array_range
    template <typename _T, size_t _Size, size_t _Alignment = alignof(_T)>
    using fixed_array_range = _FST::detail::basic_array_range<_T, _Size, _Alignment>;

    /// array_range
    template <typename _T, size_t _Alignment = alignof(_T)>
    using array_range = _FST::detail::basic_array_range<_T, _FST::dynamic_size, _Alignment>;

    /// fixed_heap_array
    template <typename _T, size_t _Size, size_t _Alignment = alignof(_T), class _MemoryZone = _FST::default_memory_zone,
        class _MemoryCategory = _FST::default_memory_category>
    using fixed_heap_array = _FST::detail::basic_heap_array<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>;

    /// heap_array
    template <typename _T, size_t _Alignment = alignof(_T), class _MemoryZone = _FST::default_memory_zone, class _MemoryCategory = _FST::default_memory_category>
    using heap_array = _FST::detail::basic_heap_array<_T, _FST::dynamic_size, _Alignment, _MemoryZone, _MemoryCategory>;

    // is_array
    template <class _T>
    struct is_array : _FST::false_t
    {};

    template <class _T, size_t _Size, size_t _Alignment>
    struct is_array<_FST::array<_T, _Size, _Alignment>> : _FST::true_t
    {};

    // is_fixed_array_range
    template <class _T>
    struct is_fixed_array_range : _FST::false_t
    {};

    template <class _T, size_t _Size, size_t _Alignment>
    struct is_fixed_array_range<_FST::detail::basic_array_range<_T, _Size, _Alignment>> : _FST::bool_t<_Size != _FST::dynamic_size>
    {};

    // is_array_range
    template <class _T>
    struct is_array_range : _FST::false_t
    {};

    template <typename _T, size_t _Alignment>
    struct is_array_range<_FST::detail::basic_array_range<_T, _FST::dynamic_size, _Alignment>> : _FST::true_t
    {};

    // is_fixed_heap_array
    template <class _T>
    struct is_fixed_heap_array : _FST::false_t
    {};

    template <typename _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_fixed_heap_array<_FST::detail::basic_heap_array<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>> : _FST::bool_t<_Size != _FST::dynamic_size>
    {};

    // is_heap_array
    template <class _T>
    struct is_heap_array : _FST::false_t
    {};

    template <typename _T, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_heap_array<_FST::detail::basic_heap_array<_T, _FST::dynamic_size, _Alignment, _MemoryZone, _MemoryCategory>> : _FST::true_t
    {};

    // is_basic_array_range
    template <class _T>
    struct is_basic_array_range : _FST::false_t
    {};

    template <class _T, size_t _Size, size_t _Alignment>
    struct is_basic_array_range<_FST::detail::basic_array_range<_T, _Size, _Alignment>> : _FST::true_t
    {};

    // is_basic_heap_array
    template <class _T>
    struct is_basic_heap_array : _FST::false_t
    {};

    template <typename _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_basic_heap_array<_FST::detail::basic_heap_array<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>> : _FST::true_t
    {};

    /*template <class _T, size_t _Size, size_t _Alignment>
    struct array
    {
        FST_DECLARE_CONTAINER_TYPES(_T);
        using memory_zone_type = _FST::void_memory_zone;

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr memory_zone_type get_memory_zone() noexcept { return memory_zone_type{}; }

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type size() noexcept { return _Size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type alignment() noexcept { return _Alignment; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type max_size() noexcept { return _Size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type capacity() noexcept { return _Size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool empty() noexcept { return false; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_full() noexcept { return true; }

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_buffer_owner() noexcept { return true; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_data_owner() noexcept { return true; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_dynamically_allocted() noexcept { return false; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_size() noexcept { return true; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_capacity() noexcept { return true; }

        FST_DECLARE_CONTAINER_DATA(_data);

        alignas(_Alignment) _T _data[_Size];
    };

    template <class T, class... U>
    array(T, U...) -> array<T, 1 + sizeof...(U)>;*/

    namespace detail
    {

        template <class _T, size_t _Size, size_t _Alignment>
        struct basic_array
        {
            FST_DECLARE_CONTAINER_TYPES(_T);
            using memory_zone_type = _FST::void_memory_zone;

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr memory_zone_type get_memory_zone() noexcept { return memory_zone_type{}; }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type size() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type alignment() noexcept { return _Alignment; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type max_size() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type capacity() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool empty() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_full() noexcept { return true; }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_buffer_owner() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_data_owner() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_dynamically_allocted() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_size() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_capacity() noexcept { return true; }

            FST_DECLARE_CONTAINER_DATA(_data)

            alignas(_Alignment) _T _data[_Size];
        };

        template <class _T, size_t _Alignment>
        struct basic_array<_T, _FST::dynamic_size, _Alignment>
        {
            static_assert(_FST::always_false<_T>, "basic_array can't have a dynamic size");
        };

        template <class _T, size_t _Size, size_t _Alignment>
        class basic_array_range
        {
          public:
            static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");

            FST_DECLARE_CONTAINER_TYPES(_T);
            using memory_zone_type = _FST::void_memory_zone;

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr memory_zone_type get_memory_zone() noexcept { return memory_zone_type{}; }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type size() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type alignment() noexcept { return _Alignment; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type max_size() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type capacity() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _data == nullptr; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_full() const noexcept { return _data != nullptr; }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_buffer_owner() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_data_owner() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_dynamically_allocted() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_size() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_capacity() noexcept { return true; }

            FST_DECLARE_CONTAINER_DEFAULT_CTOR(basic_array_range);

            template <class _TT, size_t _TSize, size_t _TAlignment>
            FST_ALWAYS_INLINE constexpr basic_array_range(const basic_array_range<_TT, _TSize, _TAlignment>& r) noexcept
                : _data(r._data)
            {}

            template <class _Container,
                _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_FST::remove_cvref_t<_Container>, basic_array_range>, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_array_range(_Container& c) noexcept
                : _data(c.data())
            {
                fst_assert(!_data || (_FST::is_aligned(_data, _Alignment) && c.capacity() >= _Size), "Wrong alignment");
            }

            FST_ALWAYS_INLINE constexpr basic_array_range(pointer p) noexcept
                : _data(p)
            {
                fst_assert(!p || _FST::is_aligned(p, _Alignment), "Wrong alignment");
            }

            template <class _TT, size_t _TSize, size_t _TAlignment, _FST::enable_if_t<_FST::is_convertible_v<_TT*, _T*> && _TAlignment >= _Alignment, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_array_range& operator=(const basic_array_range<_TT, _TSize, _TAlignment>& other) noexcept
            {
                _data = other._data;
                return *this;
            }

            template <class _Container,
                _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_FST::remove_cvref_t<_Container>, basic_array_range>, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_array_range& operator=(_Container& other) noexcept
            {
                fst_assert(other.size() >= size());
                _data = other.data();
                return *this;
            }

            FST_DECLARE_CONTAINER_DATA(_data)

            FST_ALWAYS_INLINE constexpr void reset() noexcept { _data = nullptr; }

            FST_ALWAYS_INLINE constexpr void reset(pointer p) noexcept
            {
                fst_assert(!p || _FST::is_aligned(p, _Alignment), "Wrong alignment");
                _data = p;
            }

            pointer _data = nullptr;
        };

        //
        template <class _T, size_t _Alignment>
        class basic_array_range<_T, _FST::dynamic_size, _Alignment>
        {
          public:
            static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");

            FST_DECLARE_CONTAINER_TYPES(_T);
            using memory_zone_type = _FST::void_memory_zone;

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr memory_zone_type get_memory_zone() noexcept { return memory_zone_type{}; }

            FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type size() const noexcept { return _size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type alignment() noexcept { return _Alignment; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type max_size() const noexcept { return _size; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type capacity() const noexcept { return _size; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _data == nullptr || _size == 0; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_full() const noexcept { return _data != nullptr && _size != 0; }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_buffer_owner() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_data_owner() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_dynamically_allocted() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_size() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_capacity() noexcept { return false; }

            FST_DECLARE_CONTAINER_DEFAULT_CTOR(basic_array_range);

            template <class _TT, size_t _TSize, size_t _TAlignment, _FST::enable_if_t<_FST::is_convertible_v<_TT*, _T*> && _TAlignment >= _Alignment, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_array_range(const basic_array_range<_TT, _TSize, _TAlignment>& r) noexcept
                : _data(r._data)
                , _size(r.size())
            {}

            template <class _Container,
                _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_FST::remove_cvref_t<_Container>, basic_array_range>, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_array_range(_Container& c) noexcept
                : _data(c.data())
                , _size(c.size())
            {
                fst_assert(!_data || _FST::is_aligned(_data, _Alignment), "Wrong alignment");
            }

            FST_ALWAYS_INLINE constexpr basic_array_range(pointer p, size_type __size) noexcept
                : _data(p)
                , _size(__size)
            {
                fst_assert(p || (!p && __size == 0), "nullptr with size");
                fst_assert(!p || _FST::is_aligned(p, _Alignment), "Wrong alignment");
            }

            FST_ALWAYS_INLINE constexpr basic_array_range(_FST::nullptr_t, size_type) noexcept
                : _data(nullptr)
                , _size(0)
            {}

            template <class _TT, size_t _TSize, size_t _TAlignment, _FST::enable_if_t<_FST::is_convertible_v<_TT*, _T*> && _TAlignment >= _Alignment, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_array_range& operator=(const basic_array_range<_TT, _TSize, _TAlignment>& other) noexcept
            {
                _data = other._data;
                _size = other.size();
                return *this;
            }

            template <class _Container,
                _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_FST::remove_cvref_t<_Container>, basic_array_range>, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_array_range& operator=(_Container& other) noexcept
            {
                _data = other.data();
                _size = other.size();
                return *this;
            }

            FST_DECLARE_CONTAINER_DATA(_data)

            FST_ALWAYS_INLINE constexpr void reset() noexcept
            {
                _data = nullptr;
                _size = 0;
            }

            FST_ALWAYS_INLINE constexpr void reset(pointer p, size_type __size) noexcept
            {
                fst_assert(!p || _FST::is_aligned(p, _Alignment), "Wrong alignment");
                _data = p;
                _size = __size;
            }

            pointer _data = nullptr;
            size_type _size = 0;
        };

        //
        template <typename _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
        class basic_heap_array
        {
          public:
            static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");

            FST_DECLARE_CONTAINER_TYPES(_T);
            using memory_zone_type = _MemoryZone;

            FST_NODISCARD FST_ALWAYS_INLINE constexpr memory_zone_type get_memory_zone() const noexcept { return _data.second(); }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type size() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type alignment() noexcept { return _Alignment; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type max_size() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type capacity() noexcept { return _Size; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _data.first() == nullptr; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_full() const noexcept { return _data.first() != nullptr; }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_buffer_owner() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_data_owner() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_dynamically_allocted() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_size() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_capacity() noexcept { return true; }

            FST_DECLARE_CONTAINER_DATA(_data.first())

            template <class _Zone = _MemoryZone, _FST::enable_if_t<_FST::is_default_constructible_v<_Zone>, int> = 0>
            inline basic_heap_array() noexcept
                : _data(nullptr, memory_zone_type{})
            {
                _data.first() = (pointer) _data.second().aligned_allocate(size() * sizeof(value_type), _Alignment, _MemoryCategory::id());
                if (_data.first()) { _FST::default_construct_range(_data.first(), size()); }
            }

            template <class _Zone = _MemoryZone, _FST::enable_if_t<_FST::is_copy_constructible_v<_Zone>, int> = 0>
            inline basic_heap_array(const memory_zone_type& zone) noexcept
                : _data(nullptr, zone)
            {
                _data.first() = (pointer) _data.second().aligned_allocate(size() * sizeof(value_type), _Alignment, _MemoryCategory::id());
                if (_data.first()) { _FST::default_construct_range(_data.first(), size()); }
            }

            inline basic_heap_array(const basic_heap_array& other) noexcept
                : _data(nullptr, other.get_memory_zone())
            {
                _data.first() = (pointer) _data.second().aligned_allocate(size() * sizeof(value_type), _Alignment, _MemoryCategory::id());
                if (_data.first()) { _FST::copy_construct_range(_data.first(), other.data(), size()); }
            }

            inline basic_heap_array(basic_heap_array&& d) noexcept
                : _data(_FST::move(d._data))
            {
                d._data.first() = nullptr;
            }

            template <class _Container,
                _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_FST::remove_cvref_t<_Container>, basic_heap_array>, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_heap_array(const _Container& other) noexcept
                : _data(nullptr, memory_zone_type{})
            {
                fst_assert(other.size() >= size());

                _data.first() = (pointer) _data.second().aligned_allocate(size() * sizeof(value_type), _Alignment, _MemoryCategory::id());
                if (_data.first()) { _FST::copy_construct_range(_data.first(), other.data(), size()); }
            }

            inline ~basic_heap_array() noexcept { reset(); }

            inline basic_heap_array& operator=(const basic_heap_array& other) noexcept
            {
                reset();

                if (!other.data()) { return *this; }

                _data.first() = (pointer) _data.second().aligned_allocate(other.size() * sizeof(value_type), _Alignment, _MemoryCategory::id());
                if (_data.first()) { _FST::copy_construct_range(_data.first(), other.data(), size()); }

                return *this;
            }

            inline basic_heap_array& operator=(basic_heap_array&& other) noexcept
            {
                reset();
                _data.first() = other._data.first();
                other._data.first() = nullptr;
                return *this;
            }

            template <class _Container,
                _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_FST::remove_cvref_t<_Container>, basic_heap_array>, int> = 0>
            inline basic_heap_array& operator=(const _Container& other) noexcept
            {

                fst_assert(other.size() >= size());

                reset();

                if (!other.data()) { return *this; }

                _data.first() = (pointer) _data.second().aligned_allocate(other.size() * sizeof(value_type), _Alignment, _MemoryCategory::id());
                if (_data.first()) { _FST::copy_construct_range(_data.first(), other.data(), size()); }

                return *this;
            }

            inline void reset() noexcept
            {
                if (_data.first())
                {
                    if constexpr (!_FST::is_trivially_destructible_v<value_type>)
                    {
                        pointer __data = _data.first();

                        for (size_type i = 0; i < size(); i++)
                        {
                            __data[i].~value_type();
                        }
                    }

                    _data.second().aligned_deallocate((void*) _data.first(), _MemoryCategory::id());
                    _data.first() = nullptr;
                }
            }
            _FST::pair<pointer, memory_zone_type> _data;
        };

        template <typename _T, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
        class basic_heap_array<_T, _FST::dynamic_size, _Alignment, _MemoryZone, _MemoryCategory>
        {
          public:
            static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");

            FST_DECLARE_CONTAINER_TYPES(_T);
            using memory_zone_type = _MemoryZone;

            FST_NODISCARD FST_ALWAYS_INLINE constexpr memory_zone_type get_memory_zone() const noexcept { return _data.second(); }

            FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type size() const noexcept { return _size; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_type alignment() noexcept { return _Alignment; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type max_size() const noexcept { return _size; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type capacity() const noexcept { return _size; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _data.first() == nullptr || _size == 0; }
            FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_full() const noexcept { return _data.first() != nullptr && _size != 0; }

            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_buffer_owner() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_data_owner() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_dynamically_allocted() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_size() noexcept { return true; }
            FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_fixed_capacity() noexcept { return true; }

            FST_DECLARE_CONTAINER_DATA(_data.first())

            template <class _Zone = memory_zone_type, _FST::enable_if_t<_FST::is_default_constructible_v<_Zone>, int> = 0>
            inline basic_heap_array() noexcept
                : _data(nullptr, memory_zone_type{})
                , _size(0)
            {}

            template <class _Zone = memory_zone_type, _FST::enable_if_t<_FST::is_copy_constructible_v<_Zone>, int> = 0>
            inline basic_heap_array(const memory_zone_type& zone) noexcept
                : _data(nullptr, zone)
                , _size(0)
            {}

            inline basic_heap_array(size_t __size, const memory_zone_type& zone) noexcept
                : _data(nullptr, zone)
                , _size(0)
            {
                _data.first() = (pointer) _data.second().aligned_allocate(__size * sizeof(value_type), _Alignment, _MemoryCategory::id());

                if (_data.first())
                {
                    _size = __size;

                    _FST::default_construct_range(_data.first(), size());
                }
            }

            template <class _Zone = memory_zone_type, _FST::enable_if_t<_FST::is_default_constructible_v<_Zone>, int> = 0>
            inline basic_heap_array(size_t __size) noexcept
                : basic_heap_array(__size, memory_zone_type{})
            {}

            inline basic_heap_array(const basic_heap_array& other) noexcept
                : _data(nullptr, other.get_memory_zone())
                , _size(0)
            {
                _data.first() = (pointer) _data.second().aligned_allocate(other.size() * sizeof(value_type), _Alignment, _MemoryCategory::id());

                if (_data.first())
                {
                    _FST::copy_construct_range(_data.first(), other.data(), other.size());
                    _size = other.size();
                }
            }

            inline basic_heap_array(basic_heap_array&& other) noexcept
                : _data(_FST::move(other._data))
                , _size(other.size())
            {
                other._data.first() = nullptr;
                other._size = 0;
            }

            template <class _Container, _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_Container, basic_heap_array>, int> = 0>
            FST_ALWAYS_INLINE constexpr basic_heap_array(const _Container& other) noexcept
                : _data(nullptr, memory_zone_type{})
                , _size(0)
            {
                _data.first() = (pointer) _data.second().aligned_allocate(other.size() * sizeof(value_type), _Alignment, _MemoryCategory::id());

                if (_data.first())
                {
                    _FST::copy_construct_range(_data.first(), other.data(), other.size());
                    _size = other.size();
                }
            }

            //basic_heap_array_base() noexcept = default;

            /*inline basic_heap_array(const basic_heap_array& buf) noexcept
            : _MemoryZone((const _MemoryZone&) buf)
        {
            _data = nullptr;
            _size = 0;
            if (!buf._data) { return; }

            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), _Alignment, _MemoryCategory::id());
            if (_data)
            {
                _size = buf.size();
                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
            }
        }

        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory, size_t _OtherAlignment>
        inline basic_heap_array_base(const basic_heap_array_base<_T, _OtherSize, _OtherAlignment, _OtherMemoryZone, _OtherMemoryCategory>& buf) noexcept
            : basic_heap_array_base()
        {
            if (!buf._data) { return; }

            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), _Alignment, _MemoryCategory::id());
            if (_data)
            {
                _size = buf.size();
                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
            }
        }*/

            /*inline basic_heap_array_base(basic_heap_array_base&& d) noexcept
            : _data(d._data)
            , _size(d._size)
        {
            d._data = nullptr;
            d._size = 0;
        }*/

            /*template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory, size_t _OtherAlignment>
        inline basic_heap_array_base(basic_heap_array_base<_T, _OtherSize, _OtherAlignment, _OtherMemoryZone, _OtherMemoryCategory>&& buf) noexcept
            : basic_heap_array_base()
        {
            if (!buf._data) { return; }

            if constexpr (_FST::is_same_v<_MemoryZone, _OtherMemoryZone>)
            {
                _MemoryZone::template move_allocation<_OtherMemoryCategory, _MemoryCategory>(buf._data);
                _data = buf._data;
                _size = buf._size;
                buf._data = nullptr;
                buf._size = 0;
            }
            else
            {
                _data = (_T*) _MemoryZone::template aligned_allocate<_MemoryCategory>(buf.size() * sizeof(_T), _Alignment);
                if (_data)
                {
                    _size = buf.size();
                    _FST::memcpy(_data, buf._data, _size * sizeof(_T));
                }

                buf.reset();
            }
        }*/

            inline ~basic_heap_array() noexcept { reset(); }

            /*inline basic_heap_array& operator=(const basic_heap_array& buf) noexcept
        {
            reset();

            if (!buf._data) { return *this; }

            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), _Alignment, _MemoryCategory::id());

            if (_data)
            {
                _size = buf.size();
                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
            }

            return *this;
        }*/

            inline basic_heap_array& operator=(const basic_heap_array& other) noexcept
            {
                reset();

                if (!other.data()) { return *this; }

                _data.first() = (pointer) _data.second().aligned_allocate(other.size() * sizeof(value_type), _Alignment, _MemoryCategory::id());

                if (_data.first())
                {
                    _FST::copy_construct_range(_data.first(), other.data(), other.size());
                    _size = other.size();
                }

                return *this;
            }

            inline basic_heap_array& operator=(basic_heap_array&& other) noexcept
            {
                reset();
                _data.first() = other._data.first();
                _size = other._size;
                other._data.first() = nullptr;
                other._size = 0;
                return *this;
            }

            template <class _Container, _FST::enable_if_t<_FST::is_container_v<_Container> && _FST::is_different_v<_Container, basic_heap_array>, int> = 0>
            inline basic_heap_array& operator=(const _Container& other) noexcept
            {
                reset();

                if (!other.data()) { return *this; }

                _data.first() = (pointer) _data.second().aligned_allocate(other.size() * sizeof(value_type), _Alignment, _MemoryCategory::id());

                if (_data.first())
                {
                    _FST::copy_construct_range(_data.first(), other.data(), other.size());
                    _size = other.size();
                }

                return *this;
            }

            inline void reset() noexcept
            {
                if (_data.first())
                {
                    if constexpr (!_FST::is_trivially_destructible_v<value_type>)
                    {
                        pointer __data = _data.first();

                        for (size_type i = 0; i < size(); i++)
                        {
                            __data[i].~value_type();
                        }
                    }

                    _data.second().aligned_deallocate((void*) _data.first(), _MemoryCategory::id());
                    _data.first() = nullptr;
                    _size = 0;
                }
            }

            inline void reset(size_type new_size) noexcept
            {
                if (_size == new_size) return;

                reset();

                _data.first() = (pointer) _data.second().aligned_allocate(new_size * sizeof(value_type), _Alignment, _MemoryCategory::id());

                if (_data.first())
                {
                    _size = new_size;
                    _FST::default_construct_range(_data.first(), size());
                }
            }

            _FST::pair<pointer, memory_zone_type> _data;
            size_type _size = 0;
        };
    } // namespace detail

FST_END_NAMESPACE
