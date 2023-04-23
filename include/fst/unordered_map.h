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
#include "fst/small_vector.h"
#include "fst/vector.h"

FST_BEGIN_NAMESPACE

    template <class _Key, class _T, size_t _Alignment = alignof(_T), class _MemoryZone = _FST::default_memory_zone, class _MemoryCategory = _FST::default_memory_category>
    class unordered_map
    {
      public:
        static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");

        using key_type = _Key;
        using value_type = _T;

        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = _FST::cref_t<value_type>;

        using iterator = pointer;
        using const_iterator = const_pointer;

        using key_const_reference = _FST::cref_t<key_type>;

        using key_vector = _FST::vector<key_type, alignof(key_type), _MemoryZone, _MemoryCategory>;
        using value_vector = _FST::vector<value_type, _Alignment, _MemoryZone, _MemoryCategory>;

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type size() const noexcept { return _values.size(); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _values.empty(); }

        FST_NODISCARD inline constexpr iterator begin() noexcept { return _values.begin(); }
        FST_NODISCARD inline constexpr const_iterator begin() const noexcept { return _values.begin(); }

        FST_NODISCARD inline constexpr iterator end() noexcept { return _values.end(); }
        FST_NODISCARD inline constexpr const_iterator end() const noexcept { return _values.end(); }

        FST_NODISCARD inline constexpr const_iterator cbegin() const noexcept { return begin(); }
        FST_NODISCARD inline constexpr const_iterator cend() const noexcept { return end(); }

        FST_NODISCARD constexpr reference front() noexcept { return _values.front(); }
        FST_NODISCARD constexpr const_reference front() const noexcept { return _values.front(); }

        FST_NODISCARD constexpr reference back() noexcept { return _values.back(); }
        FST_NODISCARD constexpr const_reference back() const noexcept { return _values.back(); }

        FST_NODISCARD constexpr iterator find(key_const_reference key) noexcept
        {

            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return iterator(_values.data() + i); }
            }

            return end();
        }

        FST_NODISCARD constexpr const_iterator find(key_const_reference key) const noexcept
        {

            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return const_iterator(_values.data() + i); }
            }

            return end();
        }

        FST_ALWAYS_INLINE reference operator[](key_const_reference key) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return _values[i]; }
            }

            _keys.push_back(key);
            _values.push_back(value_type{});
            return _values.back();
        }

        template <class U = key_type, _FST::enable_if_t<!_FST::is_trivial_cref_v<U> && _FST::is_same_v<U, key_type>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE reference operator[](key_type&& key) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return _values[i]; }
            }

            _keys.push_back(_FST::move(key));
            _values.push_back(value_type{});
            return _values.back();
        }

        FST_ALWAYS_INLINE void clear() noexcept
        {
            _keys.clear();
            _values.clear();
        }

        bool insert(key_const_reference key, const_reference value) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return false; }
            }

            _keys.push_back(key);
            _values.push_back(value);
            return true;
        }

        template <class... _Args>
        bool emplace(key_const_reference key, _Args&&... args) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return false; }
            }

            _keys.push_back(key);
            _values.emplace_back(_FST::forward<_Args>(args)...);
            return true;
        }

        template <class K>
        FST_NODISCARD FST_ALWAYS_INLINE bool contains(const K& key) const noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return true; }
            }

            return false;
        }

        size_type erase(key_const_reference key) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key)
                {

                    _keys.erase_at(i);
                    _values.erase_at(i);
                    return 1;
                }
            }

            return 0;
        }

        inline size_type erase(const_iterator it) noexcept
        {
            size_type index = _FST::distance(_values.cbegin(), it);
            if (_values.size() <= index) { return 0; }

            _keys.erase_at(index);
            _values.erase_at(index);
            return 1;
        }

        FST_NODISCARD FST_ALWAYS_INLINE const key_vector& keys() const noexcept { return _keys; }

        FST_NODISCARD FST_ALWAYS_INLINE const value_vector& values() const noexcept { return _values; }

        key_vector _keys;
        value_vector _values;
    };

    template <class _Key, class _T, size_t _Size, size_t _Alignment = alignof(_T), class _MemoryZone = _FST::default_memory_zone,
        class _MemoryCategory = _FST::default_memory_category>
    class small_unordered_map
    {
      public:
        static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");

        using key_type = _Key;
        using value_type = _T;

        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = _FST::cref_t<value_type>;

        using iterator = pointer;
        using const_iterator = const_pointer;

        using key_const_reference = _FST::cref_t<key_type>;

        using key_vector = _FST::small_vector<key_type, _Size, alignof(key_type), _MemoryZone, _MemoryCategory>;
        using value_vector = _FST::small_vector<value_type, _Size, _Alignment, _MemoryZone, _MemoryCategory>;

        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_type size() const noexcept { return _values.size(); }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool empty() const noexcept { return _values.empty(); }

        FST_NODISCARD inline constexpr iterator begin() noexcept { return _values.begin(); }
        FST_NODISCARD inline constexpr const_iterator begin() const noexcept { return _values.begin(); }

        FST_NODISCARD inline constexpr iterator end() noexcept { return _values.end(); }
        FST_NODISCARD inline constexpr const_iterator end() const noexcept { return _values.end(); }

        FST_NODISCARD inline constexpr const_iterator cbegin() const noexcept { return begin(); }
        FST_NODISCARD inline constexpr const_iterator cend() const noexcept { return end(); }

        FST_NODISCARD constexpr reference front() noexcept { return _values.front(); }
        FST_NODISCARD constexpr const_reference front() const noexcept { return _values.front(); }

        FST_NODISCARD constexpr reference back() noexcept { return _values.back(); }
        FST_NODISCARD constexpr const_reference back() const noexcept { return _values.back(); }

        FST_NODISCARD constexpr iterator find(key_const_reference key) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return iterator(_values.data() + i); }
            }

            return end();
        }

        FST_NODISCARD constexpr const_iterator find(key_const_reference key) const noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return const_iterator(_values.data() + i); }
            }

            return end();
        }

        FST_ALWAYS_INLINE reference operator[](key_const_reference key) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return _values[i]; }
            }

            _keys.push_back(key);
            _values.push_back(value_type{});
            return _values.back();
        }

        template <class U = key_type, _FST::enable_if_t<!_FST::is_trivial_cref_v<U> && _FST::is_same_v<U, key_type>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE reference operator[](key_type&& key) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return _values[i]; }
            }

            _keys.push_back(_FST::move(key));
            _values.push_back(value_type{});
            return _values.back();
        }

        FST_ALWAYS_INLINE void clear() noexcept
        {
            _keys.clear();
            _values.clear();
        }

        bool insert(key_const_reference key, const_reference value) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return false; }
            }

            _keys.push_back(key);
            _values.push_back(value);
            return true;
        }

        template <class K>
        FST_NODISCARD FST_ALWAYS_INLINE bool contains(const K& key) const noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key) { return true; }
            }

            return false;
        }

        size_type erase(key_const_reference key) noexcept
        {
            for (size_type i = 0; i < size(); i++)
            {
                if (_keys[i] == key)
                {

                    _keys.erase_at(i);
                    _values.erase_at(i);
                    return 1;
                }
            }

            return 0;
        }

        inline size_type erase(const_iterator it) noexcept
        {
            difference_type index = _FST::distance(_values.cbegin(), it);
            if (_values.size() <= (size_type) index) { return 0; }

            _keys.erase_at((size_type) index);
            _values.erase_at((size_type) index);
            return 1;
        }

        FST_NODISCARD FST_ALWAYS_INLINE const key_vector& keys() const noexcept { return _keys; }

        FST_NODISCARD FST_ALWAYS_INLINE const value_vector& values() const noexcept { return _values; }

        key_vector _keys;
        value_vector _values;
    };

FST_END_NAMESPACE
