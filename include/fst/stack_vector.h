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
#include "fst/pair.h"
#include "fst/pointer.h"
#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    template <typename _T, size_t _Size, size_t _Alignment = alignof(_T)>
    class stack_vector
    {
      public:
        static_assert(_Size != __fst::dynamic_size, "should be dynamic");
        static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");
        
        using value_type = _T;
        #include "fst/template/container_types.template"

        using memory_zone_type = __fst::void_memory_zone;
        FST_NODISCARD FST_ALWAYS_INLINE memory_zone_type get_memory_zone() const noexcept { return memory_zone_type{}; }

        stack_vector() noexcept = default;

        stack_vector(const stack_vector& vec) noexcept
        {

            if (vec.size() <= capacity())
            {
                __fst::copy_construct_range(data(), vec.data(), vec.size());
                _size = vec.size();
                return;
            }
            fst_error("CALADLKLADKA");
        }

        stack_vector(stack_vector&& vec) noexcept
        {
            __fst::copy_construct_range(data(), vec.data(), vec.size());
            _size = vec.size();
        }

        inline ~stack_vector() noexcept
        {
            if (size())
            {
                __fst::destruct_range(data(), size());

                _size = 0;
            }
        }

        inline stack_vector& operator=(const stack_vector& vec) noexcept
        {
            reset();

            if (vec.size() <= capacity())
            {
                __fst::copy_construct_range(data(), vec.data(), vec.size());
                _size = vec.size();
            }
            else { fst_error("CALADLKLADKA"); }
        }

        inline stack_vector& operator=(stack_vector&& vec) noexcept
        {
            reset();

            __fst::copy_construct_range(data(), vec.data(), vec.size());
            _size = vec.size();
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t alignment() noexcept { return _Alignment; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t max_size() noexcept { return _Size; }
        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t capacity() noexcept { return _Size; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _size == 0; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_full() const noexcept { return _size == capacity(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer data() noexcept { return (pointer) &_data[0]; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer data() const noexcept { return (const_pointer) &_data[0]; }
#include "fst/template/container_data.template"

        inline void reset()
        {
            if (size()) { __fst::destruct_range(data(), size()); }

            _size = 0;
        }

        FST_ALWAYS_INLINE constexpr void clear() noexcept
        {

            if (size()) { __fst::destruct_range(data(), size()); }

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

            fst_error("DLKDJSKLJDLS");
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

            fst_error("DLKDJSKLJDLS");
        }

        template <class U, __fst::enable_if_t<__fst::is_same_v<U, value_type>, int> = 0>
        FST_ALWAYS_INLINE constexpr void push_back(U&& value) noexcept
        {
            if (size() < capacity())
            {
                if constexpr (__fst::is_trivially_copyable_v<_T>) { __fst::move_element((*this)[_size++], __fst::forward<U>(value)); }
                else { fst_placement_new(this->data() + _size++) value_type(__fst::forward<U>(value)); }

                return;
            }

            fst_error("DLKDJSKLJDLS");
        }

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

            fst_error("DLKDJSKLJDLS");

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

            fst_error("DLKDJSKLJDLS");
            return end();
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

            fst_error("DSDS");
            return end();
        }

        FST_ALWAYS_INLINE constexpr void pop_back() noexcept
        {
            fst_assert(_size > 0, "Can't pop_back an empty fixed_vector.");
            if constexpr (!__fst::is_trivially_destructible_v<value_type>) { (*this)[_size].~value_type(); }
            --_size;
        }

        FST_ALWAYS_INLINE constexpr void erase(size_type index) noexcept
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
            fst_error("DSLKDJSKLJD");
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

            fst_error("DLSKDJSKLJDS");
        }

        alignas(_Alignment) __fst::aligned_type_storage<_T> _data[_Size];
        size_t _size = 0;
    };

    // is_small_vector
    template <class _T>
    struct is_stack_vector : __fst::false_t
    {};
    template <class _T, size_t _Size, size_t _Alignment>
    struct is_stack_vector<__fst::stack_vector<_T, _Size, _Alignment>> : __fst::true_t
    {};

FST_END_NAMESPACE
