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

#include "fst/utility.h"
#include "fst/memory.h"

FST_BEGIN_NAMESPACE

    ///
    template <size_t _Size, size_t _Alignment>
    struct aligned_storage
    {
        using value_type = __fst::byte;
        using pointer = __fst::byte*;
        using const_pointer = const __fst::byte*;
        using reference = __fst::byte&;
        using const_reference = __fst::byte;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        ///
        FST_NODISCARD static inline constexpr size_type size() noexcept { return _Size; }
        FST_NODISCARD static inline constexpr size_type alignment() noexcept { return _Alignment; }
        FST_NODISCARD static inline constexpr size_type aligned_size() noexcept { return sizeof(aligned_storage); }

        FST_NODISCARD inline constexpr __fst::byte* data() noexcept { return &_data[0]; }
        FST_NODISCARD inline constexpr const __fst::byte* data() const noexcept { return &_data[0]; }

        FST_NODISCARD inline constexpr __fst::byte* data(size_type index) noexcept
        {
            fst_assert(index < aligned_size(), "index out of bounds");
            return &_data[index];
        }

        FST_NODISCARD inline constexpr const __fst::byte* data(size_type index) const noexcept
        {
            fst_assert(index < aligned_size(), "index out of bounds");
            return &_data[index];
        }

        template <class T>
        FST_NODISCARD inline constexpr T* data() noexcept
        {
            return reinterpret_cast<T*>(data());
        }

        template <class T>
        FST_NODISCARD inline constexpr const T* data() const noexcept
        {
            return reinterpret_cast<const T*>(data());
        }

        template <class T>
        FST_NODISCARD inline constexpr T* data(size_type offset) noexcept
        {
            fst_assert(offset + sizeof(T) <= aligned_size(), "offset out of bounds");
            return reinterpret_cast<T*>(data(offset));
        }

        template <class T>
        FST_NODISCARD inline constexpr const T* data(size_type offset) const noexcept
        {
            fst_assert(offset + sizeof(T) <= aligned_size(), "offset out of bounds");
            return reinterpret_cast<const T*>(data(offset));
        }

        template <class T>
        FST_NODISCARD inline constexpr T& as(size_type offset = 0) noexcept
        {
            fst_assert(offset + sizeof(T) <= aligned_size(), "offset out of bounds");
            return *reinterpret_cast<T*>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr const T& as(size_type offset = 0) const noexcept
        {
            fst_assert(offset + sizeof(T) <= aligned_size(), "offset out of bounds");
            return *reinterpret_cast<const T*>(data() + offset);
        }

        FST_NODISCARD inline constexpr __fst::byte& operator[](size_type index) noexcept
        {
            fst_assert(index < aligned_size(), "index out of bounds");
            return _data[index];
        }

        FST_NODISCARD inline constexpr __fst::byte operator[](size_type index) const noexcept
        {
            fst_assert(index < aligned_size(), "index out of bounds");
            return _data[index];
        }

        template <size_t _OtherAlignment>
        FST_NODISCARD inline bool operator==(const aligned_storage<_Size, _OtherAlignment>& s) const noexcept
        {
            return !__fst::memcmp(data(), s.data(), size());
        }

        template <size_t _OtherAlignment>
        FST_NODISCARD inline bool operator!=(const aligned_storage<_Size, _OtherAlignment>& s) const noexcept
        {
            return __fst::memcmp(data(), s.data(), size());
        }

        alignas(_Alignment) __fst::byte _data[_Size];
    };

    ///
    template <size_t _Size>
    using byte_storage = aligned_storage<_Size, 1>;

    ///
    template <class T>
    struct aligned_type_storage
    {
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        FST_NODISCARD static inline constexpr size_type size() noexcept { return sizeof(T); }
        FST_NODISCARD static inline constexpr size_type alignment() noexcept { return alignof(T); }
        FST_NODISCARD static inline constexpr size_type aligned_size() noexcept { return sizeof(T); }

        FST_NODISCARD inline T* data() noexcept { return (T*) _data; }
        FST_NODISCARD inline const T* data() const noexcept { return (const T*) _data; }

        template <class... Args>
        inline T* construct(Args&&... args) noexcept
        {
            if constexpr (__fst::is_trivial_v<T>)
            {
                get() = T(__fst::forward<Args>(args)...);
                return data();
            }
            else { return fst_placement_new(_data) T(__fst::forward<Args>(args)...); }
        }

        inline void destroy() noexcept
        {
            if constexpr (!__fst::is_trivial_v<T>) { get().~T(); }
        }

        FST_NODISCARD inline T& get() noexcept { return *data(); }
        FST_NODISCARD inline const T& get() const noexcept { return *data(); }

        FST_NODISCARD inline operator T&() noexcept { return get(); }
        FST_NODISCARD inline operator const T&() const noexcept { return get(); }

      private:
        alignas(alignof(T)) __fst::byte _data[sizeof(T)];
    };

FST_END_NAMESPACE
