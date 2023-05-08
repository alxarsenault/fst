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

#include "fst/iterator.h"
#include "fst/memory.h"
#include "fst/array.h"
#include "fst/traits.h"
#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    ///
    template <typename _Tp, size_t _Size = dynamic_size>
    class memory_range;

    template <class T>
    struct is_memory_range : __fst::false_t
    {};

    template <class T, size_t _Size>
    struct is_memory_range<__fst::memory_range<T, _Size>> : __fst::true_t
    {};
    //__fst::is_template_of<__fst::memory_range, T>;

    namespace detail
    {
        template <class _Tp, class _ElementType, class = void>
        struct is_memory_range_compatible_container : __fst::false_t
        {};

        template <class _Tp, class _ElementType>
        struct is_memory_range_compatible_container<_Tp, _ElementType,
            __fst::void_t<
                // is not a specialization of memory_range
                __fst::enable_if_t<!__fst::is_memory_range<_Tp>::value, __fst::nullptr_t>,
                // is not a specialization of array
                __fst::enable_if_t<!__fst::is_array<_Tp>::value, __fst::nullptr_t>,
                // is_array_v<Container> is false,
                __fst::enable_if_t<!__fst::is_c_array_v<_Tp>, __fst::nullptr_t>,
                // data(cont) and size(cont) are well formed
                decltype(__fst::container_data(__fst::declval<_Tp>())), decltype(__fst::container_size(__fst::declval<_Tp>())),
                // remove_pointer_t<decltype(data(cont))>(*)[] is convertible to
                // ElementType(*)[]
                __fst::enable_if_t<__fst::is_convertible_v<__fst::container_data_type_t<_Tp> (*)[], _ElementType (*)[]>, __fst::nullptr_t>>> : __fst::true_t
        {};

        template <class _Tp, class _ElementType>
        inline constexpr bool is_memory_range_compatible_container_v = is_memory_range_compatible_container<_Tp, _ElementType>::value;
    } // namespace detail.

    ///
    template <class _Tp>
    using memory_view = memory_range<__fst::add_const_t<__fst::remove_cv_t<_Tp>>>;

    ///
    using byte_range = memory_range<__fst::byte>;

    ///
    using byte_view = memory_view<__fst::byte>;

    /// @class memory_range
    template <class _Tp>
    class memory_range<_Tp, dynamic_size>
    {
      public:
        using __self = memory_range;
        using element_type = _Tp;

        using value_type = __fst::remove_cv_t<_Tp>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using pointer = element_type*;
        using const_pointer = const value_type*;
        using reference = element_type&;
        using const_reference = const value_type&;

        using iterator = __fst::iterator<pointer>;
        using const_iterator = __fst::iterator<const_pointer>;

        static constexpr bool is_const = __fst::is_const_v<element_type>;
        using const_range = __fst::conditional_t<is_const, memory_range, memory_range<const element_type>>;

      private:
        template <class OtherElementType>
        using to_other_pointer = __fst::conditional_t<is_const, __fst::add_const_t<__fst::remove_cv_t<OtherElementType>>*, OtherElementType*>;

        template <class OtherElementType>
        using to_other_reference = __fst::conditional_t<is_const, __fst::add_const_t<__fst::remove_cv_t<OtherElementType>>&, OtherElementType&>;

        template <class OtherElementType>
        using to_other_type = __fst::conditional_t<is_const, __fst::add_const_t<__fst::remove_cv_t<OtherElementType>>, OtherElementType>;

        template <class OtherElementType>
        using enable_if_convertible_t = __fst::enable_if_t<__fst::is_buffer_convertible<OtherElementType, element_type>::value, __fst::nullptr_t>;

        // template <class OtherElementType>
        // using enable_if_convertible_t =
        // __fst::enable_if_buffer_convertible_t<OtherElementType, element_type>;

        template <class Container>
        using enable_if_compatible_t = __fst::enable_if_t<detail::is_memory_range_compatible_container<Container, _Tp>::value, __fst::nullptr_t>;

      public:
        inline constexpr memory_range() noexcept FST_UNIT_TESTED
            : __data{ nullptr }
            , __size{ 0 }
        {}

        constexpr memory_range(const memory_range&) noexcept FST_UNIT_TESTED = default;
        constexpr memory_range& operator=(const memory_range&) noexcept = default;

        inline constexpr memory_range(pointer ptr, size_type count) FST_UNIT_TESTED
            : __data{ ptr }
            , __size{ count }
        {}

        inline constexpr memory_range(pointer first, pointer last) FST_UNIT_TESTED
            : __data{ first }
            , __size{ (size_type) __fst::distance(first, last) }
        {}

        template <size_t Size>
        inline constexpr memory_range(element_type (&__arr)[Size]) noexcept FST_UNIT_TESTED
            : __data{ __arr }
            , __size{ (size_type) Size }
        {}

        template <class U, size_t Size, enable_if_convertible_t<U> = nullptr>
        inline constexpr memory_range(__fst::array<U, Size>& arr) noexcept FST_UNIT_TESTED
            : __data{ arr.data() }
            , __size{ (size_type) Size }
        {}

        template <class U, size_t Size, enable_if_convertible_t<const U> = nullptr>
        inline constexpr memory_range(const __fst::array<U, Size>& arr) noexcept FST_UNIT_TESTED
            : __data{ arr.data() }
            , __size{ (size_type) Size }
        {}

        template <class Container>
        inline constexpr memory_range(Container& c, enable_if_compatible_t<Container> = nullptr) FST_UNIT_TESTED
            : __data{ __fst::container_data(c) }
            , __size{ (size_type) __fst::container_size(c) }
        {}

        template <class Container>
        inline constexpr memory_range(const Container& c, enable_if_compatible_t<const Container> = nullptr) FST_UNIT_TESTED
            : __data{ __fst::container_data(c) }
            , __size{ (size_type) __fst::container_size(c) }
        {}

        template <class U, size_t _OtherSize>
        inline constexpr memory_range(const memory_range<U, _OtherSize>& r, enable_if_convertible_t<U> = nullptr) noexcept FST_UNIT_TESTED
            : __data{ r.__data }
            , __size{ (size_type) r.size() }
        {}

        ~memory_range() noexcept = default;

        FST_NODISCARD inline constexpr size_type size() const noexcept FST_UNIT_TESTED { return __size; }

        FST_NODISCARD inline constexpr size_type size_bytes() const noexcept FST_UNIT_TESTED { return __size * sizeof(element_type); }

        FST_NODISCARD inline constexpr bool empty() const noexcept FST_UNIT_TESTED { return __size == 0; }

        FST_NODISCARD inline constexpr reference operator[](size_type __idx) noexcept FST_UNIT_TESTED
        {
            fst_assert(__idx < size(), "fst::memory_range::operator[] index out of bounds");
            return __data[__idx];
        }

        FST_NODISCARD inline constexpr const_reference operator[](size_type __idx) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__idx < size(), "fst::memory_range::operator[] index out of bounds");
            return __data[__idx];
        }

        FST_NODISCARD inline constexpr reference front() noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::front() on empty range");
            return __data[0];
        }

        FST_NODISCARD inline constexpr const_reference front() const noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::front() on empty range");
            return __data[0];
        }

        FST_NODISCARD inline constexpr reference back() noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::back() on empty range");
            return __data[size() - 1];
        }

        FST_NODISCARD inline constexpr const_reference back() const noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::back() on empty range");
            return __data[size() - 1];
        }

        FST_NODISCARD inline constexpr pointer data() noexcept FST_UNIT_TESTED { return __data; }
        FST_NODISCARD inline constexpr const_pointer data() const noexcept FST_UNIT_TESTED { return __data; }

        FST_NODISCARD inline constexpr pointer data(size_type index) noexcept FST_UNIT_TESTED
        {
            fst_assert(index < size(), "index out of bounds");
            return data() + index;
        }

        FST_NODISCARD inline constexpr const_pointer data(size_type index) const noexcept FST_UNIT_TESTED
        {
            fst_assert(index < size(), "index out of bounds");
            return data() + index;
        }

        template <class T>
        FST_NODISCARD inline constexpr to_other_pointer<T> data() noexcept FST_UNIT_TESTED
        {
            return reinterpret_cast<to_other_pointer<T>>(data());
        }

        template <class T>
        FST_NODISCARD inline constexpr const T* data() const noexcept FST_UNIT_TESTED
        {
            return reinterpret_cast<const T*>(data());
        }

        template <class T>
        FST_NODISCARD inline constexpr to_other_pointer<T> data(size_type offset) noexcept FST_UNIT_TESTED
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return reinterpret_cast<to_other_pointer<T>>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr const T* data(size_type offset) const noexcept FST_UNIT_TESTED
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return reinterpret_cast<const T*>(data() + offset);
        }

        FST_NODISCARD inline constexpr iterator begin() noexcept FST_UNIT_TESTED { return iterator(data()); }
        FST_NODISCARD inline constexpr const_iterator begin() const noexcept FST_UNIT_TESTED { return const_iterator(data()); }

        FST_NODISCARD inline constexpr iterator end() noexcept FST_UNIT_TESTED { return iterator(data() + size()); }
        FST_NODISCARD inline constexpr const_iterator end() const noexcept FST_UNIT_TESTED { return const_iterator(data() + size()); }

        FST_NODISCARD inline constexpr const_iterator cbegin() const noexcept FST_UNIT_TESTED { return begin(); }
        FST_NODISCARD inline constexpr const_iterator cend() const noexcept FST_UNIT_TESTED { return end(); }

        ///
        FST_NODISCARD inline constexpr memory_range subrange(size_type __offset, size_type __count = (__fst::numeric_limits<size_type>::max)()) noexcept FST_UNIT_TESTED
        {
            fst_assert(__offset <= size(), "Offset out of range in fst::memory_range::subrange(offset, "
                                           "count)");
            __count = __fst::minimum(size() - __offset, __count);
            return { data() + __offset, __count };
        }

        FST_NODISCARD inline constexpr const_range subrange(
            size_type __offset, size_type __count = (__fst::numeric_limits<size_type>::max)()) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__offset <= size(), "Offset out of range in fst::memory_range::subrange(offset, "
                                           "count)");
            __count = __fst::minimum(size() - __offset, __count);
            return { data() + __offset, __count };
        }

        FST_NODISCARD inline constexpr memory_range first(size_type __count) noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::first(count)");
            return { data(), __count };
        }

        FST_NODISCARD inline constexpr const_range first(size_type __count) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::first(count)");
            return { data(), __count };
        }

        FST_NODISCARD inline constexpr memory_range last(size_type __count) noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::last(count)");
            return { data() + size() - __count, __count };
        }

        FST_NODISCARD inline constexpr const_range last(size_type __count) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::last(count)");
            return { data() + size() - __count, __count };
        }

        //

        FST_NODISCARD inline memory_view<element_type> view() const noexcept { return *this; }

        FST_NODISCARD inline memory_range<const __fst::byte> byte_view() const noexcept { return { reinterpret_cast<const __fst::byte*>(data()), size_bytes() }; }

        FST_NODISCARD inline memory_range<to_other_type<__fst::byte>> byte_range() noexcept
        {
            return { reinterpret_cast<to_other_pointer<__fst::byte>>(data()), size_bytes() };
        }

        FST_NODISCARD inline memory_range<const __fst::byte> byte_range() const noexcept { return { reinterpret_cast<const __fst::byte*>(data()), size_bytes() }; }

        template <class T>
        FST_NODISCARD inline memory_range<to_other_type<T>> cast_range() noexcept
        {
            fst_assert((size_bytes() % sizeof(T)) == 0);
            return { reinterpret_cast<to_other_pointer<T>>(data()), size_bytes() / sizeof(T) };
        }

        template <class T>
        FST_NODISCARD inline memory_range<const T> cast_range() const noexcept
        {
            fst_assert((size_bytes() % sizeof(T)) == 0);
            return { reinterpret_cast<const T*>(data()), size_bytes() / sizeof(T) };
        }

        ///
        template <class T>
        FST_NODISCARD inline constexpr to_other_reference<T> as(size_type offset = 0) noexcept
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<to_other_pointer<T>>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr const T& as(size_type offset = 0) const noexcept
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<const T*>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr T& as(const_iterator pos) noexcept
        {
            const difference_type offset = pos - begin();
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<to_other_pointer<T>>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr const T& as(const_iterator pos) const noexcept
        {
            const difference_type offset = pos - begin();

            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<const T*>(data() + offset);
        }

        template <class EndianTag, class T, __fst::enable_if_t<__fst::is_endian_tag_v<EndianTag>, int> = 0>
        FST_NODISCARD inline constexpr T as(size_type offset = 0) const noexcept
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            if constexpr (__fst::is_same_v<__fst::little_endian_tag, EndianTag>) { return *reinterpret_cast<const T*>(data() + offset); }
            else { return __fst::byte_swap(*reinterpret_cast<const T*>(data() + offset)); }
        }

      private:
        pointer __data;
        size_type __size;

        template <class T, size_t Size>
        friend class memory_range;
    };

    template <class _Tp, size_t _Size>
    class memory_range
    {
      public:
        using __self = memory_range;
        using element_type = _Tp;

        using value_type = __fst::remove_cv_t<_Tp>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using pointer = element_type*;
        using const_pointer = const value_type*;
        using reference = element_type&;
        using const_reference = const value_type&;

        using iterator = __fst::iterator<pointer>;
        using const_iterator = __fst::iterator<const_pointer>;

        static constexpr bool is_const = __fst::is_const_v<element_type>;
        using const_range = __fst::conditional_t<is_const, memory_range, memory_range<const element_type>>;

      private:
        template <class OtherElementType>
        using to_other_pointer = __fst::conditional_t<is_const, __fst::add_const_t<__fst::remove_cv_t<OtherElementType>>*, OtherElementType*>;

        template <class OtherElementType>
        using to_other_reference = __fst::conditional_t<is_const, __fst::add_const_t<__fst::remove_cv_t<OtherElementType>>&, OtherElementType&>;

        template <class OtherElementType>
        using to_other_type = __fst::conditional_t<is_const, __fst::add_const_t<__fst::remove_cv_t<OtherElementType>>, OtherElementType>;

        template <class OtherElementType>
        using enable_if_convertible_t = __fst::enable_if_t<__fst::is_buffer_convertible<OtherElementType, element_type>::value, __fst::nullptr_t>;

        template <class Container>
        using enable_if_compatible_t = __fst::enable_if_t<detail::is_memory_range_compatible_container<Container, _Tp>::value, __fst::nullptr_t>;

      public:
        inline constexpr memory_range() noexcept FST_UNIT_TESTED : __data{ nullptr } {}

        constexpr memory_range(const memory_range&) noexcept FST_UNIT_TESTED = default;
        constexpr memory_range& operator=(const memory_range&) noexcept = default;

        inline constexpr memory_range(pointer first) FST_UNIT_TESTED : __data{ first } {}

        template <size_t Size>
        inline constexpr memory_range(element_type (&__arr)[Size]) noexcept FST_UNIT_TESTED : __data{ __arr }
        {}

        template <class U, size_t Size, enable_if_convertible_t<U> = nullptr>
        inline constexpr memory_range(__fst::array<U, Size>& arr) noexcept FST_UNIT_TESTED : __data{ arr.data() }
        {}

        template <class U, size_t Size, enable_if_convertible_t<const U> = nullptr>
        inline constexpr memory_range(const __fst::array<U, Size>& arr) noexcept FST_UNIT_TESTED : __data{ arr.data() }
        {}

        template <class Container>
        inline constexpr memory_range(Container& c, enable_if_compatible_t<Container> = nullptr) FST_UNIT_TESTED : __data{ __fst::container_data(c) }
        {}

        template <class Container>
        inline constexpr memory_range(const Container& c, enable_if_compatible_t<const Container> = nullptr) FST_UNIT_TESTED : __data{ __fst::container_data(c) }
        {}

        template <class U, size_t _OtherSize>
        inline constexpr memory_range(const memory_range<U, _OtherSize>& r, enable_if_convertible_t<U> = nullptr) noexcept FST_UNIT_TESTED : __data{ r.__data }
        {}

        ~memory_range() noexcept = default;

        FST_NODISCARD static inline constexpr size_type size() noexcept FST_UNIT_TESTED { return _Size; }

        FST_NODISCARD static inline constexpr size_type size_bytes() noexcept FST_UNIT_TESTED { return _Size * sizeof(element_type); }

        FST_NODISCARD static inline constexpr bool empty() noexcept FST_UNIT_TESTED { return _Size == 0; }

        FST_NODISCARD inline constexpr reference operator[](size_type __idx) noexcept FST_UNIT_TESTED
        {
            fst_assert(__idx < size(), "fst::memory_range::operator[] index out of bounds");
            return __data[__idx];
        }

        FST_NODISCARD inline constexpr const_reference operator[](size_type __idx) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__idx < size(), "fst::memory_range::operator[] index out of bounds");
            return __data[__idx];
        }

        FST_NODISCARD inline constexpr reference front() noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::front() on empty range");
            return __data[0];
        }

        FST_NODISCARD inline constexpr const_reference front() const noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::front() on empty range");
            return __data[0];
        }

        FST_NODISCARD inline constexpr reference back() noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::back() on empty range");
            return __data[size() - 1];
        }

        FST_NODISCARD inline constexpr const_reference back() const noexcept FST_UNIT_TESTED
        {
            fst_assert(!empty(), "fst::memory_range::back() on empty range");
            return __data[size() - 1];
        }

        FST_NODISCARD inline constexpr pointer data() noexcept FST_UNIT_TESTED { return __data; }
        FST_NODISCARD inline constexpr const_pointer data() const noexcept FST_UNIT_TESTED { return __data; }

        FST_NODISCARD inline constexpr pointer data(size_type index) noexcept FST_UNIT_TESTED
        {
            fst_assert(index < size(), "index out of bounds");
            return data() + index;
        }

        FST_NODISCARD inline constexpr const_pointer data(size_type index) const noexcept FST_UNIT_TESTED
        {
            fst_assert(index < size(), "index out of bounds");
            return data() + index;
        }

        template <class T>
        FST_NODISCARD inline constexpr to_other_pointer<T> data() noexcept FST_UNIT_TESTED
        {
            return reinterpret_cast<to_other_pointer<T>>(data());
        }

        template <class T>
        FST_NODISCARD inline constexpr const T* data() const noexcept FST_UNIT_TESTED
        {
            return reinterpret_cast<const T*>(data());
        }

        template <class T>
        FST_NODISCARD inline constexpr to_other_pointer<T> data(size_type offset) noexcept FST_UNIT_TESTED
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return reinterpret_cast<to_other_pointer<T>>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr const T* data(size_type offset) const noexcept FST_UNIT_TESTED
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return reinterpret_cast<const T*>(data() + offset);
        }

        FST_NODISCARD inline constexpr iterator begin() noexcept FST_UNIT_TESTED { return iterator(data()); }
        FST_NODISCARD inline constexpr const_iterator begin() const noexcept FST_UNIT_TESTED { return const_iterator(data()); }

        FST_NODISCARD inline constexpr iterator end() noexcept FST_UNIT_TESTED { return iterator(data() + size()); }
        FST_NODISCARD inline constexpr const_iterator end() const noexcept FST_UNIT_TESTED { return const_iterator(data() + size()); }

        FST_NODISCARD inline constexpr const_iterator cbegin() const noexcept FST_UNIT_TESTED { return begin(); }
        FST_NODISCARD inline constexpr const_iterator cend() const noexcept FST_UNIT_TESTED { return end(); }

        ///
        FST_NODISCARD inline constexpr memory_range subrange(size_type __offset, size_type __count = (__fst::numeric_limits<size_type>::max)()) noexcept FST_UNIT_TESTED
        {
            fst_assert(__offset <= size(), "Offset out of range in fst::memory_range::subrange(offset, "
                                           "count)");
            __count = __fst::minimum(size() - __offset, __count);
            return { data() + __offset, __count };
        }

        FST_NODISCARD inline constexpr const_range subrange(
            size_type __offset, size_type __count = (__fst::numeric_limits<size_type>::max)()) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__offset <= size(), "Offset out of range in fst::memory_range::subrange(offset, "
                                           "count)");
            __count = __fst::minimum(size() - __offset, __count);
            return { data() + __offset, __count };
        }

        FST_NODISCARD inline constexpr memory_range first(size_type __count) noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::first(count)");
            return { data(), __count };
        }

        FST_NODISCARD inline constexpr const_range first(size_type __count) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::first(count)");
            return { data(), __count };
        }

        FST_NODISCARD inline constexpr memory_range last(size_type __count) noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::last(count)");
            return { data() + size() - __count, __count };
        }

        FST_NODISCARD inline constexpr const_range last(size_type __count) const noexcept FST_UNIT_TESTED
        {
            fst_assert(__count <= size(), "Count out of range in fst::memory_range::last(count)");
            return { data() + size() - __count, __count };
        }

        //

        FST_NODISCARD inline memory_view<element_type> view() const noexcept { return *this; }

        FST_NODISCARD inline memory_range<const __fst::byte> byte_view() const noexcept { return { reinterpret_cast<const __fst::byte*>(data()), size_bytes() }; }

        FST_NODISCARD inline memory_range<to_other_type<__fst::byte>> byte_range() noexcept
        {
            return { reinterpret_cast<to_other_pointer<__fst::byte>>(data()), size_bytes() };
        }

        FST_NODISCARD inline memory_range<const __fst::byte> byte_range() const noexcept { return { reinterpret_cast<const __fst::byte*>(data()), size_bytes() }; }

        template <class T>
        FST_NODISCARD inline memory_range<to_other_type<T>> cast_range() noexcept
        {
            fst_assert((size_bytes() % sizeof(T)) == 0);
            return { reinterpret_cast<to_other_pointer<T>>(data()), size_bytes() / sizeof(T) };
        }

        template <class T>
        FST_NODISCARD inline memory_range<const T> cast_range() const noexcept
        {
            fst_assert((size_bytes() % sizeof(T)) == 0);
            return { reinterpret_cast<const T*>(data()), size_bytes() / sizeof(T) };
        }

        ///
        template <class T>
        FST_NODISCARD inline constexpr to_other_reference<T> as(size_type offset = 0) noexcept
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<to_other_pointer<T>>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr const T& as(size_type offset = 0) const noexcept
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<const T*>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr T& as(const_iterator pos) noexcept
        {
            const difference_type offset = pos - begin();
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<to_other_pointer<T>>(data() + offset);
        }

        template <class T>
        FST_NODISCARD inline constexpr const T& as(const_iterator pos) const noexcept
        {
            const difference_type offset = pos - begin();

            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            return *reinterpret_cast<const T*>(data() + offset);
        }

        template <class EndianTag, class T, __fst::enable_if_t<__fst::is_endian_tag_v<EndianTag>, int> = 0>
        FST_NODISCARD inline constexpr T as(size_type offset = 0) const noexcept
        {
            fst_assert((sizeof(value_type) * offset + sizeof(T)) <= size_bytes(), "offset out of bounds");
            if constexpr (__fst::is_same_v<__fst::little_endian_tag, EndianTag>) { return *reinterpret_cast<const T*>(data() + offset); }
            else { return __fst::byte_swap(*reinterpret_cast<const T*>(data() + offset)); }
        }

      private:
        pointer __data;

        template <class T, size_t Size>
        friend class memory_range;
    };

    ///
    //template <class _T, size_t _Size, size_t _Alignment = 32>
    //class aligned_memory_range_base
    //{
    //  public:
    //    static_assert(__fst::is_power_of_two(_Size), "_Size must be a power of two");
    //    static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");
    //
    //    FST_ALWAYS_INLINE constexpr aligned_memory_range_base(_T* p = nullptr) noexcept
    //        : _data(p)
    //    {
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //    }
    //
    //    constexpr aligned_memory_range_base(const aligned_memory_range_base&) noexcept = default;
    //
    //    FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t size() noexcept { return _Size; }
    //    FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return _Alignment; }
    //
    //    FST_ALWAYS_INLINE constexpr void reset() noexcept { _data = nullptr; }
    //
    //    FST_ALWAYS_INLINE constexpr void reset(_T* p) noexcept
    //    {
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //        _data = p;
    //    }
    //
    //    _T* _data = nullptr;
    //};
    //
    //template <class _T, size_t _Alignment>
    //class aligned_memory_range_base<_T, (__fst::numeric_limits<size_t>::max) (), _Alignment>
    //{
    //  public:
    //    static_assert(__fst::is_power_of_two(_Alignment), "_Alignment must be a power of two");
    //
    //    constexpr aligned_memory_range_base() noexcept = default;
    //
    //    FST_ALWAYS_INLINE constexpr aligned_memory_range_base(_T* p, size_t __size) noexcept
    //        : _data(p)
    //        , _size(__size)
    //    {
    //        fst_assert(p || (!p && __size == 0), "nullptr with size");
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //    }
    //
    //    FST_ALWAYS_INLINE constexpr aligned_memory_range_base(__fst::nullptr_t, size_t) noexcept
    //        : _data(nullptr)
    //        , _size(0)
    //    {}
    //
    //    template <class _TT, size_t _TSize, size_t _TAlignment, __fst::enable_if_t<__fst::is_convertible_v<_TT*, _T*>, int> = 0>
    //    FST_ALWAYS_INLINE constexpr aligned_memory_range_base(const aligned_memory_range_base<_TT, _TSize, _TAlignment>& r) noexcept
    //        : _data(r._data)
    //        , _size(r.size())
    //    {}
    //
    //    constexpr aligned_memory_range_base(const aligned_memory_range_base&) noexcept = default;
    //    constexpr aligned_memory_range_base& operator=(const aligned_memory_range_base&) noexcept = default;
    //
    //    FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
    //    FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return _Alignment; }
    //
    //    FST_ALWAYS_INLINE constexpr void reset() noexcept
    //    {
    //        _data = nullptr;
    //        _size = 0;
    //    }
    //
    //    FST_ALWAYS_INLINE constexpr void reset(_T* p, size_t __size) noexcept
    //    {
    //        fst_assert(!p || __fst::is_aligned(p, _Alignment), "Wrong alignment");
    //        _data = p;
    //        _size = __size;
    //    }
    //
    //    _T* _data = nullptr;
    //    size_t _size = 0;
    //};
    //
    /////
    //template <class _T, size_t _Size, size_t _Alignment = 32>
    //class aligned_memory_range : public contiguous_container_base<_T, aligned_memory_range_base<_T, _Size, _Alignment>>
    //{
    //  public:
    //    using base = contiguous_container_base<_T, aligned_memory_range_base<_T, _Size, _Alignment>>;
    //    using base::base;
    //    using value_type = typename base::value_type;
    //    using size_type = typename base::size_type;
    //    using difference_type = typename base::difference_type;
    //    using pointer = typename base::pointer;
    //    using const_pointer = typename base::const_pointer;
    //    using reference = typename base::reference;
    //    using const_reference = typename base::const_reference;
    //    using iterator = typename base::iterator;
    //    using const_iterator = typename base::const_iterator;
    //};
    //
    //template <class T>
    //struct is_aligned_memory_range : __fst::false_t
    //{};
    //
    //template <class _T, size_t _Size, size_t _Alignment>
    //struct is_aligned_memory_range<aligned_memory_range<_T, _Size, _Alignment>> : __fst::true_t
    //{};

FST_END_NAMESPACE
