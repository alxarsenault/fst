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
#include "fst/traits.h"
#include "fst/memory.h"
#include "fst/string.h"
#include "fst/utility.h"
#include "fst/small_vector.h"
#include "fst/vector.h"

FST_BEGIN_NAMESPACE

    // Only works with little endian.
    template <class _VectorType>
    class basic_byte_vector : public _VectorType
    {
      public:
        static_assert(__fst::is_same_v<typename _VectorType::value_type, __fst::byte>, //
            "vector base must have fst::byte as value_type");

        using __self = basic_byte_vector;
        using value_type = __fst::byte;
        using buffer_type = _VectorType;
        using reference = typename buffer_type::reference;
        using const_reference = typename buffer_type::const_reference;
        using pointer = typename buffer_type::pointer;
        using const_pointer = typename buffer_type::const_pointer;
        using iterator = typename buffer_type::iterator;
        using const_iterator = typename buffer_type::const_iterator;
        using size_type = typename buffer_type::size_type;
        using difference_type = typename buffer_type::difference_type;

        enum class convert_options {
            pcm_8_bit,
            pcm_16_bit,
            pcm_24_bit,
            pcm_32_bit,
        };

        //
        // MARK: Constructors.
        //
        using buffer_type::buffer_type;

        FST_DECLARE_DEFAULT_CTOR(basic_byte_vector);

        inline basic_byte_vector(__fst::string_view data)
            : buffer_type((const_pointer) data.data(), (const_pointer) data.data() + data.size())
        {}

        using buffer_type::operator=;

        //
        // MARK: Iterators.
        //
        using buffer_type::begin;
        using buffer_type::end;
        using buffer_type::cbegin;
        using buffer_type::cend;
        //
        // MARK: Capacity.
        //
        using buffer_type::size;
        using buffer_type::max_size;
        using buffer_type::empty;

        //
        // MARK: Resize, reserve and clear.
        //
        using buffer_type::resize;
        using buffer_type::reserve;
        using buffer_type::clear;

        //
        // MARK: Element access.
        //
        using buffer_type::operator[];
        using buffer_type::front;
        using buffer_type::back;
        using buffer_type::data;

        inline pointer data(size_type __index) noexcept { return buffer_type::data() + __index; }
        inline const_pointer data(size_type __index) const noexcept { return buffer_type::data() + __index; }

        template <typename T>
        inline T* data() noexcept
        {
            return __fst::launder(reinterpret_cast<T*>(buffer_type::data()));
        }

        template <typename T>
        inline const T* data() const noexcept
        {
            return __fst::launder(reinterpret_cast<const T*>(buffer_type::data()));
        }

        template <typename T>
        inline T* data(size_type __index) noexcept
        {
            return __fst::launder(reinterpret_cast<T*>(buffer_type::data() + __index));
        }

        template <typename T>
        inline const T* data(size_type __index) const noexcept
        {
            return __fst::launder(reinterpret_cast<const T*>(buffer_type::data() + __index));
        }

        //
        // MARK: Add elements.
        //
        template <class _InputIt, bool _IsLittleEndian = true>
        inline void insert(iterator pos, _InputIt first, _InputIt last)
        {
            static_assert(_IsLittleEndian, "byte_vector::insert is not supported for big endian.");
            buffer_type::insert(pos, first, last);
        }

        using buffer_type::push_back;

        inline void push_back(__fst::string_view str) { buffer_type::insert(buffer_type::end(), str.begin(), str.end()); }

        inline void push_back(const char* str)
        {
            for (size_t i = 0; i < __fst::strlen(str); i++)
            {
                buffer_type::push_back(static_cast<value_type>(str[i]));
            }
        }

        /*inline void push_back(const __fst::string& str)
        {
            for (size_t i = 0; i < str.size(); i++)
            {
                buffer_type::push_back(static_cast<value_type>(str[i]));
            }
        }*/

        template <class _Container, bool _IsLittleEndian = true, __fst::enable_if_t<__fst::is_container_v<_Container>, int> = 0>
        inline void push_back(const _Container& bvec)
        {
            static_assert(_IsLittleEndian, "byte_vector::push_back is not supported for big endian.");

            // TODO: change this
            for (const auto& n : bvec)
            {
                push_back(n);
            }
            //insert<_IsLittleEndian>(end(), bvec.begin(), bvec.end());
        }

        template <typename T, bool _IsLittleEndian = true, __fst::enable_if_t<!__fst::is_container_v<T>, int> = 0>
        inline void push_back(const T& value)
        {
            const value_type* other_data = reinterpret_cast<const value_type*>(&value);

            if constexpr (is_iterable<T>::value)
            {
                for (const auto& n : value)
                {
                    push_back<__fst::remove_cvref_t<decltype(n)>, _IsLittleEndian>(n);
                }
            }
            else
            {
                static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
                if constexpr (_IsLittleEndian)
                {
                    for (size_t i = 0; i < sizeof(T); i++)
                    {
                        buffer_type::push_back(other_data[i]);
                    }
                }
                else
                {
                    for (int i = sizeof(T) - 1; i >= 0; i--)
                    {
                        buffer_type::push_back(other_data[i]);
                    }
                }
            }
        }

        template <typename T, bool _IsLittleEndian = true>
        inline void push_back(const T* in_data, size_type size)
        {
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");

            if constexpr (_IsLittleEndian)
            {
                size_t last_index = buffer_type::size();
                size_t byte_size = size * sizeof(T);
                buffer_type::resize(buffer_type::size() + byte_size);
                __fst::memmove(&buffer_type::operator[](last_index), in_data, byte_size);
            }
            else
            {
                for (size_t i = 0; i < size; i++)
                {
                    push_back<T, _IsLittleEndian>(in_data[i]);
                }
            }
        }

        /*template <typename T, bool _IsLittleEndian = true>
        inline void push_back(const __fst::vector<T>& in_data)
        {
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
            push_back<T, _IsLittleEndian>(in_data.data(), in_data.size());
        }*/

        //    template <typename T, bool _IsLittleEndian = true>
        //    inline void push_back(const __fst::span<T>& data) {
        //      static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
        //      push_back<T, _IsLittleEndian>(data.data(), data.size());
        //    }

        template <typename T, convert_options c_opts>
        inline void push_back(const T& value)
        {
            static_assert(__fst::is_floating_point<T>::value, "Type must be a floating point.");

            if constexpr (c_opts == convert_options::pcm_8_bit)
            {
                T s = __fst::clamp<T>(value, (T) -1.0, (T) 1.0);
                s = (s + (T) 1.0) / (T) 2.0;
                push_back(static_cast<value_type>(s * (T) 255.0));
            }
            else if constexpr (c_opts == convert_options::pcm_16_bit)
            {
                // constexpr T mult = (1 << 15) - 1;
                constexpr T mult = (1 << 15);
                const T s = __fst::clamp<T>(value, (T) -1.0, (T) 1.0);
                push_back<int16_t>(static_cast<int16_t>(s * mult));
            }
            else if constexpr (c_opts == convert_options::pcm_24_bit)
            {
                int32_t s_int = static_cast<int32_t>(value * 8388608.0);
                push_back(static_cast<value_type>(s_int & 0xFF));
                push_back(static_cast<value_type>((s_int >> 8) & 0xFF));
                push_back(static_cast<value_type>((s_int >> 16) & 0xFF));
            }
            else if constexpr (c_opts == convert_options::pcm_32_bit)
            {
                constexpr T mult_tmp = 1L << 31L;
                constexpr T mult = mult_tmp; // - 1;
                const T s = __fst::clamp<T>(value, (T) -1.0, (T) 1.0);
                push_back<int32_t>(static_cast<int32_t>(s * mult));
            }
        }

        inline void push_padding(size_t count)
        {
            for (size_t i = 0; i < count; i++)
            {
                push_back((value_type) 0);
            }
        }

        inline int push_padding()
        {
            if (int padding = size() % 4)
            {
                push_padding(4 - padding);
                return 4 - padding;
            }

            return 0;
        }

        // Calling pop_back on an empty container is undefined.
        using buffer_type::pop_back;

        //
        // MARK: Find.
        //
        // template <class T>
        // inline difference_type find(const T* data, size_type size) const noexcept
        // {
        //     typename buffer_type::const_iterator it = __fst::search(buffer_type::cbegin(), buffer_type::cend(), data, data + size);
        //     if (it == buffer_type::cend()) { return -1; }

        //     return __fst::distance(buffer_type::cbegin(), it);
        // }

        // template <class T>
        // inline difference_type find(size_type offset, const T* data, size_type size) const noexcept
        // {
        //     typename buffer_type::const_iterator it = __fst::search(buffer_type::cbegin() + offset, buffer_type::cend(), data, data + size);
        //     if (it == buffer_type::cend()) { return -1; }

        //     return __fst::distance(buffer_type::cbegin(), it);
        // }

        //
        // MARK: Convertions.
        //
        template <typename T, bool _IsLittleEndian = true>
        inline T& as_ref_offset(size_type __index) noexcept
        {
            static_assert(_IsLittleEndian, "byte_vector::as_ref is not supported for big endian.");
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
            return *data<T>(__index);
        }

        template <typename T, bool _IsLittleEndian = true>
        inline const T& as_ref_offset(size_type __index) const noexcept
        {
            static_assert(_IsLittleEndian, "byte_vector::as_ref is not supported for big endian.");
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
            return *data<T>(__index);
        }

        template <typename T, class ..._Args>
        inline T as_type_offset() const noexcept
        {
            return as_offset<T>((sizeof(_Args) + ... + 0));
        }

        template <typename T, bool _IsLittleEndian = true>
        inline T as_offset(size_type __index) const noexcept
        {
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");

            if constexpr (_IsLittleEndian)
            {
                if constexpr (sizeof(T) <= 8)
                {
                    T value;
                    pointer data = reinterpret_cast<pointer>(&value);
                    for (size_type i = __index, j = 0; i < __index + sizeof(T); i++, j++)
                    {
                        data[j] = buffer_type::operator[](i);
                    }
                    return value;
                }
                else
                {
                    T value;
                    pointer value_data = reinterpret_cast<pointer>(&value);
                    __fst::memmove(value_data, data<T>(__index), sizeof(T));
                    return value;
                }
            }
            else
            {
                T value;
                pointer data = reinterpret_cast<pointer>(&value);
                for (size_type i = __index, j = sizeof(T) - 1; i < __index + sizeof(T); i++, j--)
                {
                    data[j] = buffer_type::operator[](i);
                }
                return value;
            }
        }

        template <typename T, bool _IsLittleEndian = true>
        inline T as(iterator pos) const noexcept
        {
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
            difference_type index = __fst::distance(pos, buffer_type::begin());
            fst_assert(index >= 0, "Wrong iterator position.");
            return as_offset<T, _IsLittleEndian>((size_type) index);
        }

        // Get array element at array_index from array starting at index.
        template <typename T, bool _IsLittleEndian = true>
        inline T as_offset(size_type index, size_type array_index) const noexcept
        {
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
            return as_offset<T, _IsLittleEndian>(index + array_index * sizeof(T));
        }

        // Get array element at array_index from array starting at pos.
        template <typename T, bool _IsLittleEndian = true>
        inline T as(iterator pos, size_type array_index) const noexcept
        {
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");
            difference_type index = __fst::distance(pos, buffer_type::begin());
            fst_assert(index >= 0, "Wrong iterator position.");
            return as_offset<T, _IsLittleEndian>((size_type) index, array_index);
        }

        template <typename T, bool _IsLittleEndian = true>
        inline void copy_as(T* buffer, size_type index, size_type array_size) const noexcept
        {
            static_assert(__fst::is_trivially_copyable<T>::value, "Type cannot be serialized.");

            if constexpr (_IsLittleEndian) { __fst::memmove(buffer, data<T>(index), array_size * sizeof(T)); }
            else
            {
                for (size_type i = 0; i < array_size; i++)
                {
                    buffer[i] = as_offset<T, _IsLittleEndian>(index, i);
                }
            }
        }

        template <typename T, convert_options c_opts>
        inline T as(size_type __index) const noexcept
        {
            static_assert(__fst::is_floating_point<T>::value, "Type must be a floating point.");

            if constexpr (c_opts == convert_options::pcm_8_bit)
            {
                constexpr T div = 1 << 7;
                return static_cast<T>(int(buffer_type::operator[](__index)) - 128) / div;
            }
            else if constexpr (c_opts == convert_options::pcm_16_bit)
            {
                constexpr T denom = T(1.0) / (T) (1 << 15);
                return (T) as_offset<int16_t>(__index) * denom;
            }
            else if constexpr (c_opts == convert_options::pcm_24_bit)
            {
                constexpr T denom = 1.0 / (T) 8388608.0;
                int32_t value = (static_cast<int32_t>(buffer_type::operator[](__index + 2)) << 16) | (static_cast<int32_t>(buffer_type::operator[](__index + 1)) << 8)
                                | static_cast<int32_t>(buffer_type::operator[](__index));

                // If the 24th bit is set, this is a negative number in 24-bit world.
                // Make sure sign is extended to the 32 bit float.
                return (value & 0x800000 ? (value | ~0xFFFFFF) : value) * denom;
            }
            else if constexpr (c_opts == convert_options::pcm_32_bit)
            {
                constexpr T div = 1L << 31L;
                return as_offset<int32_t>(__index) / div;
            }
        }
    };

    template<class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    using byte_vector_t = __fst::basic_byte_vector<__fst::vector<__fst::byte, alignof(__fst::byte), _MemoryCategory, _MemoryZone>>;


    using byte_vector = __fst::basic_byte_vector<__fst::vector<__fst::byte>>;

        template <size_t _Size, class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    using small_byte_vector = __fst::basic_byte_vector<__fst::small_vector<__fst::byte, _Size, alignof(__fst::byte), _MemoryZone, _MemoryCategory>>;

FST_END_NAMESPACE
