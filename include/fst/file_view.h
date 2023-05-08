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
#include "fst/status_code.h"
#include "fst/string_view.h"
#include "fst/memory_range.h"

FST_BEGIN_NAMESPACE

    class file_view
    {
      public:
        using value_type = uint8_t;
        using pointer = value_type*;
        using const_reference = value_type;
        using const_pointer = const value_type*;
        using iterator = const_pointer;
        using const_iterator = const_pointer;
        using size_type = size_t;

        file_view() noexcept = default;

        file_view(const file_view&) = delete;

        inline file_view(file_view&& fb) noexcept
            : _data(__fst::exchange(fb._data, nullptr))
            , _size(__fst::exchange(fb._size, (size_type) 0))
        {}

        inline ~file_view() noexcept { close(); }

        file_view& operator=(const file_view&) = delete;

        inline file_view& operator=(file_view&& fb) noexcept
        {
            close();
            _data = __fst::exchange(fb._data, nullptr);
            _size = __fst::exchange(fb._size, (size_type) 0);
            return *this;
        }

        FST_NODISCARD __fst::status open(const char* file_path) noexcept;

        void close() noexcept;

        FST_NODISCARD FST_ALWAYS_INLINE bool is_open() const noexcept { return _data && _size; }

        FST_NODISCARD FST_ALWAYS_INLINE bool empty() const noexcept { return !is_open(); }

        FST_NODISCARD FST_ALWAYS_INLINE size_type size() const noexcept { return _size; }

        FST_NODISCARD FST_ALWAYS_INLINE const_reference operator[](size_type __n) const noexcept
        {
            fst_assert(__n < size(), "index out of bounds");
            return _data[__n];
        }

        FST_NODISCARD FST_ALWAYS_INLINE const_reference front() const noexcept
        {
            fst_assert(is_open(), "front() called when empty");
            return *_data;
        }

        FST_NODISCARD FST_ALWAYS_INLINE const_reference back() const noexcept
        {
            fst_assert(is_open(), "back() called when empty");
            return *(_data + _size - 1);
        }

        FST_NODISCARD FST_ALWAYS_INLINE __fst::string_view str() const noexcept { return __fst::string_view((const char*) _data, _size); }

        FST_NODISCARD FST_ALWAYS_INLINE __fst::byte_view content() const noexcept { return __fst::byte_view((const __fst::byte_view::value_type*) _data, _size); }

        FST_NODISCARD FST_ALWAYS_INLINE iterator begin() const noexcept { return _data; }
        FST_NODISCARD FST_ALWAYS_INLINE iterator end() const noexcept { return _data + _size; }

        FST_NODISCARD FST_ALWAYS_INLINE const_iterator cbegin() const noexcept { return begin(); }
        FST_NODISCARD FST_ALWAYS_INLINE const_iterator cend() const noexcept { return end(); }

        FST_NODISCARD FST_ALWAYS_INLINE const_pointer data() const { return _data; }

        FST_NODISCARD FST_ALWAYS_INLINE const_pointer data(size_type offset) const noexcept
        {
            fst_assert(is_open(), "access nullptr");
            fst_assert(offset < size(), "offset out of bounds");
            return data() + offset;
        }

        template <size_t I>
        FST_NODISCARD FST_ALWAYS_INLINE const_pointer data() const noexcept
        {
            fst_assert(is_open(), "access nullptr");
            fst_assert(I < size(), "offset out of bounds");
            return data() + I;
        }

        FST_NODISCARD FST_ALWAYS_INLINE const_reference reversed(size_type index) const noexcept
        {
            fst_assert(index < size(), "index out of bounds");
            return data()[size() - index - 1];
        }

      private:
        pointer _data = nullptr;
        size_type _size = 0;
    };

FST_END_NAMESPACE
