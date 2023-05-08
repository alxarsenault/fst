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

#include "fst/common.h"
#include "fst/aligned_storage.h"
#include "fst/math.h"
#include "fst/memory.h"
#include "fst/traits.h"
#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    template <typename _T, size_t _Size, size_t _Alignment = alignof(_T), class _MemoryZone = __fst::default_memory_zone,
        class _MemoryCategory = __fst::default_memory_category>
    class small_vector : public _MemoryZone
    {
      public:
        static_assert(_Size != __fst::dynamic_size, "should be dynamic");
        static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");

        FST_DECLARE_CONTAINER_TYPES(_T);
        using memory_zone_type = _MemoryZone;
        FST_NODISCARD FST_ALWAYS_INLINE memory_zone_type& get_memory_zone() const noexcept { return (memory_zone_type&) *this; }

        small_vector() noexcept = default;

        small_vector(const small_vector& vec) noexcept
        {

            if (vec.size() <= _Size)
            {
                __fst::copy_construct_range(_data, vec.data(), vec.size());
                _size = vec.size();
            }
            else
            {
                if (grow(vec.size()))
                {
                    __fst::copy_construct_range(_data, vec.data(), vec.size());
                    _size = vec.size();
                    _capacity = vec.size();
                }
                else { fst_error("CALADLKLADKA"); }
            }
        }

        small_vector(small_vector&& vec) noexcept
        {
            if (vec.has_stack_data())
            {
                __fst::copy_construct_range(_data, vec.data(), vec.size());
                _size = vec.size();
            }
            else
            {
                _data = vec._data;
                _capacity = vec._capacity;
                _size = vec._size;

                vec._data = (pointer) &vec._sdata[0];
                vec._capacity = _Size;
                vec._size = 0;
            }
        }

        inline ~small_vector() noexcept
        {
            if (_data)
            {

                __fst::destruct_range(data(), size());

                if (_data != (_T*) &_sdata[0]) { _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id()); }
            }
        }

        inline small_vector& operator=(const small_vector& vec) noexcept
        {
            reset();

            if (vec.size() <= _Size)
            {
                __fst::copy_construct_range(_data, vec.data(), vec.size());
                _size = vec.size();
            }
            else
            {
                if (grow(vec.size()))
                {
                    __fst::copy_construct_range(_data, vec.data(), vec.size());
                    _size = vec.size();
                    _capacity = vec.size();
                }
                else { fst_error("CALADLKLADKA"); }
            }
            return *this;
        }

        inline small_vector& operator=(small_vector&& vec) noexcept
        {
            reset();

            if (vec.has_stack_data())
            {
                __fst::copy_construct_range(_data, vec.data(), vec.size());
                _size = vec.size();
            }
            else
            {
                _data = vec._data;
                _capacity = vec._capacity;
                _size = vec._size;

                vec._data = (pointer) &vec._sdata[0];
                vec._capacity = _Size;
                vec._size = 0;
            }
            return *this;
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t alignment() noexcept { return _Alignment; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t max_size() noexcept { return __fst::dynamic_size; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t capacity() const noexcept { return _capacity; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _size == 0; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_full() const noexcept { return _size == _capacity; }

        FST_NODISCARD FST_ALWAYS_INLINE static constexpr bool is_resizable() noexcept { return true; }

        FST_DECLARE_CONTAINER_DATA(_data)

        inline constexpr bool has_stack_data() const noexcept { return _data == (const_pointer) &_sdata[0]; }

        inline void reset()
        {
            if (_data)
            {

                __fst::destruct_range(data(), size());

                if (!has_stack_data()) { _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id()); }
            }

            _data = (pointer) &_sdata[0];
            _size = 0;
            _capacity = _Size;
        }

        FST_ALWAYS_INLINE constexpr void clear() noexcept
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
            }

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
        }

        template <class U = value_type, __fst::enable_if_t<!__fst::is_trivial_cref_v<U> && __fst::is_same_v<U, value_type>, int> = 0>
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
                const size_type index = __fst::distance(cbegin(), pos);

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

            const size_type index = __fst::distance(cbegin(), pos);

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
                const size_type index = __fst::distance(cbegin(), pos);

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

            const size_type index = __fst::distance(cbegin(), pos);

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
                    (*this)[i] = __fst::move((*this)[i + 1]);
                }

                if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[_size].~value_type(); }
            }

            _size--;
        }

        FST_ALWAYS_INLINE constexpr void erase(const_iterator it) noexcept { erase_at((size_type) __fst::distance(cbegin(), it)); }

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
                __fst::destruct_range(data(), size());

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

        inline bool grow(size_t new_capacity) noexcept
        {
            _T* tmp_data = (_T*) _MemoryZone::aligned_allocate(new_capacity * sizeof(_T), _Alignment, _MemoryCategory::id());
            if (!tmp_data) { return false; }

            _capacity = new_capacity;

            if (_data)
            {
                __fst::relocate(tmp_data, _data, _size);

                if (_data != (_T*) &_sdata[0]) { _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id()); }
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

                if (_data != (_T*) &_sdata[0]) { _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id()); }
            }

            _data = tmp_data;
            return true;
        }

        alignas(_Alignment) __fst::aligned_type_storage<_T> _sdata[_Size];

        _T* _data = (_T*) &_sdata[0];
        size_t _size = 0;
        size_t _capacity = _Size;
    };

    // is_small_vector
    template <class _T>
    struct is_small_vector : __fst::false_t
    {};
    template <class _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_small_vector<__fst::small_vector<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>> : __fst::true_t
    {};

FST_END_NAMESPACE
