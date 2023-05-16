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
#include "fst/string.h"
#include "fst/traits.h"
#include "fst/utility.h"

#include <ctype.h>

FST_BEGIN_NAMESPACE

    template <typename _CharT, size_t _Size>
    class basic_small_string
    {
      public:
        using __self = basic_small_string;
        using view_type = __fst::basic_string_view<_CharT>;
        using string_type = __fst::basic_string<_CharT>;
        using traits_type = __fst::char_traits<_CharT>;
        using value_type = _CharT;
        using size_type = size_t;
        using difference_type = ::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;

        static constexpr size_type maximum_size = _Size;
        static constexpr size_type npos = (__fst::numeric_limits<size_type>::max)();

        static_assert(!__fst::is_c_array_v<value_type>, "Character type of basic_small_string must not be an array.");
        static_assert(__fst::is_trivial<value_type>::value, "Character type of basic_small_string must be trivial.");
        static_assert(__fst::is_same<value_type, typename traits_type::char_type>::value, "traits_type::char_type must be the same type as value_type.");

        constexpr basic_small_string() noexcept = default;

        inline constexpr basic_small_string(size_type count, value_type ch) noexcept
        {
            fst_assert(count <= maximum_size, "basic_small_string count must be smaller or equal to "
                                              "maximum_size.");
            __fst::memfill(_data.data(), ch, count);

            _size = count;
            _data[_size] = 0;
        }

        inline constexpr basic_small_string(const basic_small_string& other) noexcept
            : _data(other._data)
            , _size(other._size)
        {}

        inline constexpr basic_small_string(basic_small_string&& other) noexcept
            : _data(__fst::move(other._data))
            , _size(other._size)
        {

            other._size = 0;
            other._data[0] = 0;
        }

        inline constexpr basic_small_string(const basic_small_string& other, size_type pos, size_type count = npos) noexcept
        {
            fst_assert(pos <= other._size, "basic_small_string pos must be smaller or equal to size.");
            _size = __fst::minimum(count, other._size - pos);

            __fst::memcpy(_data.data(), other.data() + pos, _size * sizeof(value_type));

            _data[_size] = 0;
        }

        template <size_t _OtherSize>
        inline constexpr basic_small_string(const basic_small_string<value_type, _OtherSize>& other) noexcept
        {
            fst_assert(other.size() <= maximum_size, "basic_small_string count must be smaller or equal to "
                                                     "maximum_size.");

            _size = other.size();
            __fst::memcpy(_data.data(), other.data(), _size * sizeof(value_type));
            _data[_size] = 0;
        }

        template <size_t _OtherSize>
        inline constexpr basic_small_string(const basic_small_string<value_type, _OtherSize>& other, size_type pos, size_type count = npos) noexcept
        {
            fst_assert(other.size() <= maximum_size, "basic_small_string size must be smaller or equal to "
                                                     "maximum_size.");
            fst_assert(pos <= other.size(), "basic_small_string pos must be smaller or equal to size.");
            _size = __fst::minimum(count, other.size() - pos);

            __fst::memcpy(_data.data(), other.data() + pos, _size * sizeof(value_type));

            _data[_size] = 0;
        }

        inline constexpr basic_small_string(const value_type* s) noexcept
        {
            _size = c_strlen(s);
            fst_assert(_size <= maximum_size, "basic_small_string c string must be smaller or equal to "
                                              "maximum_size.");
            __fst::memcpy(_data.data(), s, _size * sizeof(value_type));
            _data[_size] = 0;
        }

        inline constexpr basic_small_string(const value_type* s, size_type count) noexcept
        {
            fst_assert(count <= maximum_size, "basic_small_string count must be smaller or equal to "
                                              "maximum_size.");
            fst_assert(count <= c_strlen(s), "basic_small_string count must be smaller or equal to c string "
                                             "size.");
            _size = count;
            __fst::memcpy(_data.data(), s, _size * sizeof(value_type));

            _data[_size] = 0;
        }

        template <class InputIt>
        inline constexpr basic_small_string(InputIt first, InputIt last) noexcept
        {
            _size = last - first;
            fst_assert(_size <= maximum_size, "basic_small_string iteration distance must be smaller or "
                                              "equal to maximum_size.");

            __fst::memcpy(_data.data(), first, _size * sizeof(value_type));
            _data[_size] = 0;
        }

        inline constexpr basic_small_string(view_type v) noexcept
        {
            fst_assert(v.size() <= maximum_size, "basic_small_string view size must be smaller or equal to "
                                                 "maximum_size.");
            _size = v.size();
            __fst::memcpy(_data.data(), v.data(), _size * sizeof(value_type));

            _data[_size] = 0;
        }

        inline constexpr basic_small_string(view_type v, size_type pos, size_type count = npos) noexcept
        {
            fst_assert(pos <= v.size(), "basic_small_string pos must be smaller or equal to view "
                                        "size.");
            _size = __fst::minimum(count, v.size() - pos);
            fst_assert(_size <= maximum_size, "basic_small_string size must be smaller or equal to "
                                              "maximum_size.");

            __fst::memcpy(_data.data(), v.data() + pos, _size * sizeof(value_type));

            _data[_size] = 0;
        }

        inline constexpr basic_small_string(const string_type& s) noexcept
            : basic_small_string(view_type(s))
        {}

        inline constexpr basic_small_string(const string_type& s, size_type pos, size_type count = npos) noexcept
            : basic_small_string(view_type(s), pos, count)
        {}

        inline constexpr basic_small_string& operator=(const basic_small_string& other) noexcept
        {
            _data = other._data;
            _size = other._size;
            return *this;
        }

        inline constexpr basic_small_string& operator=(basic_small_string&& other) noexcept
        {
            _data = __fst::move(other._data);
            _size = other._size;
            other._size = 0;
            return *this;
        }

        inline constexpr basic_small_string& operator=(view_type v) noexcept
        {
            fst_assert(v.size() <= maximum_size, "basic_small_string view size must be smaller or equal to "
                                                 "maximum_size.");
            _size = v.size();
            __fst::memcpy(_data.data(), v.data(), _size * sizeof(value_type));

            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& operator=(const string_type& s) noexcept { return operator=(view_type(s)); }

        inline constexpr basic_small_string& operator=(const value_type* s) noexcept
        {
            _size = c_strlen(s);
            fst_assert(_size <= maximum_size, "basic_small_string c string must be smaller or equal to "
                                              "maximum_size.");

            __fst::memcpy(_data.data(), s, _size * sizeof(value_type));

            _data[_size] = 0;
            return *this;
        }

        // Iterators.
        inline constexpr iterator begin() noexcept { return iterator(_data.data()); }
        inline constexpr const_iterator begin() const noexcept { return const_iterator(_data.data()); }

        inline constexpr iterator end() noexcept { return iterator(_data.data() + _size); }
        inline constexpr const_iterator end() const noexcept { return const_iterator(_data.data() + _size); }

        inline constexpr const_iterator cbegin() const noexcept { return begin(); }
        inline constexpr const_iterator cend() const noexcept { return end(); }

        // Capacity.
        FST_NODISCARD inline constexpr size_type size() const noexcept { return _size; }
        FST_NODISCARD inline constexpr size_type length() const noexcept { return _size; }
        FST_NODISCARD inline constexpr size_type max_size() const noexcept { return maximum_size; }
        FST_NODISCARD inline constexpr size_type capacity() const noexcept { return maximum_size; }
        FST_NODISCARD inline constexpr bool empty() const noexcept { return _size == 0; }

        // Element access.
        inline constexpr reference operator[](size_type n) noexcept
        {
            fst_assert(n < maximum_size, "Index out of bounds");
            return _data[n];
        }

        inline constexpr const_reference operator[](size_type n) const noexcept
        {
            fst_assert(n < maximum_size, "Index out of bounds");
            return _data[n];
        }

        inline constexpr reference front() noexcept
        {
            fst_assert(_size > 0, "basic_small_string::front when empty.");
            return _data[0];
        }

        inline constexpr const_reference front() const noexcept
        {
            fst_assert(_size > 0, "basic_small_string::front when empty.");
            return _data[0];
        }

        inline constexpr reference back() noexcept
        {
            fst_assert(_size > 0, "basic_small_string::back when empty.");
            return _data[_size - 1];
        }

        inline constexpr const_reference back() const noexcept
        {
            fst_assert(_size > 0, "basic_small_string::back when empty.");
            return _data[_size - 1];
        }

        inline constexpr pointer data() noexcept { return _data.data(); }
        inline constexpr const_pointer data() const noexcept { return _data.data(); }

        inline constexpr const_pointer c_str() const noexcept { return _data.data(); }

        //
        // Operations.
        //

        inline constexpr void clear() noexcept
        {
            _data[0] = 0;
            _size = 0;
        }

        inline constexpr void push_back(value_type c) noexcept
        {
            fst_assert(_size + 1 <= maximum_size, "basic_small_string::push_back size would end up greather than "
                                                  "maximum_size.");
            _data[_size++] = c;
            _data[_size] = 0;
        }

        inline constexpr void pop_back() noexcept
        {
            fst_assert(_size, "basic_small_string::pop_back when empty.");
            _size--;
            _data[_size] = 0;
        }

        //
        // Append.
        //

        inline constexpr bool is_appendable(const basic_small_string& other) const noexcept { return _size + other.size() <= maximum_size; }

        inline constexpr bool is_appendable(const basic_small_string& other, size_type pos, size_type count = npos) const noexcept
        {
            return (pos <= other.size()) && (_size + __fst::minimum(count, other.size() - pos) <= maximum_size);
        }

        inline constexpr bool is_appendable(size_type count) const noexcept { return _size + count <= maximum_size; }

        inline constexpr bool is_appendable(view_type v) const noexcept { return _size + v.size() <= maximum_size; }

        inline constexpr bool is_appendable(view_type v, size_type pos, size_type count = npos) const noexcept
        {
            return (pos <= v.size()) && (_size + __fst::minimum(count, v.size() - pos) <= maximum_size);
        }

        inline constexpr basic_small_string& append(value_type c) noexcept
        {
            fst_assert(_size + 1 <= maximum_size, "basic_small_string::push_back size would end up greather than "
                                                  "maximum_size.");
            _data[_size++] = c;
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& append(size_type count, value_type c) noexcept
        {
            fst_assert(_size + count <= maximum_size, "basic_small_string::append size would end up greather than "
                                                      "maximum_size.");

            __fst::memfill(_data.data() + _size, c, count);
            _size += count;
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& append(const basic_small_string& other) noexcept
        {
            fst_assert(_size + other.size() <= maximum_size, "basic_small_string::append size would end up greather than "
                                                             "maximum_size.");

            __fst::memcpy(_data.data() + _size, other.data(), other.size() * sizeof(value_type));

            _size += other.size();
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& append(const basic_small_string& other, size_type pos, size_type count = npos) noexcept
        {
            fst_assert(pos <= other.size(), "basic_small_string pos must be smaller or equal to string "
                                            "size.");
            size_type o_size = __fst::minimum(count, other.size() - pos);
            fst_assert(_size + o_size <= maximum_size, "basic_small_string::append size would end up greather than "
                                                       "maximum_size.");

            __fst::memcpy(_data.data() + _size, other.data() + pos, o_size * sizeof(value_type));

            _size += o_size;
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& append(view_type v) noexcept
        {
            fst_assert(_size + v.size() <= maximum_size, "basic_small_string::append size would end up greather than "
                                                         "maximum_size.");

            __fst::memcpy(_data.data() + _size, v.data(), v.size() * sizeof(value_type));

            _size += v.size();
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& append(view_type v, size_type pos, size_type count = npos) noexcept
        {
            fst_assert(pos <= v.size(), "basic_small_string pos must be smaller or equal to view "
                                        "size.");
            size_type o_size = __fst::minimum(count, v.size() - pos);
            fst_assert(_size + o_size <= maximum_size, "basic_small_string::append size would end up greather than "
                                                       "maximum_size.");

            __fst::memcpy(_data.data() + _size, v.data() + pos, o_size * sizeof(value_type));

            _size += o_size;
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& append(const value_type* s) noexcept { return append(view_type(s)); }

        inline constexpr basic_small_string& append(const value_type* s, size_type pos, size_type count = npos) noexcept { return append(view_type(s), pos, count); }

        inline constexpr basic_small_string& append(const string_type& s) noexcept { return append(view_type(s)); }

        inline constexpr basic_small_string& append(const string_type& s, size_type pos, size_type count = npos) noexcept { return append(view_type(s), pos, count); }

        template <class InputIt>
        constexpr basic_small_string& append(InputIt first, InputIt last) noexcept
        {
            return append(view_type(first, last));
        }

        inline constexpr basic_small_string& operator+=(const basic_small_string& other) noexcept { return append(other); }

        inline constexpr basic_small_string& operator+=(view_type v) noexcept { return append(v); }

        inline constexpr basic_small_string& operator+=(const value_type* s) noexcept { return append(view_type(s)); }

        //
        // Insert.
        //
        inline constexpr basic_small_string& insert(size_type index, size_type count, value_type c)
        {
            fst_assert(index <= _size, "basic_small_string::insert index out of bounds.");
            fst_assert(count + _size <= maximum_size, "basic_small_string::insert size would end up greather than "
                                                      "maximum_size.");
            size_type delta = _size - index;
            __fst::memmove((void*) (_data.data() + index + count), (const void*) (_data.data() + index), delta * sizeof(value_type));
            // std::fill_n(_data.data() + index, count, c);
            __fst::memfill(_data.data() + index, c, count);
            _size += count;
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& insert(size_type index, view_type v)
        {
            fst_assert(index <= _size, "basic_small_string::insert index out of bounds.");
            fst_assert(v.size() + _size <= maximum_size, "basic_small_string::insert size would end up greather than "
                                                         "maximum_size.");
            size_type delta = _size - index;
            __fst::memmove((void*) (_data.data() + index + v.size()), (const void*) (_data.data() + index), delta * sizeof(value_type));

            __fst::memcpy(_data.data() + index, v.data(), v.size() * sizeof(value_type));

            _size += v.size();
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& insert(size_type index, view_type v, size_type count)
        {
            fst_assert(index <= _size, "basic_small_string::insert index out of bounds.");
            fst_assert(count <= v.size(), "basic_small_string::insert count out of bounds.");
            fst_assert(count + _size <= maximum_size, "basic_small_string::insert size would end up greather than "
                                                      "maximum_size.");
            size_type delta = _size - index;
            __fst::memmove((void*) (_data.data() + index + count), (const void*) (_data.data() + index), delta * sizeof(value_type));

            __fst::memcpy(_data.data() + index, v.data(), count * sizeof(value_type));

            _size += count;
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& insert(size_type index, view_type v, size_type index_str, size_type count)
        {
            fst_assert(index <= _size, "basic_small_string::insert index out of bounds.");
            fst_assert(index_str <= v.size(), "basic_small_string::insert index_str out of bounds.");
            size_type s_size = __fst::minimum(count, v.size() - index_str);
            fst_assert(s_size + _size <= maximum_size, "basic_small_string::insert size would end up greather than "
                                                       "maximum_size.");

            size_type delta = _size - index;
            __fst::memmove((void*) (_data.data() + index + s_size), (const void*) (_data.data() + index), delta * sizeof(value_type));

            __fst::memcpy(_data.data() + index, v.data() + index_str, s_size * sizeof(value_type));

            _size += s_size;
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& insert(size_type index, const value_type* s) { return insert(index, view_type(s)); }

        inline constexpr basic_small_string& insert(size_type index, const value_type* s, size_type count) { return insert(index, view_type(s), count); }

        inline constexpr basic_small_string& insert(size_type index, const basic_small_string& str)
        {
            fst_assert(index <= _size, "basic_small_string::insert index out of bounds.");
            fst_assert(str.size() + _size <= maximum_size, "basic_small_string::insert size would end up greather than "
                                                           "maximum_size.");
            size_type delta = _size - index;
            __fst::memmove((void*) (_data.data() + index + str.size()), (const void*) (_data.data() + index), delta * sizeof(value_type));

            __fst::memcpy(_data.data() + index, str.data(), str.size() * sizeof(value_type));

            _size += str.size();
            _data[_size] = 0;
            return *this;
        }

        inline constexpr basic_small_string& insert(size_type index, const basic_small_string& str, size_type index_str, size_type count = npos)
        {
            fst_assert(index <= _size, "basic_small_string::insert index out of bounds.");
            fst_assert(index_str <= str.size(), "basic_small_string::insert index_str out of bounds.");
            size_type s_size = __fst::minimum(count, str.size() - index_str);
            fst_assert(s_size + _size <= maximum_size, "basic_small_string::insert size would end up greather than "
                                                       "maximum_size.");

            size_type delta = _size - index;
            __fst::memmove((void*) (_data.data() + index + s_size), (const void*) (_data.data() + index), delta * sizeof(value_type));

            __fst::memcpy(_data.data() + index, str.data() + index_str, s_size * sizeof(value_type));

            _size += s_size;
            _data[_size] = 0;
            return *this;
        }

        //
        //
        //
        inline constexpr basic_small_string& erase(size_type index = 0, size_type count = npos)
        {
            fst_assert(index <= _size, "basic_small_string::insert index out of bounds.");
            size_type s_size = __fst::minimum(count, _size - index);
            size_type delta = _size - s_size;
            __fst::memmove((void*) (_data.data() + index), (const void*) (_data.data() + index + s_size), delta * sizeof(value_type));
            _size -= s_size;
            _data[_size] = 0;
            return *this;
        }

        //
        // Resize.
        //
        inline constexpr void resize(size_type count, value_type c)
        {
            fst_assert(count <= maximum_size, "basic_small_string::resize count must be smaller or equal to "
                                              "maximum_size.");
            if (count < _size)
            {
                _size = count;
                _data[_size] = 0;
            }
            else if (count > _size)
            {
                __fst::memfill(_data.data() + _size, c, (count - _size));
                _size = count;
                _data[_size] = 0;
            }
        }
        inline constexpr void resize(size_type count)
        {
            fst_assert(count <= maximum_size, "basic_small_string::resize count must be smaller or equal to "
                                              "maximum_size.");
            if (count < _size)
            {
                _size = count;
                _data[_size] = 0;
            }
            else if (count > _size)
            {
                _size = count;
                _data[_size] = 0;
            }
        }

        //
        //
        //
        inline constexpr basic_small_string& to_upper_case()
        {
            for (size_t i = 0; i < _size; i++)
            {
                _data[i] = (value_type)::toupper(_data[i]);
            }
            return *this;
        }

        inline constexpr basic_small_string& to_lower_case()
        {
            for (size_t i = 0; i < _size; i++)
            {
                _data[i] = (value_type)::tolower(_data[i]);
            }

            return *this;
        }

        //
        // Convert.
        //
        inline string_type to_string() const { return string_type(data(), size()); }
        inline operator string_type() const { return to_string(); }

        inline constexpr view_type view() const { return view_type(data(), size()); }
        inline constexpr operator view_type() const { return view(); }

      private:
        static constexpr size_type maximum_size_with_escape_char = maximum_size + 1;
        __fst::array<value_type, maximum_size_with_escape_char> _data;
        size_t _size = 0;

        inline constexpr size_type c_strlen(const value_type* str) noexcept { return *str ? 1 + c_strlen(str + 1) : 0; }
    };

    //
    // Operator ==
    //
    template <class _CharT, size_t _Size>
    inline bool operator==(const basic_small_string<_CharT, _Size>& __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        using view_type = typename basic_small_string<_CharT, _Size>::view_type;
        return (__lhs.size() == __rhs.size()) && (view_type(__lhs) == view_type(__rhs));
    }

    template <class _CharT, size_t _LSize, size_t _RSize>
    inline bool operator==(const basic_small_string<_CharT, _LSize>& __lhs, const basic_small_string<_CharT, _RSize>& __rhs) noexcept
    {
        using view_type = typename basic_small_string<_CharT, _LSize>::view_type;
        return (__lhs.size() == __rhs.size()) && (view_type(__lhs) == view_type(__rhs));
    }

    template <class _CharT, size_t _Size>
    inline bool operator==(const basic_small_string<_CharT, _Size>& __lhs, const _CharT* __rhs) noexcept
    {
        using view_type = typename basic_small_string<_CharT, _Size>::view_type;
        return view_type(__lhs) == view_type(__rhs);
    }

    template <class _CharT, size_t _Size>
    inline bool operator==(const _CharT* __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        using view_type = typename basic_small_string<_CharT, _Size>::view_type;
        return view_type(__lhs) == view_type(__rhs);
    }

    //
    // Operator !=
    //
    template <class _CharT, size_t _LSize, size_t _RSize>
    inline bool operator!=(const basic_small_string<_CharT, _LSize>& __lhs, const basic_small_string<_CharT, _RSize>& __rhs) noexcept
    {
        return !(__lhs == __rhs);
    }

    template <class _CharT, size_t _Size>
    inline bool operator!=(const basic_small_string<_CharT, _Size>& __lhs, const _CharT* __rhs) noexcept
    {
        return !(__lhs == __rhs);
    }

    template <class _CharT, size_t _Size>
    inline bool operator!=(const _CharT* __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        return !(__lhs == __rhs);
    }

    //
    // Operator <
    //
    template <class _CharT, size_t _LSize, size_t _RSize>
    inline bool operator<(const basic_small_string<_CharT, _LSize>& __lhs, const basic_small_string<_CharT, _RSize>& __rhs) noexcept
    {
        using view_type = typename basic_small_string<_CharT, _LSize>::view_type;
        return view_type(__lhs).compare(view_type(__rhs)) < 0;
    }

    template <class _CharT, size_t _Size>
    inline bool operator<(const basic_small_string<_CharT, _Size>& __lhs, const _CharT* __rhs) noexcept
    {
        using view_type = typename basic_small_string<_CharT, _Size>::view_type;
        return view_type(__lhs).compare(view_type(__rhs)) < 0;
    }

    template <class _CharT, size_t _Size>
    inline bool operator<(const _CharT* __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        using view_type = typename basic_small_string<_CharT, _Size>::view_type;
        return view_type(__lhs).compare(view_type(__rhs)) < 0;
    }

    //
    // Operator >
    //
    template <class _CharT, size_t _LSize, size_t _RSize>
    inline bool operator>(const basic_small_string<_CharT, _LSize>& __lhs, const basic_small_string<_CharT, _RSize>& __rhs) noexcept
    {
        return __rhs < __lhs;
    }

    template <class _CharT, size_t _Size>
    inline bool operator>(const basic_small_string<_CharT, _Size>& __lhs, const _CharT* __rhs) noexcept
    {
        return __rhs < __lhs;
    }

    template <class _CharT, size_t _Size>
    inline bool operator>(const _CharT* __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        return __rhs < __lhs;
    }

    //
    // Operator <=
    //
    template <class _CharT, size_t _LSize, size_t _RSize>
    inline bool operator<=(const basic_small_string<_CharT, _LSize>& __lhs, const basic_small_string<_CharT, _RSize>& __rhs) noexcept
    {
        return !(__rhs < __lhs);
    }

    template <class _CharT, size_t _Size>
    inline bool operator<=(const basic_small_string<_CharT, _Size>& __lhs, const _CharT* __rhs) noexcept
    {
        return !(__rhs < __lhs);
    }

    template <class _CharT, size_t _Size>
    inline bool operator<=(const _CharT* __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        return !(__rhs < __lhs);
    }

    //
    // Operator >=
    //
    template <class _CharT, size_t _LSize, size_t _RSize>
    inline bool operator>=(const basic_small_string<_CharT, _LSize>& __lhs, const basic_small_string<_CharT, _RSize>& __rhs) noexcept
    {
        return !(__lhs < __rhs);
    }

    template <class _CharT, size_t _Size>
    inline bool operator>=(const basic_small_string<_CharT, _Size>& __lhs, const _CharT* __rhs) noexcept
    {
        return !(__lhs < __rhs);
    }

    template <class _CharT, size_t _Size>
    inline bool operator>=(const _CharT* __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        return !(__lhs < __rhs);
    }

    // operator +
    template <class _CharT, size_t _LSize, size_t _RSize>
    inline basic_small_string<_CharT, _LSize> operator+(const basic_small_string<_CharT, _LSize>& __lhs, const basic_small_string<_CharT, _RSize>& __rhs) noexcept
    {
        basic_small_string<_CharT, _LSize> s = __lhs;
        s += __rhs;
        return s;
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(_CharT __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        basic_small_string<_CharT, _Size> s;
        s.push_back(__lhs);
        s.append(__rhs);
        return s;
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(const basic_small_string<_CharT, _Size>& __lhs, _CharT __rhs) noexcept
    {
        basic_small_string<_CharT, _Size> s = __lhs;
        s.push_back(__rhs);
        return s;
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(const _CharT* __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        basic_small_string<_CharT, _Size> s = __lhs;
        s += __rhs;
        return s;
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(const basic_small_string<_CharT, _Size>& __lhs, const _CharT* __rhs) noexcept
    {
        basic_small_string<_CharT, _Size> s = __lhs;
        s += __rhs;
        return s;
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(basic_small_string<_CharT, _Size>&& __lhs, const basic_small_string<_CharT, _Size>& __rhs) noexcept
    {
        return __fst::move(__lhs.append(__rhs));
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(const basic_small_string<_CharT, _Size>& __lhs, basic_small_string<_CharT, _Size>&& __rhs) noexcept
    {
        return __fst::move(__rhs.insert(0, __lhs));
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(basic_small_string<_CharT, _Size>&& __lhs, basic_small_string<_CharT, _Size>&& __rhs) noexcept
    {
        return __fst::move(__lhs.append(__rhs));
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(basic_small_string<_CharT, _Size>&& __lhs, const _CharT* __rhs) noexcept
    {
        return __fst::move(__lhs.append(__rhs));
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(const _CharT* __lhs, basic_small_string<_CharT, _Size>&& __rhs) noexcept
    {
        return __fst::move(__rhs.insert(0, __lhs));
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(_CharT __lhs, basic_small_string<_CharT, _Size>&& __rhs) noexcept
    {
        __rhs.insert(__rhs.begin(), __lhs);
        return __fst::move(__rhs);
    }

    template <class _CharT, size_t _Size>
    inline basic_small_string<_CharT, _Size> operator+(basic_small_string<_CharT, _Size>&& __lhs, _CharT __rhs) noexcept
    {
        __lhs.push_back(__rhs);
        return __fst::move(__lhs);
    }

    template <size_t N>
    using small_string = basic_small_string<char, N>;

FST_END_NAMESPACE
