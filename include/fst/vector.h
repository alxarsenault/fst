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
#include "fst/aligned_storage.h"
#include "fst/math.h"
#include "fst/memory.h"
#include "fst/traits.h"
#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    //template <class _T, size_t _Size, size_t _Alignment = alignof(_T)>
    //class vector_range_base
    //{
    //  public:
    //    static_assert(__fst::is_trivially_copyable_v<_T>, "_Alignment must be a power of two");

    //    // static_assert(__fst::is_power_of_two(_Size), "_Size must be a power of
    //    // two");
    //    static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");

    //    using memory_zone_type = __fst::void_memory_zone;
    //    FST_NODISCARD FST_ALWAYS_INLINE static constexpr memory_zone_type get_memory_zone() noexcept { return __fst::_global_void_memory_zone; }

    //    //constexpr vector_range_base() noexcept = default;

    //    constexpr vector_range_base() noexcept = default;
    //    constexpr vector_range_base(const vector_range_base&) noexcept = default;
    //    constexpr vector_range_base(vector_range_base&&) noexcept = default;

    //    constexpr vector_range_base& operator=(const vector_range_base&) noexcept = default;
    //    constexpr vector_range_base& operator=(vector_range_base&&) noexcept = default;

    //    template <class _Container, __fst::enable_if_t<__fst::is_iterable<_Container>::value && __fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr vector_range_base(_Container& c) noexcept
    //        : _data(c.data())
    //    {

    //        // static_assert(__fst::static_capacity_v<_Container> <= _Size, "Wrong
    //        // size");

    //        fst_assert(!_data || (__fst::is_aligned(_data, _Alignment) && c.capacity() >= _Size), "Wrong alignment");
    //    }

    //    template <class _Container, __fst::enable_if_t<__fst::is_iterable<_Container>::value && !__fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr vector_range_base(_Container& c) noexcept
    //        : _data(c.data())
    //    {

    //        // static_assert(__fst::static_capacity_v<_Container> <= _Size, "Wrong
    //        // size");

    //        fst_assert(!_data || (__fst::is_aligned(_data, _Alignment) && c.capacity() >= _Size), "Wrong alignment");
    //    }

    //    FST_ALWAYS_INLINE constexpr vector_range_base(_T* p) noexcept
    //        : _data(p)
    //    {
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //    }

    //    //constexpr vector_range_base(const vector_range_base&) noexcept = default;
    //    //constexpr vector_range_base& operator=(const vector_range_base&) noexcept = default;

    //    //template <class _Container, __fst::enable_if_t<__fst::is_iterable<_Container>::value && __fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    //constexpr vector_range_base& operator=(_Container& c) noexcept
    //    //    {
    //    //    //_data = c.data();
    //    //    //_capacity = c.capacity();
    //    //    //_size = c.size();
    //    //    return *this;
    //    //}

    //    //template <class _Container, __fst::enable_if_t<__fst::is_iterable<_Container>::value && !__fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    //constexpr vector_range_base& operator=(_Container& c) noexcept
    //    //    {

    //    //    return *this;
    //    //}

    //    FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
    //    FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return _Alignment; }
    //    FST_NODISCARD static constexpr size_t max_size() noexcept { return _Size; }
    //    FST_NODISCARD static constexpr size_t capacity() noexcept { return _Size; }
    //    FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }

    //    FST_ALWAYS_INLINE constexpr void reset() noexcept
    //    {
    //        _data = nullptr;
    //        _size = 0;
    //    }

    //    FST_ALWAYS_INLINE constexpr void reset(_T* p) noexcept
    //    {
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //        _data = p;
    //    }

    //    _T* _data = nullptr;
    //    size_t _size = 0;
    //};

    //template <class _T, size_t _Alignment>
    //class vector_range_base<_T, __fst::dynamic_size, _Alignment>
    //{
    //  public:
    //    static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");
    //    static_assert(__fst::is_trivially_copyable_v<_T>, "_Alignment must be a power of two");

    //    using memory_zone_type = __fst::void_memory_zone;
    //    FST_NODISCARD FST_ALWAYS_INLINE static constexpr memory_zone_type get_memory_zone() noexcept { return __fst::_global_void_memory_zone; }

    //    constexpr vector_range_base() noexcept = default;

    //    template <class _Container, __fst::enable_if_t<__fst::is_iterable<_Container>::value && !__fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr vector_range_base(_Container& c) noexcept
    //        : _data(c.data())
    //        , _capacity(c.capacity())
    //        , _size(0)
    //    {
    //        fst_assert(!_data || __fst::is_aligned(_data, _Alignment), "Wrong alignment");
    //    }

    //    template <class _Container, __fst::enable_if_t<__fst::is_iterable<_Container>::value && __fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr vector_range_base(_Container& c) noexcept
    //        : _data(c.data())
    //        , _capacity(c.capacity())
    //        , _size(0)
    //    {
    //        fst_assert(!_data || __fst::is_aligned(_data, _Alignment), "Wrong alignment");
    //    }

    //    FST_ALWAYS_INLINE constexpr vector_range_base(_T* p, size_t __capacity, size_t __size = 0) noexcept
    //        : _data(p)
    //        , _capacity(__capacity)
    //        , _size(__size)
    //    {
    //        fst_assert(p || (!p && __capacity == 0), "nullptr with size");
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //    }

    //    FST_ALWAYS_INLINE constexpr vector_range_base(__fst::nullptr_t, size_t) noexcept
    //        : _data(nullptr)
    //        , _capacity(0)
    //        , _size(0)
    //    {}

    //    template <class _TT, size_t _TSize, size_t _TAlignment, __fst::enable_if_t<__fst::is_convertible_v<_TT*, _T*>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr vector_range_base(const vector_range_base<_TT, _TSize, _TAlignment>& r) noexcept
    //        : _data(r._data)
    //        , _capacity(r.capacity())
    //        , _size(r.size())
    //    {}

    //    constexpr vector_range_base(const vector_range_base&) noexcept = default;
    //    constexpr vector_range_base& operator=(const vector_range_base&) noexcept = default;

    //    /*template <class _Container, __fst::enable_if_t<__fst::is_container_v<_Container> && __fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    constexpr vector_range_base& operator=(_Container& c) noexcept
    //    {
    //        _data = c.data();
    //        _capacity = c.capacity();
    //        _size = c.size();
    //        return *this;
    //    }

    //    template <class _Container, __fst::enable_if_t<__fst::is_container_v<_Container> && !__fst::is_base_of_v<vector_range_base, _Container>, int> = 0>
    //    constexpr vector_range_base& operator=(_Container& c) noexcept
    //    {
    //        _data = c.data();
    //        _capacity = c.capacity();
    //        _size = c.size();
    //        return *this;
    //    }*/

    //    FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
    //    FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return _Alignment; }
    //    FST_NODISCARD constexpr size_t max_size() const noexcept { return _capacity; }
    //    FST_NODISCARD constexpr size_t capacity() const noexcept { return _capacity; }
    //    FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return false; }

    //    FST_ALWAYS_INLINE constexpr void reset() noexcept
    //    {
    //        _data = nullptr;
    //        _size = 0;
    //        _capacity = 0;
    //    }

    //    FST_ALWAYS_INLINE constexpr void reset(_T* p, size_t __size) noexcept
    //    {
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //        _data = p;
    //        _size = 0;
    //        _capacity = __size;
    //    }

    //    _T* _data = nullptr;
    //    size_t _capacity = 0;
    //    size_t _size = 0;
    //};

    template <typename _T, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    class heap_vector_base : public _MemoryZone
    {
      public:
        // static_assert(_Size == __fst::dynamic_size, "should be dynamic");
        static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");

        using memory_zone_type = _MemoryZone;
        FST_NODISCARD FST_ALWAYS_INLINE memory_zone_type& get_memory_zone() const noexcept { return (memory_zone_type&) *this; }

        inline ~heap_vector_base() noexcept
        {
            if (_data) { _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id()); }
        }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t alignment() noexcept { return _Alignment; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t max_size() noexcept { return __fst::dynamic_size; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t capacity() const noexcept { return _capacity; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return true; }

        inline void reset()
        {
            if (_data)
            {
                if constexpr (!__fst::is_trivially_destructible_v<_T>)
                {
                    for (size_t i = 0; i < size(); i++)
                    {
                        _data[i].~_T();
                    }
                }

                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
            }

            _data = nullptr;
            _size = 0;
            _capacity = 0;
        }

        inline bool grow(size_t new_capacity) noexcept
        {
            _T* tmp_data = (_T*) _MemoryZone::aligned_allocate(new_capacity * sizeof(_T), _Alignment, _MemoryCategory::id());
            if (!tmp_data) { return false; }

            _capacity = new_capacity;

            if (_data)
            {
                __fst::relocate(tmp_data, _data, _size);
                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
            }

            _data = tmp_data;
            return true;
        }

        template <class _Mod>
        inline bool grow(size_t new_capacity, _Mod mod) noexcept
        {
            _T* tmp_data = (_T*) _MemoryZone::aligned_allocate(new_capacity * sizeof(_T), _Alignment, _MemoryCategory::id());
            if (!tmp_data) { return false; }

            _capacity = new_capacity;

            if (_data)
            {
                mod(tmp_data, _data, _size, new_capacity);
                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
            }

            _data = tmp_data;
            return true;
        }

        _T* _data = nullptr;
        size_t _size = 0;
        size_t _capacity = 0;
    };

    //
    //template <class _T, size_t _Size, class _ArrayType, size_t _Alignment = alignof(_T)>
    //class basic_vector : public __fst::contiguous_container_base<_T, _ArrayType>
    //{
    //  public:
    //    using base = __fst::contiguous_container_base<_T, _ArrayType>;
    //    using value_type = typename base::value_type;
    //    using size_type = typename base::size_type;
    //    using difference_type = typename base::difference_type;
    //    using pointer = typename base::pointer;
    //    using const_pointer = typename base::const_pointer;
    //    using reference = typename base::reference;
    //    using const_reference = typename base::const_reference;
    //    using iterator = typename base::iterator;
    //    using const_iterator = typename base::const_iterator;
    //    using memory_zone_type = typename base::memory_zone_type;

    //    using base::base;
    //    using base::operator=;
    //    using base::get_memory_zone;
    //    using base::back;
    //    using base::begin;
    //    using base::capacity;
    //    using base::cbegin;
    //    using base::cend;
    //    using base::data;
    //    using base::empty;
    //    using base::end;
    //    using base::front;
    //    using base::is_full;
    //    using base::is_resizable;
    //    using base::max_size;
    //    using base::reversed;
    //    using base::size;
    //    using base::operator[];
    //    //using base::operator=;
    //    using base::reset;

    //    constexpr basic_vector() noexcept = default;
    //    constexpr basic_vector(const basic_vector&) noexcept = default;
    //    constexpr basic_vector(basic_vector&&) noexcept = default;

    //    constexpr basic_vector& operator=(const basic_vector&) noexcept = default;
    //    constexpr basic_vector& operator=(basic_vector&&) noexcept = default;

    //    inline ~basic_vector() noexcept
    //    {
    //        if constexpr (!__fst::is_trivially_destructible_v<value_type>)
    //        {
    //            for (size_type i = 0; i < size(); i++)
    //            {
    //                (*this)[i].~value_type();
    //            }
    //        }
    //    }

    //    FST_ALWAYS_INLINE constexpr void clear() noexcept { base::_size = 0; }

    //    FST_ALWAYS_INLINE constexpr void push_back(const_reference value) noexcept
    //    {
    //        if (size() < capacity())
    //        {
    //            if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::copy_element((*this)[base::_size++], value); }
    //            else
    //            {
    //                fst_placement_new(this->data() + base::_size++)
    //                value_type(value);
    //            }

    //            return;
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            if (base::grow(__fst::next_power_of_two(capacity())))
    //            {
    //                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::copy_element((*this)[base::_size++], value); }
    //                else
    //                {
    //                    fst_placement_new(this->data() + base::_size++)
    //                    value_type(value);
    //                }

    //                return;
    //            }
    //        }

    //        fst_error("DSDS");
    //    }

    //    template <class U = value_type, __fst::enable_if_t<!__fst::is_trivial_cref_v<U>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr void push_back(U&& value) noexcept
    //    {
    //        if (size() < capacity())
    //        {
    //            if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[base::_size++], __fst::forward<U>(value)); }
    //            else
    //            {
    //                fst_placement_new(this->data() + base::_size++)
    //                value_type(__fst::forward<U>(value));
    //            }

    //            return;
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            if (base::grow(__fst::next_power_of_two(capacity())))
    //            {
    //                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[base::_size++], __fst::forward<U>(value)); }
    //                else
    //                {
    //                    fst_placement_new(this->data() + base::_size++)
    //                    value_type(__fst::forward<U>(value));
    //                }

    //                return;
    //            }
    //        }

    //        fst_error("DSDS");
    //    }

    //    /*template <class U, __fst::enable_if_t<__fst::is_same_v<U, value_type>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr void push_back(U&& value) noexcept
    //    {
    //        if (size() < capacity())
    //        {
    //            if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[base::_size++], __fst::forward<U>(value)); }
    //            else
    //            {
    //                fst_placement_new(this->data() + base::_size++) value_type(__fst::forward<U>(value));
    //            }

    //            return;
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            if (base::grow(__fst::next_power_of_two(capacity())))
    //            {
    //                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[base::_size++], __fst::forward<U>(value)); }
    //                else
    //                {
    //                    fst_placement_new(this->data() + base::_size++) value_type(__fst::forward<U>(value));
    //                }

    //                return;
    //            }
    //        }

    //        fst_error("DSDS");
    //    }*/

    //    template <typename... _Args>
    //    FST_ALWAYS_INLINE constexpr reference emplace_back(_Args&&... args) noexcept
    //    {
    //        // push_back(value_type(__fst::forward<_Args>(args)...));
    //        if (size() < capacity())
    //        {
    //            if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[base::_size++], __fst::forward<_Args>(args)...); }
    //            else
    //            {
    //                fst_placement_new(this->data() + base::_size++)
    //                value_type(__fst::forward<_Args>(args)...);
    //            }

    //            return this->back();
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            if (base::grow(__fst::next_power_of_two(capacity())))
    //            {
    //                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[base::_size++], __fst::forward<_Args>(args)...); }
    //                else
    //                {
    //                    fst_placement_new(this->data() + base::_size++)
    //                    value_type(__fst::forward<_Args>(args)...);
    //                }

    //                return this->back();
    //            }
    //        }

    //        fst_error("DSDS");
    //        return this->back();
    //    }

    //    inline constexpr iterator insert(const_iterator pos, const_reference value) noexcept
    //    {
    //        if (!base::_size || pos == cend())
    //        {
    //            push_back(value);
    //            return end() - 1;
    //        }

    //        if (size() < capacity())
    //        {
    //            const size_type index = __fst::pdistance(cbegin(), pos);

    //            if constexpr (__fst::is_trivially_copyable_v<_T>)
    //            {
    //                __fst::memmove(begin() + index + 1, begin() + index, (base::_size - index) * sizeof(value_type));
    //                __fst::copy_element((*this)[index], value);
    //            }
    //            else
    //            {
    //                fst_placement_new(end())
    //                value_type(__fst::move(back()));

    //                for (size_type i = size() - 1; i > index; i--)
    //                {
    //                    __fst::move_element((*this)[i], __fst::move((*this)[i - 1]));
    //                }

    //                __fst::copy_element((*this)[index], value);
    //            }

    //            base::_size++;
    //            return begin() + index;
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            const size_type index = __fst::pdistance(cbegin(), pos);

    //            if (!base::grow(__fst::next_power_of_two(capacity()),
    //                    [&](pointer ndata, pointer data, size_type size, FST_ATTRIBUTE_UNUSED size_type new_capacity)
    //                    {
    //                        __fst::relocate(ndata, data, index);
    //                        __fst::relocate(ndata + index + 1, data + index, size - index);
    //                        fst_placement_new(ndata + index)
    //                        value_type(value);
    //                    }))
    //            {

    //                fst_error("DSDS");
    //                return end();
    //            }

    //            base::_size++;

    //            return begin() + index;
    //        }
    //        else
    //        {
    //            fst_assert(base::_size != capacity(), "Can't push_back when fixed_vector is full.");
    //            //__fst::move_element((*this)[base::_size++], __fst::move(value));
    //            return end();
    //        }
    //    }

    //    template <class U = value_type, __fst::enable_if_t<!__fst::is_trivial_cref_v<U>, int> = 0>
    //    inline constexpr iterator insert(const_iterator pos, U&& value) noexcept
    //    {
    //        if (!base::_size || pos == cend())
    //        {
    //            push_back(__fst::move(value));
    //            return end() - 1;
    //        }

    //        if (size() < capacity())
    //        {
    //            const size_type index = __fst::pdistance(cbegin(), pos);

    //            if constexpr (__fst::is_trivially_copyable_v<_T>)
    //            {
    //                __fst::memmove(begin() + index + 1, begin() + index, (base::_size - index) * sizeof(value_type));
    //                __fst::copy_element((*this)[index], value);
    //            }
    //            else
    //            {
    //                fst_placement_new(end())
    //                value_type(__fst::move(back()));

    //                for (size_type i = size() - 1; i > index; i--)
    //                {
    //                    __fst::move_element((*this)[i], __fst::move((*this)[i - 1]));
    //                }

    //                __fst::move_element((*this)[index], __fst::move(value));
    //            }

    //            base::_size++;
    //            return begin() + index;
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            const size_type index = __fst::pdistance(cbegin(), pos);

    //            if (!base::grow(__fst::next_power_of_two(capacity()),
    //                    [&](pointer ndata, pointer data, size_type size, FST_ATTRIBUTE_UNUSED size_type new_capacity)
    //                    {
    //                        __fst::relocate(ndata, data, index);
    //                        __fst::relocate(ndata + index + 1, data + index, size - index);
    //                        fst_placement_new(ndata + index)
    //                        value_type(__fst::move(value));
    //                    }))
    //            {

    //                fst_error("DSDS");
    //                return end();
    //            }

    //            base::_size++;
    //            return begin() + index;
    //        }
    //        else
    //        {
    //            fst_assert(base::_size != capacity(), "Can't push_back when fixed_vector is full.");
    //            return end();
    //        }
    //    }

    //    FST_ALWAYS_INLINE constexpr void pop_back() noexcept
    //    {
    //        fst_assert(base::_size > 0, "Can't pop_back an empty fixed_vector.");
    //        if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[base::_size].~value_type(); }
    //        --base::_size;
    //    }

    //    FST_ALWAYS_INLINE constexpr void erase(size_type index) noexcept
    //    {
    //        fst_assert(index < base::_size, "Try to erase out of bounds index.");

    //        if (index == base::_size - 1)
    //        {
    //            pop_back();
    //            return;
    //        }

    //        if constexpr (__fst::is_trivially_copyable_v<value_type>) { __fst::memmove(begin() + index, begin() + index + 1, (base::_size - index) * sizeof(value_type)); }
    //        else
    //        {
    //            for (size_type i = index; i < size() - 1; i++)
    //            {
    //                (*this)[i] = __fst::move((*this)[i + 1]);
    //            }

    //            if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[base::_size].~value_type(); }
    //        }

    //        base::_size--;
    //    }

    //    FST_ALWAYS_INLINE constexpr void unordered_erase(size_type index) noexcept
    //    {
    //        fst_assert(index < base::_size, "Try to erase out of bounds index.");

    //        if (index == base::_size - 1)
    //        {
    //            pop_back();
    //            return;
    //        }

    //        __fst::move_element((*this)[index], __fst::move(this->back()));

    //        if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[base::_size].~value_type(); }
    //        base::_size--;
    //    }

    //    FST_ALWAYS_INLINE constexpr void resize(size_type count) noexcept
    //    {
    //        if (count <= size())
    //        {
    //            if constexpr (!__fst::is_trivially_destructible_v<value_type>)
    //            {
    //                for (size_type i = count; i < size(); i++)
    //                {
    //                    (*this)[i].~value_type();
    //                }
    //            }
    //            base::_size = count;
    //            return;
    //        }

    //        if (count <= capacity())
    //        {
    //            if constexpr (!__fst::is_trivially_default_constructible_v<value_type>)
    //            {
    //                for (size_type i = size(); i < count; i++)
    //                {
    //                    fst_placement_new(data() + i)
    //                    value_type();
    //                }
    //            }

    //            base::_size = count;

    //            return;
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            if (!base::grow(__fst::next_power_of_two(count)))
    //            {
    //                fst_assert(count <= capacity());
    //                return;
    //            }

    //            if constexpr (!__fst::is_trivially_default_constructible_v<value_type>)
    //            {
    //                for (size_type i = size(); i < count; i++)
    //                {
    //                    fst_placement_new(data() + i)
    //                    value_type();
    //                }
    //            }
    //        }
    //        base::_size = count;
    //    }

    //    FST_ALWAYS_INLINE constexpr void resize(size_type count, const_reference value) noexcept
    //    {
    //        if (count <= size())
    //        {
    //            if constexpr (!__fst::is_trivially_destructible_v<value_type>)
    //            {
    //                for (size_type i = count; i < size(); i++)
    //                {
    //                    (*this)[i].~value_type();
    //                }
    //            }
    //            base::_size = count;
    //            return;
    //        }

    //        if (count <= capacity())
    //        {
    //            if constexpr (__fst::is_trivially_copyable_v<value_type>)
    //            {
    //                for (size_type i = size(); i < count; i++)
    //                {
    //                    __fst::copy_element((*this)[i], value);
    //                }
    //            }
    //            else
    //            {
    //                for (size_type i = size(); i < count; i++)
    //                {
    //                    fst_placement_new(data() + i)
    //                    value_type(value);
    //                }
    //            }

    //            base::_size = count;

    //            return;
    //        }

    //        if constexpr (is_resizable())
    //        {
    //            if (!base::grow(__fst::next_power_of_two(count)))
    //            {
    //                fst_assert(count <= capacity());
    //                return;
    //            }

    //            if constexpr (__fst::is_trivially_copyable_v<value_type>)
    //            {
    //                for (size_type i = size(); i < count; i++)
    //                {
    //                    __fst::copy_element((*this)[i], value);
    //                }
    //            }
    //            else
    //            {
    //                for (size_type i = size(); i < count; i++)
    //                {
    //                    fst_placement_new(data() + i)
    //                    value_type(value);
    //                }
    //            }
    //        }
    //        base::_size = count;
    //    }

    //    /*FST_ALWAYS_INLINE constexpr void swap(basic_fixed_vector& __a) noexcept
    //{
    //        __fst::swap_ranges(data(), data() + _Size, __a.data());
    //}*/
    //};

#define FST_EXPOSE_BASIC_VECTOR(TYPE)                         \
    using value_type = typename TYPE::value_type;             \
    using size_type = typename TYPE::size_type;               \
    using difference_type = typename TYPE::difference_type;   \
    using pointer = typename TYPE::pointer;                   \
    using const_pointer = typename TYPE::const_pointer;       \
    using reference = typename TYPE::reference;               \
    using const_reference = typename TYPE::const_reference;   \
    using iterator = typename TYPE::iterator;                 \
    using const_iterator = typename TYPE::const_iterator;     \
    using memory_zone_type = typename TYPE::memory_zone_type; \
    using TYPE::get_memory_zone;                              \
    using TYPE::TYPE;                                         \
    using TYPE::operator=;                                    \
    using TYPE::operator[];                                   \
    using TYPE::size;                                         \
    using TYPE::capacity;                                     \
    using TYPE::max_size;                                     \
    using TYPE::is_resizable;                                 \
    using TYPE::empty;                                        \
    using TYPE::is_full;                                      \
    using TYPE::data;                                         \
    using TYPE::begin;                                        \
    using TYPE::end;                                          \
    using TYPE::cbegin;                                       \
    using TYPE::cend;                                         \
    using TYPE::front;                                        \
    using TYPE::back;                                         \
    using TYPE::reversed;                                     \
    using TYPE::reset;                                        \
    using TYPE::clear;                                        \
    using TYPE::push_back;                                    \
    using TYPE::emplace_back;                                 \
    using TYPE::insert;                                       \
    using TYPE::erase;                                        \
    using TYPE::resize;                                       \
    using TYPE::pop_back;                                     \
    using TYPE::unordered_erase

    //// fixed_vector_range
    //template <typename _T, size_t _Size, size_t _Alignment = alignof(_T)>
    //struct fixed_vector_range : __fst::basic_vector<_T, _Size, __fst::vector_range_base<_T, _Size, _Alignment>, _Alignment>
    //{
    //    using basic_vector_base = __fst::basic_vector<_T, _Size, __fst::vector_range_base<_T, _Size, _Alignment>, _Alignment>;
    //    FST_EXPOSE_BASIC_VECTOR(basic_vector_base);

    //    constexpr fixed_vector_range() noexcept = default;
    //    constexpr fixed_vector_range(const fixed_vector_range&) noexcept = default;
    //    constexpr fixed_vector_range(fixed_vector_range&&) noexcept = default;

    //    constexpr fixed_vector_range& operator=(const fixed_vector_range&) noexcept = default;
    //    constexpr fixed_vector_range& operator=(fixed_vector_range&&) noexcept = default;
    //};

    //// vector_range
    //template <typename _T, size_t _Alignment = alignof(_T)>
    //struct vector_range : __fst::basic_vector<_T, __fst::dynamic_size, __fst::vector_range_base<_T, __fst::dynamic_size, _Alignment>, _Alignment>
    //{
    //    using basic_vector_base = __fst::basic_vector<_T, __fst::dynamic_size, __fst::vector_range_base<_T, __fst::dynamic_size, _Alignment>, _Alignment>;
    //    FST_EXPOSE_BASIC_VECTOR(basic_vector_base);
    //};

    //// clang-format off
    //// is_fixed_vector_range
    //template <class _T> struct is_fixed_vector_range : __fst::false_t {};
    //template <class _T, size_t _Size, size_t _Alignment>
    //struct is_fixed_vector_range<__fst::fixed_vector_range<_T, _Size, _Alignment>> : __fst::true_t {};

    //// is_vector_range
    //template <class _T> struct is_vector_range : __fst::false_t {};
    //template <class _T, size_t _Alignment>
    //struct is_vector_range<__fst::vector_range<_T, _Alignment>> : __fst::true_t {};
    //// clang-format on

    template <typename _T, size_t _Alignment = alignof(_T), class _MemoryZone = __fst::default_memory_zone, class _MemoryCategory = __fst::default_memory_category>
    class vector
    {
      public:
        static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");

        using memory_zone_type = _MemoryZone;
        FST_DECLARE_CONTAINER_TYPES(_T);

        FST_NODISCARD FST_ALWAYS_INLINE const memory_zone_type& get_memory_zone() const noexcept { return _data.second(); }
        FST_NODISCARD FST_ALWAYS_INLINE memory_zone_type& get_memory_zone() noexcept { return _data.second(); }

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_default_constructible_v<_Zone>, int> = 0>
        inline vector() noexcept
            : _data(nullptr, memory_zone_type{})
            , _size(0)
            , _capacity(0)

        {}

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_copy_constructible_v<_Zone>, int> = 0>
        inline vector(const memory_zone_type& zone) noexcept
            : _data(nullptr, zone)
            , _size(0)
            , _capacity(0)
        {}

        vector(const vector& vec) noexcept
            : _data(nullptr, vec.get_memory_zone())
        {

            if (grow(vec.size()))
            {
                __fst::copy_construct_range(_data.first(), vec.data(), vec.size());
                _size = vec.size();
                _capacity = vec.size();
            }
            else { fst_error("CALADLKLADKA"); }
        }

        vector(vector&& vec) noexcept
            : _data(nullptr, __fst::move(vec.get_memory_zone()))
        {
            _data.first() = vec._data.first();
            _capacity = vec._capacity;
            _size = vec._size;

            vec._data.first() = nullptr;
            vec._capacity = 0;
            vec._size = 0;
        }

        inline ~vector() noexcept
        {
            if (_data.first())
            {
                __fst::destruct_range(data(), size());
                _data.second().aligned_deallocate((void*) _data.first(), _MemoryCategory::id());
            }
        }

        inline vector& operator=(const vector& vec) noexcept
        {
            reset();

            if (grow(vec.size()))
            {
                __fst::copy_construct_range(data(), vec.data(), vec.size());
                _size = vec.size();
                _capacity = vec.size();
            }
            else { fst_error("CALADLKLADKA"); }

            return *this;
        }

        inline vector& operator=(vector&& vec) noexcept
        {
            reset();

            _data.first() = vec._data.first();
            _capacity = vec._capacity;
            _size = vec._size;

            vec._data.first() = nullptr;
            vec._capacity = 0;
            vec._size = 0;

            return *this;
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t alignment() noexcept { return _Alignment; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t max_size() noexcept { return __fst::dynamic_size; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t capacity() const noexcept { return _capacity; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _data.first() == nullptr || _size == 0; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_full() const noexcept { return (bool) _size == _capacity; }

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return true; }

        FST_DECLARE_CONTAINER_DATA(_data.first())

        inline void reset()
        {
            if (_data.first())
            {
                __fst::destruct_range(data(), size());
                _data.second().aligned_deallocate((void*) _data.first(), _MemoryCategory::id());
            }

            _data.first() = nullptr;
            _size = 0;
            _capacity = 0;
        }

        FST_ALWAYS_INLINE constexpr void clear() noexcept
        {

            if (_data.first()) { __fst::destruct_range(data(), size()); }

            _size = 0;
        }

        FST_ALWAYS_INLINE constexpr void push_back(const_reference value) noexcept
        {
            if (size() < capacity())
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::copy_element((*this)[_size++], value); }
                else { fst_placement_new(this->data() + _size++) value_type(value); }

                return;
            }

            if (grow(__fst::next_power_of_two(capacity())))
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::copy_element((*this)[_size++], value); }
                else { fst_placement_new(this->data() + _size++) value_type(value); }

                return;
            }

            fst_error("DLKSJDKLSJDLS");
        }

        template <class U = value_type, __fst::enable_if_t<!__fst::is_trivial_cref_v<U>, int> = 0>
        FST_ALWAYS_INLINE constexpr void push_back(U&& value) noexcept
        {
            if (size() < capacity())
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[_size++], __fst::forward<U>(value)); }
                else { fst_placement_new(this->data() + _size++) value_type(__fst::forward<U>(value)); }

                return;
            }

            if (grow(__fst::next_power_of_two(capacity())))
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[_size++], __fst::forward<U>(value)); }
                else { fst_placement_new(this->data() + _size++) value_type(__fst::forward<U>(value)); }

                return;
            }

            fst_error("DLKSJDKLSJDLS");
        }

        /*template <class U, __fst::enable_if_t<__fst::is_same_v<U, value_type>, int> = 0>
        FST_ALWAYS_INLINE constexpr void push_back(U&& value) noexcept
        {
            if (size() < capacity())
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[_size++], __fst::forward<U>(value)); }
                else
                {
                    fst_placement_new(this->data() + _size++) value_type(__fst::forward<U>(value));
                }

                return;
            }

            if (grow(__fst::next_power_of_two(capacity())))
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[_size++], __fst::forward<U>(value)); }
                else
                {
                    fst_placement_new(this->data() + _size++) value_type(__fst::forward<U>(value));
                }

                return;
            }

            fst_error("DLKSJDKLSJDLS");
        }*/

        template <typename... _Args>
        FST_ALWAYS_INLINE constexpr reference emplace_back(_Args&&... args) noexcept
        {
            // push_back(value_type(__fst::forward<_Args>(args)...));
            if (size() < capacity())
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[_size++], __fst::forward<_Args>(args)...); }
                else { fst_placement_new(this->data() + _size++) value_type(__fst::forward<_Args>(args)...); }

                return this->back();
            }

            if (grow(__fst::next_power_of_two(capacity())))
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[_size++], __fst::forward<_Args>(args)...); }
                else { fst_placement_new(this->data() + _size++) value_type(__fst::forward<_Args>(args)...); }

                return this->back();
            }

            fst_error("DLKSJDKLSJDLS");

            return this->back();
        }

        inline constexpr iterator insert(const_iterator pos, const_reference value) noexcept
        {
            if (!_size || pos == cend())
            {
                push_back(value);
                return end() - 1;
            }

            if (size() < capacity())
            {
                const size_type index = __fst::pdistance(cbegin(), pos);

                if constexpr (__fst::is_trivially_copyable_v<_T>)
                {
                    __fst::memmove(begin() + index + 1, begin() + index, (_size - index) * sizeof(value_type));
                    __fst::copy_element((*this)[index], value);
                }
                else
                {
                    fst_placement_new(end()) value_type(__fst::move(back()));

                    for (size_type i = size() - 1; i > index; i--)
                    {
                        __fst::move_element((*this)[i], __fst::move((*this)[i - 1]));
                    }

                    __fst::copy_element((*this)[index], value);
                }

                _size++;
                return begin() + index;
            }

            const size_type index = __fst::pdistance(cbegin(), pos);

            if (!grow(__fst::next_power_of_two(capacity()),
                    [&](pointer ndata, pointer data, size_type size, FST_ATTRIBUTE_UNUSED size_type new_capacity)
                    {
                        __fst::relocate(ndata, data, index);
                        __fst::relocate(ndata + index + 1, data + index, size - index);
                        fst_placement_new(ndata + index) value_type(value);
                    }))
            {

                fst_error("DSDS");
                return end();
            }

            _size++;

            return begin() + index;
        }

        template <class U = value_type, __fst::enable_if_t<!__fst::is_trivial_cref_v<U>, int> = 0>
        inline constexpr iterator insert(const_iterator pos, U&& value) noexcept
        {
            if (!_size || pos == cend())
            {
                push_back(__fst::move(value));
                return end() - 1;
            }

            if (size() < capacity())
            {
                const size_type index = __fst::pdistance(cbegin(), pos);

                if constexpr (__fst::is_trivially_copyable_v<_T>)
                {
                    __fst::memmove(begin() + index + 1, begin() + index, (_size - index) * sizeof(value_type));
                    __fst::copy_element((*this)[index], value);
                }
                else
                {
                    fst_placement_new(end()) value_type(__fst::move(back()));

                    for (size_type i = size() - 1; i > index; i--)
                    {
                        __fst::move_element((*this)[i], __fst::move((*this)[i - 1]));
                    }

                    __fst::move_element((*this)[index], __fst::move(value));
                }

                _size++;
                return begin() + index;
            }

            const size_type index = __fst::pdistance(cbegin(), pos);

            if (!grow(__fst::next_power_of_two(capacity()),
                    [&](pointer ndata, pointer data, size_type size, FST_ATTRIBUTE_UNUSED size_type new_capacity)
                    {
                        __fst::relocate(ndata, data, index);
                        __fst::relocate(ndata + index + 1, data + index, size - index);
                        fst_placement_new(ndata + index) value_type(__fst::move(value));
                    }))
            {

                fst_error("DSDS");
                return end();
            }

            _size++;
            return begin() + index;
        }

        FST_ALWAYS_INLINE constexpr void pop_back() noexcept
        {
            fst_assert(_size > 0, "Can't pop_back an empty fixed_vector.");
            if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[_size].~value_type(); }
            --_size;
        }

        FST_ALWAYS_INLINE constexpr void erase_at(size_type index) noexcept
        {
            fst_assert(index < _size, "Try to erase out of bounds index.");

            if (index == _size - 1)
            {
                pop_back();
                return;
            }

            if constexpr (__fst::is_trivially_copyable_v<value_type>) { __fst::memmove(begin() + index, begin() + index + 1, (_size - index) * sizeof(value_type)); }
            else
            {
                for (size_type i = index; i < size() - 1; i++)
                {
                    this->data()[i] = __fst::move(this->data()[i + 1]);
                }

                if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[_size].~value_type(); }
            }

            _size--;
        }

        FST_ALWAYS_INLINE constexpr void erase(const_iterator it) noexcept { erase_at(__fst::pdistance(cbegin(), it)); }

        FST_ALWAYS_INLINE constexpr void unordered_erase(size_type index) noexcept
        {
            fst_assert(index < _size, "Try to erase out of bounds index.");

            if (index == _size - 1)
            {
                pop_back();
                return;
            }

            __fst::move_element((*this)[index], __fst::move(this->back()));

            if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[_size].~value_type(); }
            _size--;
        }

        void reserve(size_type count) noexcept
        {
            if (count <= capacity()) { return; }

            if (!grow(count))
            {
                fst_error("ERROR");
                return;
            }
        }

        FST_ALWAYS_INLINE constexpr void resize(size_type count) noexcept
        {
            if (count <= size())
            {
                if constexpr (!__fst::is_trivially_destructible_v<value_type>)
                {
                    for (size_type i = count; i < size(); i++)
                    {
                        (*this)[i].~value_type();
                    }
                }
                _size = count;
                return;
            }

            if (count <= capacity())
            {
                if constexpr (!__fst::is_trivially_default_constructible_v<value_type>)
                {
                    for (size_type i = size(); i < count; i++)
                    {
                        fst_placement_new(data() + i) value_type();
                    }
                }

                _size = count;

                return;
            }

            if (!grow(__fst::next_power_of_two(count)))
            {
                fst_assert(count <= capacity());
                return;
            }

            if constexpr (!__fst::is_trivially_default_constructible_v<value_type>)
            {
                for (size_type i = size(); i < count; i++)
                {
                    fst_placement_new(data() + i) value_type();
                }
            }

            _size = count;
        }

        FST_ALWAYS_INLINE constexpr void resize(size_type count, const_reference value) noexcept
        {
            if (count <= size())
            {
                if constexpr (!__fst::is_trivially_destructible_v<value_type>)
                {
                    for (size_type i = count; i < size(); i++)
                    {
                        (*this)[i].~value_type();
                    }
                }
                _size = count;
                return;
            }

            if (count <= capacity())
            {
                if constexpr (__fst::is_trivially_copyable_v<value_type>)
                {
                    for (size_type i = size(); i < count; i++)
                    {
                        __fst::copy_element((*this)[i], value);
                    }
                }
                else
                {
                    for (size_type i = size(); i < count; i++)
                    {
                        fst_placement_new(data() + i) value_type(value);
                    }
                }

                _size = count;

                return;
            }

            if (!grow(__fst::next_power_of_two(count)))
            {
                fst_assert(count <= capacity());
                return;
            }

            if constexpr (__fst::is_trivially_copyable_v<value_type>)
            {
                for (size_type i = size(); i < count; i++)
                {
                    __fst::copy_element((*this)[i], value);
                }
            }
            else
            {
                for (size_type i = size(); i < count; i++)
                {
                    fst_placement_new(data() + i) value_type(value);
                }
            }

            _size = count;
        }

        FST_ALWAYS_INLINE constexpr void resize_extra(size_type count) noexcept { resize(size() + count); }

        FST_ALWAYS_INLINE constexpr void resize_extra(size_type count, const_reference value) noexcept { resize(size() + count, value); }

      private:
        __fst::pair<_T*, _MemoryZone> _data = {};
        size_t _size = 0;
        size_t _capacity = 0;

        inline bool grow(size_t new_capacity) noexcept
        {
            _T* tmp_data = (_T*) _data.second().aligned_allocate(new_capacity * sizeof(_T), _Alignment, _MemoryCategory::id());
            if (!tmp_data) { return false; }

            _capacity = new_capacity;

            if (_data.first())
            {
                __fst::relocate(tmp_data, _data.first(), _size);
                _data.second().aligned_deallocate((void*) _data.first(), _MemoryCategory::id());
            }

            _data.first() = tmp_data;
            return true;
        }

        template <class _Mod>
        inline bool grow(size_t new_capacity, _Mod mod) noexcept
        {
            _T* tmp_data = (_T*) _data.second().aligned_allocate(new_capacity * sizeof(_T), _Alignment, _MemoryCategory::id());
            if (!tmp_data) { return false; }

            _capacity = new_capacity;

            if (_data.first())
            {
                mod(tmp_data, _data.first(), _size, new_capacity);
                _data.second().aligned_deallocate((void*) _data.first(), _MemoryCategory::id());
            }

            _data.first() = tmp_data;
            return true;
        }
    };

    // is_vector
    template <class _T>
    struct is_vector : __fst::false_t
    {};

    template <class _T, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_vector<__fst::vector<_T, _Alignment, _MemoryZone, _MemoryCategory>> : __fst::true_t
    {};

FST_END_NAMESPACE
