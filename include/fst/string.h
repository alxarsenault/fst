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
#include "fst/memory.h"
#include "fst/array.h"
#include "fst/allocator.h"

#include "fst/string_view.h"
#include "fst/utility.h"
#include "fst/stream.h"
#include "fst/allocator.h"

FST_BEGIN_NAMESPACE
    template <class _CharT, class _Allocator = __fst::allocator<_CharT>, class Enable = void>
    class basic_string;

    template <class _CharT, class _Allocator, class Enable>
    class basic_string
    {
      public:
        using value_type = _CharT;
        using traits_type = __fst::char_traits<_CharT>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;

        using allocator_type = _Allocator;
        using view_type = __fst::basic_string_view<value_type>;

        static constexpr size_type npos = (__fst::numeric_limits<size_type>::max)();

        basic_string(__fst::nullptr_t) = delete;

        inline constexpr basic_string() noexcept
            : _content{ small_array{}, allocator_type{} }
        {}

        inline explicit constexpr basic_string(const allocator_type& alloc) noexcept
            : _content(small_array{}, alloc)
        {}

        inline constexpr basic_string(size_type count, value_type c, const allocator_type& alloc = allocator_type()) noexcept
            : _content(small_array{}, alloc)
        {
            if (count <= small_capacity)
            {
                __fst::memfill(small_data(), c, count);
                small_data()[count] = 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = count + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memfill(big_data(), c, count);
                big_data()[count] = 0;
                big_size() = count;
                big_capacity() = count;
                set_big();
            }
        }

        inline constexpr basic_string(const basic_string& str) noexcept
        {
            if (str.is_small()) { _content.first() = str._content.first(); }
            else
            {
                const size_type alloc_length = str.size() + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str.data(), alloc_length * sizeof(value_type));

                big_size() = alloc_length - 1;
                big_capacity() = alloc_length - 1;
                set_big();
            }
        }

        inline constexpr basic_string(const basic_string& str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            if (str.is_small()) { _content.first() = str._content.first(); }
            else
            {
                const size_type alloc_length = str.size() + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str.data(), alloc_length * sizeof(value_type));

                big_size() = alloc_length - 1;
                big_capacity() = alloc_length - 1;
                set_big();
            }
        }

        inline constexpr basic_string(basic_string&& str) noexcept
        {
            _content.first() = str._content.first();
            str._content.first() = small_array{};
        }

        inline constexpr basic_string(basic_string&& str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            _content.first() = str._content.first();
            str._content.first() = small_array{};
        }

        inline constexpr basic_string(view_type str) noexcept
        {
            const size_type len = str.size();

            if (len <= small_capacity)
            {
                __fst::memmove(small_data(), str.data(), len * sizeof(value_type));
                small_data()[len] = 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = len + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str.data(), len * sizeof(value_type));

                big_data()[len] = 0;
                big_size() = len;
                big_capacity() = len;
                set_big();
            }
        }

        inline constexpr basic_string(view_type str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            const size_type len = str.size();

            if (len <= small_capacity)
            {
                __fst::memmove(small_data(), str.data(), len * sizeof(value_type));
                small_data()[len] = 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = len + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str.data(), len * sizeof(value_type));

                big_data()[len] = 0;
                big_size() = len;
                big_capacity() = len;
                set_big();
            }
        }

        inline constexpr basic_string(const_pointer str) noexcept
        {
            const size_type len = traits_type::length(str);
            if (len <= small_capacity)
            {
                pointer _data = small_data();
                __fst::memmove(_data, str, len * sizeof(value_type));
                _data[len] = (value_type) 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = len + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str, alloc_length * sizeof(value_type));

                big_size() = len;
                big_capacity() = len;
                set_big();
            }
        }

        inline constexpr basic_string(const_pointer str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            const size_type len = traits_type::length(str);
            if (len <= small_capacity)
            {
                pointer _data = small_data();
                __fst::memmove(_data, str, len * sizeof(value_type));
                _data[len] = (value_type) 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = len + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str, alloc_length * sizeof(value_type));

                big_size() = len;
                big_capacity() = len;
                set_big();
            }
        }

        inline constexpr basic_string(const_pointer str, size_type len) noexcept
        {
            if (len <= small_capacity)
            {
                pointer _data = small_data();
                __fst::memmove(_data, str, len * sizeof(value_type));
                _data[len] = (value_type) 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = len + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str, alloc_length * sizeof(value_type));

                big_size() = len;
                big_capacity() = len;
                set_big();
            }
        }

        inline ~basic_string() noexcept { reset(); }

        inline constexpr basic_string& operator=(const basic_string& str) noexcept
        {
            reset();

            if (str.is_small()) { _content = str._content; }
            else
            {
                const size_type alloc_length = str.size() + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str.data(), alloc_length * sizeof(value_type));

                big_size() = alloc_length - 1;
                big_capacity() = alloc_length - 1;
                set_big();
            }

            return *this;
        }

        inline constexpr basic_string& operator=(basic_string&& str) noexcept
        {
            reset();

            _content = str._content;
            str._content.first() = small_array{};
            return *this;
        }

        inline constexpr basic_string& operator=(view_type str) noexcept
        {
            reset();

            const size_type len = str.size();

            if (len <= small_capacity)
            {
                pointer _data = small_data();
                __fst::memmove(_data, str.data(), len * sizeof(value_type));
                _data[len] = (value_type) 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = len + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str.data(), alloc_length * sizeof(value_type));

                big_size() = len;
                big_capacity() = len;
                set_big();
            }

            return *this;
        }

        inline constexpr basic_string& operator=(const_pointer str) noexcept
        {
            reset();

            const size_type len = traits_type::length(str);
            if (len <= small_capacity)
            {
                pointer _data = small_data();
                __fst::memmove(_data, str, len * sizeof(value_type));
                _data[len] = (value_type) 0;
                set_small();
            }
            else
            {
                const size_type alloc_length = len + 1;
                big_data() = (pointer) _content.second().allocate(alloc_length);
                __fst::memmove(big_data(), str, alloc_length * sizeof(value_type));

                big_size() = len;
                big_capacity() = len;
                set_big();
            }

            return *this;
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr allocator_type get_allocator() const noexcept { return _content.second(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer data() noexcept { return is_small() ? small_data() : big_data(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer data() const noexcept { return is_small() ? small_data() : big_data(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer c_str() const noexcept { return data(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type size() const noexcept { return is_small() ? traits_type::length(small_data()) : big_size(); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type length() const noexcept { return is_small() ? traits_type::length(small_data()) : big_size(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return size() == 0; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type capacity() const noexcept { return is_small() ? small_capacity : big_capacity(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr view_type view() const noexcept { return view_type(data(), size()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_appendable(size_type count) const noexcept
        {
            if (is_small()) { return traits_type::length(small_data()) + count <= small_capacity; }
            else { return big_size() + count <= big_capacity(); }
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference operator[](size_type n) noexcept
        {
            //fst_assert(n < size() || (n == 0), "Index out of bounds");
            return data()[n];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference operator[](size_type n) const noexcept
        {
            //fst_assert(n < size() || (n == 0), "Index out of bounds");
            return data()[n];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference front() noexcept
        {
            fst_assert(size() > 0, "basic_string::front when empty.");
            return data()[0];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference front() const noexcept
        {
            fst_assert(size() > 0, "basic_string::front when empty.");
            return data()[0];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference back() noexcept
        {
            fst_assert(size() > 0, "basic_string::back when empty.");
            return data()[size() - 1];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference back() const noexcept
        {
            fst_assert(size() > 0, "basic_string::back when empty.");
            return data()[size() - 1];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr iterator begin() noexcept { return iterator(data()); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator begin() const noexcept { return const_iterator(data()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr iterator end() noexcept { return iterator(data() + size()); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator end() const noexcept { return const_iterator(data() + size()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator cbegin() const noexcept { return begin(); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator cend() const noexcept { return end(); }

        inline void reset() noexcept
        {
            deallocate_if_big();
            _content.first() = small_array{};
        }

        inline constexpr basic_string& operator+=(const basic_string& str) noexcept { return append(str); }
        inline constexpr basic_string& operator+=(view_type str) noexcept { return append(str); }
        inline constexpr basic_string& operator+=(value_type c) noexcept
        {
            push_back(c);
            return *this;
        }
        inline constexpr basic_string& operator+=(const_pointer str) noexcept { return append(str); }

        FST_NODISCARD constexpr basic_string substr(size_type _Off = 0, size_type _Count = npos) const noexcept
        {
            // return [_Off, _Off + _Count) as new string
            return basic_string(data() + _Off, __fst::minimum(_Count, size() - _Off));
        }

        constexpr basic_string& append(size_type count, value_type ch) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type avail_space = _capacity - _size;
            const size_type new_size = _size + count;

            pointer _data = data();

            if (count <= avail_space)
            {
                for (size_type i = 0; i < count; i++)
                {
                    _data[i + _size] = ch;
                }

                _data[new_size] = 0;

                if (is_big()) { big_size() = new_size; }

                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);

            for (size_type i = 0; i < count; i++)
            {
                new_data[i + _size] = ch;
            }

            new_data[new_size] = 0;

            deallocate_if_big();
            big_data() = new_data;
            big_size() = new_size;
            big_capacity() = new_size;
            set_big();

            return *this;
        }

        constexpr basic_string& append(const_pointer str, size_type count) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type avail_space = _capacity - _size;
            const size_type new_size = _size + count;

            pointer _data = data();

            if (count <= avail_space)
            {
                for (size_type i = 0; i < count; i++)
                {
                    _data[i + _size] = str[i];
                }

                _data[new_size] = 0;

                if (is_big()) { big_size() = new_size; }

                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);

            __fst::memmove(new_data + _size, str, count * sizeof(value_type));
            new_data[new_size] = 0;

            deallocate_if_big();
            big_data() = new_data;
            big_size() = new_size;
            big_capacity() = new_size;
            set_big();

            return *this;
        }

        FST_ALWAYS_INLINE constexpr basic_string& append(const_pointer str) noexcept { return append(str, traits_type::length(str)); }

        FST_ALWAYS_INLINE constexpr basic_string& append(const basic_string& str) noexcept { return append(str.c_str(), str.size()); }

        FST_ALWAYS_INLINE constexpr basic_string& append(view_type str) noexcept { return append(str.data(), str.size()); }

        FST_NODISCARD inline constexpr int compare(const basic_string& str) const noexcept
        {
            const size_type _size = size();
            const size_type ssize = str.size();

            if (_size != ssize) { return _size < ssize ? -1 : _size > ssize ? 1 : 0; }

            return traits_type::compare(data(), str.data(), __fst::minimum(_size, ssize));
        }

        FST_NODISCARD inline constexpr int compare(view_type str) const noexcept
        {
            const size_type _size = size();
            const size_type ssize = str.size();

            if (_size != ssize) { return _size < ssize ? -1 : _size > ssize ? 1 : 0; }

            return traits_type::compare(data(), str.data(), __fst::minimum(_size, ssize));
        }

        FST_NODISCARD inline constexpr int compare(const_pointer str) const noexcept
        {
            const size_type _size = size();
            const size_type ssize = traits_type::length(str);

            if (_size != ssize) { return _size < ssize ? -1 : _size > ssize ? 1 : 0; }

            return traits_type::compare(data(), str, __fst::minimum(_size, ssize));
        }

        FST_ALWAYS_INLINE constexpr void clear() noexcept
        {
            if (is_small()) { _content.first()._data[0] = 0; }
            else
            {
                big_data()[0] = 0;
                big_size() = 0;
            }
        }

        FST_ALWAYS_INLINE constexpr void resize(size_type new_size) noexcept { resize(new_size, value_type()); }

        inline constexpr void resize(size_type new_size, value_type c) noexcept
        {
            const size_type _size = size();

            if (new_size > _size) { append(new_size - _size, c); }
            else
            {
                if (is_small()) { _content.first()._data[new_size] = 0; }
                else
                {
                    big_size() = new_size;
                    big_data()[new_size] = 0;
                }
            }
        }

        inline void reserve(size_type count) noexcept
        {
            const size_type _capacity = capacity();

            if (count <= _capacity) { return; }

            const size_type _size = size();
            const size_type new_capacity = _size + count;

            pointer new_data = grow_copy(new_capacity, _size + 1);

            deallocate_if_big();
            big_data() = new_data;
            big_capacity() = new_capacity;
            set_big();
        }

        FST_ALWAYS_INLINE void reserve_extra(size_type count) noexcept { reserve(size() + count); }

        inline constexpr void push_back(value_type c) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type avail_space = _capacity - _size;
            const size_type new_size = _size + 1;

            pointer _data = data();

            if (avail_space)
            {
                _data[_size] = c;
                _data[new_size] = 0;

                if (is_big()) { big_size() = new_size; }

                return;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            new_data[_size] = c;
            new_data[new_size] = 0;

            deallocate_if_big();
            big_data() = new_data;
            big_size() = new_size;
            big_capacity() = new_size;
            set_big();
        }

        inline constexpr void pop_back() noexcept
        {
            fst_assert(size(), "basic_string::pop_back when empty.");

            if (is_small())
            {
                const size_type _size = size();
                _content.first()._data[_size] = 0;
            }
            else { big_data()[--(big_size())] = 0; }
        }

        //
        // insert
        //

        inline constexpr basic_string& insert(size_type index, size_type count, value_type c) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            fst_assert(index <= _size, "basic_string::insert index out of bounds.");

            if (index == _size) { return append(count, c); }

            const size_type new_size = _size + count;

            const size_type delta = _size - index;
            pointer _data = data();

            if (count <= _capacity - _size)
            {
                __fst::memmove((void*) (_data + index + count), (const void*) (_data + index), delta * sizeof(value_type));
                __fst::memfill(_data + index, c, count);
                _data[new_size] = 0;
                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            __fst::memmove((void*) (new_data + index + count), (const void*) (new_data + index), delta * sizeof(value_type));
            __fst::memfill(new_data + index, c, count);
            new_data[new_size] = 0;

            deallocate_if_big();
            big_data() = new_data;
            big_size() = new_size;
            big_capacity() = new_size;
            set_big();

            return *this;
        }

        inline constexpr basic_string& insert(size_type index, view_type str) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            fst_assert(index <= _size, "basic_string::insert index out of bounds.");

            if (index == _size) { return append(str); }

            const size_type count = str.size();
            const size_type new_size = _size + count;

            const size_type delta = _size - index;
            pointer _data = data();

            if (count <= _capacity - _size)
            {
                __fst::memmove((void*) (_data + index + count), (const void*) (_data + index), delta * sizeof(value_type));
                __fst::memmove(_data + index, str.data(), count * sizeof(value_type));
                _data[new_size] = 0;
                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            __fst::memmove((void*) (new_data + index + count), (const void*) (new_data + index), delta * sizeof(value_type));
            __fst::memmove(new_data + index, str.data(), count * sizeof(value_type));
            new_data[new_size] = 0;

            deallocate_if_big();
            big_data() = new_data;
            big_size() = new_size;
            big_capacity() = new_size;
            set_big();

            return *this;
        }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const basic_string& str) noexcept { return insert(index, str.view()); }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const_pointer s) noexcept { return insert(index, view_type(s)); }

        inline constexpr basic_string& insert(size_type index, view_type str, size_type index_str, size_type count = npos) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type str_size = str.size();

            fst_assert(index <= _size, "basic_string::insert index out of bounds.");
            fst_assert(index_str <= str_size, "basic_string::insert index_str out of bounds.");

            const size_type s_size = __fst::minimum(count, str_size - index_str);
            const size_type delta = _size - index;

            const size_type new_size = _size + s_size;

            pointer _data = data();

            if (s_size <= _capacity - _size)
            {
                __fst::memmove((void*) (_data + index + s_size), (const void*) (_data + index), delta * sizeof(value_type));
                __fst::memmove(_data + index, str.data() + index_str, s_size * sizeof(value_type));
                _data[new_size] = 0;
                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            __fst::memmove((void*) (new_data + index + s_size), (const void*) (_data + index), delta * sizeof(value_type));
            __fst::memmove(new_data + index, str.data() + index_str, s_size * sizeof(value_type));
            new_data[new_size] = 0;

            deallocate_if_big();
            big_data() = new_data;
            big_size() = new_size;
            big_capacity() = new_size;
            set_big();

            return *this;
        }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const basic_string& str, size_type index_str, size_type count = npos) noexcept
        {
            return insert(index, str.view(), index_str, count);
        }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const_pointer str, size_type index_str, size_type count = npos) noexcept
        {
            return insert(index, view_type(str), index_str, count);
        }

        //
        inline constexpr basic_string& erase(size_type index = 0, size_type count = npos) noexcept
        {
            const size_type _size = size();
            //const size_type _capacity = capacity();

            fst_assert(index <= _size, "basic_string::insert index out of bounds.");

            size_type s_size = __fst::minimum(count, _size - index);
            size_type delta = _size - s_size;

            pointer _data = data();
            __fst::memmove((void*) (_data + index), (const void*) (_data + index + s_size), delta * sizeof(value_type));

            const size_type new_size = _size - s_size;

            if (is_big()) { big_size() = new_size; }

            _data[new_size] = 0;
            return *this;
        }

        inline constexpr size_type find(value_type c) const noexcept
        {
            const value_type* ptr = traits_type::find(data(), size(), c);
            return ptr == nullptr ? npos : __fst::distance(begin(), ptr);
        }

        inline constexpr size_type find(view_type str) const noexcept
        {
            size_type n = size();
            size_type m = str.size();

            for (size_type i = 0; i <= n - m; ++i)
            {
                size_type j = 0;
                while (j < m && (*this)[i + j] == str[j])
                {
                    ++j;
                }
                if (j == m)
                { // match found
                    return i;
                }
            }
            return npos;
        }

        inline size_type find_last_of(value_type c) const noexcept
        {
            if (empty()) return npos;

            for (size_type i = size(); i != 0; i--)
            {

                if (data()[i - 1] == c) { return i - 1; }
            }

            return npos;
        }

        inline size_type find_last_of(view_type str) const noexcept
        {
            if (str.empty()) return npos; // specified in C++11
            if (empty()) return npos;

            const_iterator ret = end();
            const_iterator p_beg = begin();
            const_iterator p_end = end();

            const_iterator o_beg = str.begin();
            const_iterator o_end = str.end();

            while (p_beg != p_end)
            {
                const_iterator it1 = p_beg;
                const_iterator it2 = o_beg;

                while (*it1 == *it2)
                {

                    ++it1;
                    ++it2;

                    if (it2 == o_end)
                    {
                        ret = p_beg;
                        break;
                    }

                    if (it1 == p_end) return ret == end() ? npos : __fst::distance(begin(), ret);
                }
                ++p_beg;
            }
            return ret == end() ? npos : __fst::distance(begin(), ret);
        }

        // private:

        static constexpr size_t alignment = 16;

        struct alignas(alignment) big
        {
            pointer data;
            size_type size;

#ifdef FST_PACKED_STRING
            size_type capacity : sizeof(size_type) - sizeof(value_type);
            size_type padding : sizeof(value_type);
#else
            size_type capacity;
            size_type padding;
#endif
        };

        static constexpr size_t small_array_size = sizeof(big);
        static constexpr size_t small_capacity = small_array_size / sizeof(value_type) - sizeof(value_type);
        using small_array = __fst::array<char, small_array_size, alignment>;

        alignas(alignment) __fst::pair<small_array, allocator_type> _content;

        static_assert(sizeof(big) == sizeof(small_array), "wrong size");

        FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer big_data() const noexcept { return ((const big*) (const void*) _content.first()._data)->data; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type big_size() const noexcept { return ((const big*) (const void*) _content.first()._data)->size; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type big_capacity() const noexcept { return ((const big*) (const void*) _content.first()._data)->capacity; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer& big_data() noexcept { return ((big*) (void*) _content.first()._data)->data; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type& big_size() noexcept { return ((big*) (void*) _content.first()._data)->size; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type& big_capacity() noexcept { return ((big*) (void*) _content.first()._data)->capacity; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_small() const noexcept { return small_data()[small_capacity] == 0; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_big() const noexcept { return !is_small(); }

        FST_ALWAYS_INLINE void set_small() noexcept { small_data()[small_capacity] = 0; }
        FST_ALWAYS_INLINE void set_big() noexcept { small_data()[small_capacity] = 1; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer small_data() noexcept { return (pointer) _content.first()._data; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer small_data() const noexcept { return (const_pointer) _content.first().data(); }

        FST_ALWAYS_INLINE void deallocate_if_big() noexcept
        {
            if (is_big()) { _content.second().deallocate(big_data()); }
        }

        FST_NODISCARD FST_ALWAYS_INLINE pointer grow_copy(size_type new_size, size_type old_size) noexcept { return grow_copy(new_size, old_size, data()); }

        FST_NODISCARD inline pointer grow_copy(size_type new_size, size_type old_size, pointer _data) noexcept
        {
            const size_type alloc_length = new_size + 1;
            pointer new_data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(new_data, _data, old_size * sizeof(value_type));
            return new_data;
        }
    };

    template <class _CharT, class _Allocator>
    class basic_string<_CharT, _Allocator, __fst::enable_if_t<(sizeof(_CharT) > sizeof(char))>>
    {
      public:
        using value_type = _CharT;
        using traits_type = __fst::char_traits<_CharT>;
        using size_type = size_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;

        using allocator_type = _Allocator;
        using view_type = __fst::basic_string_view<value_type>;

        static constexpr size_type npos = (__fst::numeric_limits<size_type>::max)();

        basic_string(__fst::nullptr_t) = delete;

        inline constexpr basic_string() noexcept
            : _content{ content{}, allocator_type{} }
        {}

        inline explicit constexpr basic_string(const allocator_type& alloc) noexcept
            : _content(content{}, alloc)
        {}

        inline constexpr basic_string(size_type count, value_type c, const allocator_type& alloc = allocator_type()) noexcept
            : _content(content{}, alloc)
        {
            const size_type alloc_length = count + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memfill(_content.first().data, c, count);
            _content.first().data[count] = 0;
            _content.first().size = count;
            _content.first().capacity = count;
        }

        inline constexpr basic_string(const basic_string& str) noexcept
        {
            const size_type alloc_length = str.size() + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str.data(), alloc_length * sizeof(value_type));

            _content.first().size = alloc_length - 1;
            _content.first().capacity = alloc_length - 1;
        }

        inline constexpr basic_string(const basic_string& str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            const size_type alloc_length = str.size() + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str.data(), alloc_length * sizeof(value_type));

            _content.first().size = alloc_length - 1;
        }

        inline constexpr basic_string(basic_string&& str) noexcept
        {
            _content.first() = str._content.first();
            str._content.first() = content{};
        }

        inline constexpr basic_string(basic_string&& str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            _content.first() = str._content.first();
            str._content.first() = content{};
        }

        inline constexpr basic_string(view_type str) noexcept
        {
            const size_type len = str.size();

            const size_type alloc_length = len + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str.data(), len * sizeof(value_type));

            _content.first().data[len] = 0;
            _content.first().size = len;
            _content.first().capacity = len;
        }

        inline constexpr basic_string(view_type str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            const size_type len = str.size();

            const size_type alloc_length = len + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str.data(), len * sizeof(value_type));

            _content.first().data[len] = 0;
            _content.first().size = len;
            _content.first().capacity = len;
        }

        inline constexpr basic_string(const_pointer str) noexcept
        {
            const size_type len = traits_type::length(str);

            const size_type alloc_length = len + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str, alloc_length * sizeof(value_type));

            _content.first().size = len;
            _content.first().capacity = len;
        }

        inline constexpr basic_string(const_pointer str, const allocator_type& alloc) noexcept
            : _content({}, alloc)
        {
            const size_type len = traits_type::length(str);

            const size_type alloc_length = len + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str, alloc_length * sizeof(value_type));

            _content.first().size = len;
            _content.first().capacity = len;
        }

        inline constexpr basic_string(const_pointer str, size_type len) noexcept
        {

            const size_type alloc_length = len + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str, alloc_length * sizeof(value_type));

            _content.first().size = len;
            _content.first().capacity = len;
        }

        inline ~basic_string() noexcept { reset(); }

        inline constexpr basic_string& operator=(const basic_string& str) noexcept
        {
            reset();

            const size_type alloc_length = str.size() + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str.data(), alloc_length * sizeof(value_type));

            _content.first().size = alloc_length - 1;
            _content.first().capacity = alloc_length - 1;

            return *this;
        }

        inline constexpr basic_string& operator=(basic_string&& str) noexcept
        {
            reset();

            _content = str._content;
            str._content.first() = content{};
            return *this;
        }

        inline constexpr basic_string& operator=(view_type str) noexcept
        {
            reset();

            const size_type len = str.size();

            const size_type alloc_length = len + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str.data(), alloc_length * sizeof(value_type));

            _content.first().size = len;
            _content.first().capacity = len;

            return *this;
        }

        inline constexpr basic_string& operator=(const_pointer str) noexcept
        {
            reset();

            const size_type len = traits_type::length(str);

            const size_type alloc_length = len + 1;
            _content.first().data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(_content.first().data, str, alloc_length * sizeof(value_type));

            _content.first().size = len;
            _content.first().capacity = len;

            return *this;
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr allocator_type get_allocator() const noexcept { return _content.second(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer data() noexcept { return _content.first().data; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer data() const noexcept { return _content.first().data; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer c_str() const noexcept { return data(); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type size() const noexcept { return _content.first().size; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type length() const noexcept { return _content.first().size; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return size() == 0; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type capacity() const noexcept { return _content.first().capacity; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr view_type view() const noexcept { return view_type(data(), size()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_appendable(size_type count) const noexcept
        {
            return _content.first().size + count <= _content.first().capacity;
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference operator[](size_type n) noexcept
        {
            fst_assert(n < size() || (n == 0), "Index out of bounds");
            return data()[n];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference operator[](size_type n) const noexcept
        {
            fst_assert(n < size() || (n == 0), "Index out of bounds");
            return data()[n];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference front() noexcept
        {
            fst_assert(size() > 0, "basic_string::front when empty.");
            return data()[0];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference front() const noexcept
        {
            fst_assert(size() > 0, "basic_string::front when empty.");
            return data()[0];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr reference back() noexcept
        {
            fst_assert(size() > 0, "basic_string::back when empty.");
            return data()[size() - 1];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference back() const noexcept
        {
            fst_assert(size() > 0, "basic_string::back when empty.");
            return data()[size() - 1];
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr iterator begin() noexcept { return iterator(data()); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator begin() const noexcept { return const_iterator(data()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr iterator end() noexcept { return iterator(data() + size()); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator end() const noexcept { return const_iterator(data() + size()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator cbegin() const noexcept { return begin(); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr const_iterator cend() const noexcept { return end(); }

        inline void reset() noexcept
        {
            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first() = content{};
        }

        inline constexpr basic_string& operator+=(const basic_string& str) noexcept { return append(str); }
        inline constexpr basic_string& operator+=(view_type str) noexcept { return append(str); }
        inline constexpr basic_string& operator+=(value_type c) noexcept
        {
            push_back(c);
            return *this;
        }
        inline constexpr basic_string& operator+=(const_pointer str) noexcept { return append(str); }

        FST_NODISCARD constexpr basic_string substr(size_type _Off = 0, size_type _Count = npos) const noexcept
        {
            // return [_Off, _Off + _Count) as new string
            return basic_string(data() + _Off, __fst::minimum(_Count, size() - _Off));
        }

        constexpr basic_string& append(size_type count, value_type ch) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type avail_space = _capacity - _size;
            const size_type new_size = _size + count;

            pointer _data = data();

            if (count <= avail_space)
            {
                for (size_type i = 0; i < count; i++)
                {
                    _data[i + _size] = ch;
                }

                _data[new_size] = 0;

                _content.first().size = new_size;

                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);

            for (size_type i = 0; i < count; i++)
            {
                new_data[i + _size] = ch;
            }

            new_data[new_size] = 0;

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().size = new_size;
            _content.first().capacity = new_size;

            return *this;
        }

        constexpr basic_string& append(const_pointer str, size_type count) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type avail_space = _capacity - _size;
            const size_type new_size = _size + count;

            pointer _data = data();

            if (count <= avail_space)
            {
                for (size_type i = 0; i < count; i++)
                {
                    _data[i + _size] = str[i];
                }

                _data[new_size] = 0;

                _content.first().size = new_size;

                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);

            __fst::memmove(new_data + _size, str, count * sizeof(value_type));
            new_data[new_size] = 0;

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().size = new_size;
            _content.first().capacity = new_size;

            return *this;
        }

        FST_ALWAYS_INLINE constexpr basic_string& append(const_pointer str) noexcept { return append(str, traits_type::length(str)); }

        FST_ALWAYS_INLINE constexpr basic_string& append(const basic_string& str) noexcept { return append(str.c_str(), str.size()); }

        FST_ALWAYS_INLINE constexpr basic_string& append(view_type str) noexcept { return append(str.data(), str.size()); }

        FST_NODISCARD inline constexpr int compare(const basic_string& str) const noexcept
        {
            const size_type _size = size();
            const size_type ssize = str.size();

            if (_size != ssize) { return _size < ssize ? -1 : _size > ssize ? 1 : 0; }

            return traits_type::compare(data(), str.data(), __fst::minimum(_size, ssize));
        }

        FST_NODISCARD inline constexpr int compare(view_type str) const noexcept
        {
            const size_type _size = size();
            const size_type ssize = str.size();

            if (_size != ssize) { return _size < ssize ? -1 : _size > ssize ? 1 : 0; }

            return traits_type::compare(data(), str.data(), __fst::minimum(_size, ssize));
        }

        FST_NODISCARD inline constexpr int compare(const_pointer str) const noexcept
        {
            const size_type _size = size();
            const size_type ssize = traits_type::length(str);

            if (_size != ssize) { return _size < ssize ? -1 : _size > ssize ? 1 : 0; }

            return traits_type::compare(data(), str, __fst::minimum(_size, ssize));
        }

        FST_ALWAYS_INLINE constexpr void clear() noexcept
        {
            _content.first().data[0] = 0;
            _content.first().size = 0;
        }

        FST_ALWAYS_INLINE constexpr void resize(size_type new_size) noexcept { resize(new_size, value_type()); }

        inline constexpr void resize(size_type new_size, value_type c) noexcept
        {
            const size_type _size = size();

            if (new_size > _size) { append(new_size - _size, c); }
            else
            {
                if (!_content.first().data)
                {

                    const size_type alloc_length = new_size + 1;
                    _content.first().data = (pointer) _content.second().allocate(alloc_length);
                    __fst::memset(_content.first().data, 0, alloc_length * sizeof(value_type));

                    _content.first().capacity = new_size;
                    _content.first().size = new_size;
                }
                /*const size_type new_capacity = _size + count;

            pointer new_data = grow_copy(new_capacity, _size + 1);

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().capacity = new_capacity;*/

                //_content.first().size = new_size;
                //_content.first().data[new_size] = 0;
            }
        }

        inline void reserve(size_type count) noexcept
        {
            const size_type _capacity = capacity();

            if (count <= _capacity) { return; }

            const size_type _size = size();
            const size_type new_capacity = _size + count;

            pointer new_data = grow_copy(new_capacity, _size + 1);

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().capacity = new_capacity;
        }

        FST_ALWAYS_INLINE void reserve_extra(size_type count) noexcept { reserve(size() + count); }

        inline constexpr void push_back(value_type c) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type avail_space = _capacity - _size;
            const size_type new_size = _size + 1;

            pointer _data = data();

            if (avail_space)
            {
                _data[_size] = c;
                _data[new_size] = 0;

                _content.first().size = new_size;

                return;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            new_data[_size] = c;
            new_data[new_size] = 0;

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().size = new_size;
            _content.first().capacity = new_size;
        }

        inline constexpr void pop_back() noexcept
        {
            fst_assert(size(), "basic_string::pop_back when empty.");

            _content.first().data[--(_content.first().size)] = 0;
        }

        //
        // insert
        //

        inline constexpr basic_string& insert(size_type index, size_type count, value_type c) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            fst_assert(index <= _size, "basic_string::insert index out of bounds.");

            if (index == _size) { return append(count, c); }

            const size_type new_size = _size + count;

            const size_type delta = _size - index;
            pointer _data = data();

            if (count <= _capacity - _size)
            {
                __fst::memmove((void*) (_data + index + count), (const void*) (_data + index), delta * sizeof(value_type));
                __fst::memfill(_data + index, c, count);
                _data[new_size] = 0;
                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            __fst::memmove((void*) (new_data + index + count), (const void*) (new_data + index), delta * sizeof(value_type));
            __fst::memfill(new_data + index, c, count);
            new_data[new_size] = 0;

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().size = new_size;
            _content.first().capacity = new_size;

            return *this;
        }

        inline constexpr basic_string& insert(size_type index, view_type str) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            fst_assert(index <= _size, "basic_string::insert index out of bounds.");

            if (index == _size) { return append(str); }

            const size_type count = str.size();
            const size_type new_size = _size + count;

            const size_type delta = _size - index;
            pointer _data = data();

            if (count <= _capacity - _size)
            {
                __fst::memmove((void*) (_data + index + count), (const void*) (_data + index), delta * sizeof(value_type));
                __fst::memmove(_data + index, str.data(), count * sizeof(value_type));
                _data[new_size] = 0;
                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            __fst::memmove((void*) (new_data + index + count), (const void*) (new_data + index), delta * sizeof(value_type));
            __fst::memmove(new_data + index, str.data(), count * sizeof(value_type));
            new_data[new_size] = 0;

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().size = new_size;
            _content.first().capacity = new_size;

            return *this;
        }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const basic_string& str) noexcept { return insert(index, str.view()); }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const_pointer s) noexcept { return insert(index, view_type(s)); }

        inline constexpr basic_string& insert(size_type index, view_type str, size_type index_str, size_type count = npos) noexcept
        {
            const size_type _size = size();
            const size_type _capacity = capacity();
            const size_type str_size = str.size();

            fst_assert(index <= _size, "basic_string::insert index out of bounds.");
            fst_assert(index_str <= str_size, "basic_string::insert index_str out of bounds.");

            const size_type s_size = __fst::minimum(count, str_size - index_str);
            const size_type delta = _size - index;

            const size_type new_size = _size + s_size;

            pointer _data = data();

            if (s_size <= _capacity - _size)
            {
                __fst::memmove((void*) (_data + index + s_size), (const void*) (_data + index), delta * sizeof(value_type));
                __fst::memmove(_data + index, str.data() + index_str, s_size * sizeof(value_type));
                _data[new_size] = 0;
                return *this;
            }

            pointer new_data = grow_copy(new_size, _size, _data);
            __fst::memmove((void*) (new_data + index + s_size), (const void*) (_data + index), delta * sizeof(value_type));
            __fst::memmove(new_data + index, str.data() + index_str, s_size * sizeof(value_type));
            new_data[new_size] = 0;

            if (_content.first().data) { _content.second().deallocate(_content.first().data); }
            _content.first().data = new_data;
            _content.first().size = new_size;
            _content.first().capacity = new_size;

            return *this;
        }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const basic_string& str, size_type index_str, size_type count = npos) noexcept
        {
            return insert(index, str.view(), index_str, count);
        }

        FST_ALWAYS_INLINE constexpr basic_string& insert(size_type index, const_pointer str, size_type index_str, size_type count = npos) noexcept
        {
            return insert(index, view_type(str), index_str, count);
        }

        //
        inline constexpr basic_string& erase(size_type index = 0, size_type count = npos) noexcept
        {
            const size_type _size = size();

            fst_assert(index <= _size, "basic_string::insert index out of bounds.");

            size_type s_size = __fst::minimum(count, _size - index);
            size_type delta = _size - s_size;

            pointer _data = data();
            __fst::memmove((void*) (_data + index), (const void*) (_data + index + s_size), delta * sizeof(value_type));

            const size_type new_size = _size - s_size;

            _content.first().size = new_size;

            _data[new_size] = 0;
            return *this;
        }

        inline constexpr size_type find(view_type str) const noexcept
        {
            size_type n = size();
            size_type m = str.size();

            for (size_type i = 0; i <= n - m; ++i)
            {
                size_type j = 0;
                while (j < m && (*this)[i + j] == str[j])
                {
                    ++j;
                }
                if (j == m)
                { // match found
                    return i;
                }
            }
            return npos;
        }

        inline size_type find_last_of(value_type c) const noexcept
        {
            if (empty()) return npos;

            for (size_type i = size(); i != 0; i--)
            {

                if (data()[i - 1] == c) { return i - 1; }
            }

            return npos;
        }

        inline size_type find_last_of(view_type str) const noexcept
        {
            if (str.empty()) return npos; // specified in C++11
            if (empty()) return npos;

            const_iterator ret = end();
            const_iterator p_beg = begin();
            const_iterator p_end = end();

            const_iterator o_beg = str.begin();
            const_iterator o_end = str.end();

            while (p_beg != p_end)
            {
                const_iterator it1 = p_beg;
                const_iterator it2 = o_beg;

                while (*it1 == *it2)
                {

                    ++it1;
                    ++it2;

                    if (it2 == o_end)
                    {
                        ret = p_beg;
                        break;
                    }

                    if (it1 == p_end) return ret == end() ? npos : __fst::distance(begin(), ret);
                }
                ++p_beg;
            }
            return ret == end() ? npos : __fst::distance(begin(), ret);
        }

        friend inline __fst::output_stream<value_type>& operator<<(
            __fst::output_stream<value_type>& stream, const __fst::basic_string<value_type, allocator_type>& str) noexcept
        {
            stream.write(str.c_str(), str.size());
            return stream;
        }
        // private:

        static constexpr size_t alignment = 16;

        struct alignas(alignment) content
        {
            pointer data = nullptr;
            size_type size = 0;
            size_type capacity = 0;
            size_type padding;
        };

        alignas(alignment) __fst::pair<content, allocator_type> _content;

        FST_NODISCARD FST_ALWAYS_INLINE pointer grow_copy(size_type new_size, size_type old_size) noexcept { return grow_copy(new_size, old_size, data()); }

        FST_NODISCARD inline pointer grow_copy(size_type new_size, size_type old_size, pointer _data) noexcept
        {
            const size_type alloc_length = new_size + 1;
            pointer new_data = (pointer) _content.second().allocate(alloc_length);
            __fst::memmove(new_data, _data, old_size * sizeof(value_type));
            return new_data;
        }
    };

    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline constexpr bool operator==(const basic_string<_CharT, _LAllocator>& lhs, const basic_string<_CharT, _RAllocator>& rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator==(const basic_string<_CharT, _Allocator>& lhs, const _CharT* rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator==(const _CharT* lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return __fst::basic_string_view<_CharT>(lhs) == rhs.view();
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator==(const basic_string<_CharT, _Allocator>& lhs, __fst::basic_string_view<_CharT> rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator==(__fst::basic_string_view<_CharT> lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return lhs == rhs.view();
    }

    //
    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline constexpr bool operator!=(const basic_string<_CharT, _LAllocator>& lhs, const basic_string<_CharT, _RAllocator>& rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator!=(const basic_string<_CharT, _Allocator>& lhs, const _CharT* rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator!=(const _CharT* lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return __fst::basic_string_view<_CharT>(lhs) != rhs.view();
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator!=(const basic_string<_CharT, _Allocator>& lhs, __fst::basic_string_view<_CharT> rhs) noexcept
    {
        return lhs.view() != rhs;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator!=(__fst::basic_string_view<_CharT> lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return lhs != rhs.view();
    }

    //
    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline constexpr bool operator<(const basic_string<_CharT, _LAllocator>& lhs, const basic_string<_CharT, _RAllocator>& rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator<(const basic_string<_CharT, _Allocator>& lhs, const _CharT* rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator<(const _CharT* lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return __fst::basic_string_view<_CharT>(lhs) < rhs.view();
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator<(const basic_string<_CharT, _Allocator>& lhs, __fst::basic_string_view<_CharT> rhs) noexcept
    {
        return lhs.view() < rhs;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator<(__fst::basic_string_view<_CharT> lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return lhs < rhs.view();
    }

    //
    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline constexpr bool operator>(const basic_string<_CharT, _LAllocator>& lhs, const basic_string<_CharT, _RAllocator>& rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>(const basic_string<_CharT, _Allocator>& lhs, const _CharT* rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>(const _CharT* lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return __fst::basic_string_view<_CharT>(lhs) > rhs.view();
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>(const basic_string<_CharT, _Allocator>& lhs, __fst::basic_string_view<_CharT> rhs) noexcept
    {
        return lhs.view() > rhs;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>(__fst::basic_string_view<_CharT> lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return lhs > rhs.view();
    }

    //
    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline constexpr bool operator<=(const basic_string<_CharT, _LAllocator>& lhs, const basic_string<_CharT, _RAllocator>& rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    template <typename _CharT, class _Allocator>
    inline bool operator<=(const basic_string<_CharT, _Allocator>& lhs, const _CharT* rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator<=(const _CharT* lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return __fst::basic_string_view<_CharT>(lhs) <= rhs.view();
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator<=(const basic_string<_CharT, _Allocator>& lhs, __fst::basic_string_view<_CharT> rhs) noexcept
    {
        return lhs.view() <= rhs;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator<=(__fst::basic_string_view<_CharT> lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return lhs <= rhs.view();
    }

    //
    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline constexpr bool operator>=(const basic_string<_CharT, _LAllocator>& lhs, const basic_string<_CharT, _RAllocator>& rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>=(const basic_string<_CharT, _Allocator>& lhs, const _CharT* rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>=(const _CharT* lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return __fst::basic_string_view<_CharT>(lhs) >= rhs.view();
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>=(const basic_string<_CharT, _Allocator>& lhs, __fst::basic_string_view<_CharT> rhs) noexcept
    {
        return lhs.view() >= rhs;
    }

    template <typename _CharT, class _Allocator>
    inline constexpr bool operator>=(__fst::basic_string_view<_CharT> lhs, const basic_string<_CharT, _Allocator>& rhs) noexcept
    {
        return lhs >= rhs.view();
    }

    // operator +
    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline basic_string<_CharT, _LAllocator> operator+(const basic_string<_CharT, _LAllocator>& __lhs, const basic_string<_CharT, _RAllocator>& __rhs) noexcept
    {
        return basic_string<_CharT, _LAllocator>(__lhs).append(__rhs);
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(_CharT __lhs, const basic_string<_CharT, _Allocator>& __rhs)
    {
        basic_string<_CharT, _Allocator> s;
        s.push_back(__lhs);
        return s.append(__rhs);
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(const basic_string<_CharT, _Allocator>& __lhs, _CharT __rhs) noexcept
    {
        basic_string<_CharT, _Allocator> s = __lhs;
        s.push_back(__rhs);
        return s;
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(const _CharT* __lhs, const basic_string<_CharT, _Allocator>& __rhs) noexcept
    {
        return basic_string<_CharT, _Allocator>(__lhs).append(__rhs);
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(const basic_string<_CharT, _Allocator>& __lhs, const _CharT* __rhs) noexcept
    {
        return basic_string<_CharT, _Allocator>(__lhs).append(__rhs);
    }

    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline basic_string<_CharT, _LAllocator> operator+(basic_string<_CharT, _LAllocator>&& __lhs, const basic_string<_CharT, _RAllocator>& __rhs) noexcept
    {
        return __fst::move(__lhs.append(__rhs));
    }

    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline basic_string<_CharT, _LAllocator> operator+(const basic_string<_CharT, _LAllocator>& __lhs, basic_string<_CharT, _RAllocator>&& __rhs) noexcept
    {
        return __fst::move(__rhs.insert(0, __lhs));
    }

    template <typename _CharT, class _LAllocator, class _RAllocator>
    inline basic_string<_CharT, _LAllocator> operator+(basic_string<_CharT, _LAllocator>&& __lhs, basic_string<_CharT, _RAllocator>&& __rhs) noexcept
    {
        return __fst::move(__lhs.append(__rhs));
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(basic_string<_CharT, _Allocator>&& __lhs, const _CharT* __rhs) noexcept
    {
        return __fst::move(__lhs.append(__rhs));
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(const _CharT* __lhs, basic_string<_CharT, _Allocator>&& __rhs) noexcept
    {
        return __fst::move(__rhs.insert(0, __lhs));
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(_CharT __lhs, basic_string<_CharT, _Allocator>&& __rhs) noexcept
    {
        __rhs.insert(0, __lhs);
        return __fst::move(__rhs);
    }

    template <typename _CharT, class _Allocator>
    inline basic_string<_CharT, _Allocator> operator+(basic_string<_CharT, _Allocator>&& __lhs, _CharT __rhs) noexcept
    {
        __lhs.push_back(__rhs);
        return __fst::move(__lhs);
    }

    template <class _CharT, class _Allocator>
    inline __fst::output_stream<_CharT>& operator<<(__fst::output_stream<_CharT>& stream, const __fst::basic_string<_CharT, _Allocator>& str) noexcept
    {
        stream.write(str.data(), str.size());
        return stream;
    }

    using string = basic_string<char, __fst::allocator<char>>;
    using wstring = basic_string<wchar_t, __fst::allocator<wchar_t>>;
    using u16string = basic_string<char16_t, __fst::allocator<char16_t>>;
    using u32string = basic_string<char32_t, __fst::allocator<char32_t>>;

#if FST_HAS_CHAR8_T
    using u8string = basic_string<char8_t, __fst::allocator<char8_t>>;
#endif // FST_HAS_CHAR8_T

FST_END_NAMESPACE

#if FST_USE_STL
#include <ostream>
template <class _CharT, class _Traits, class _Allocator>
inline std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& stream, const __fst::basic_string<_CharT, _Allocator>& str)
{
    return stream.write(str.data(), str.size());
}
#endif
