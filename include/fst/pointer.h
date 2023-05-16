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
#include "fst/traits.h"
#include "fst/iterator.h"
#include "fst/tuple.h"

FST_BEGIN_NAMESPACE


    struct make_tag
    {};

    FST_INLINE_VAR constexpr make_tag make_t = {};

    template <class _Iterator, __fst::enable_if_t<__fst::is_random_access_iterator<_Iterator>::value, int> = 0>
    constexpr ptrdiff_t distance(_Iterator first, _Iterator last)
    {
        return last - first;
    }

    template <class _Iterator, __fst::enable_if_t<__fst::is_random_access_iterator<_Iterator>::value, int> = 0>
    constexpr size_t pdistance(_Iterator first, _Iterator last)
    {
        ptrdiff_t diff = __fst::distance(first, last);
        fst_assert(diff >= 0);
        return static_cast<size_t>(diff);
    }

///
/// not_null
///
/// Restricts a pointer or smart pointer to only hold non-null values.
///
/// Has zero size overhead over T.
///
/// If T is a pointer (i.e. T == U*) then
/// - allow construction from U*
/// - disallow construction from nullptr_t
/// - disallow default construction
/// - ensure construction from null U* fails
/// - allow implicit conversion to U*
///
#if __FST_DEBUG_BUILD__
    template <class T>
    class not_null
    {
      public:
        static_assert(__fst::is_convertible_v<decltype(__fst::declval<T>() != nullptr), bool>, "T cannot be compared to nullptr.");

        template <typename U, typename = __fst::enable_if_t<__fst::is_convertible_v<U, T>>>
        constexpr not_null(U&& u)
            : _ptr(__fst::forward<U>(u))
        {
            fst_assert(_ptr != nullptr, "Pointer is null.");
        }

        template <typename = __fst::enable_if_t<__fst::is_different_v<__fst::nullptr_t, T>>>
        constexpr not_null(T u)
            : _ptr(__fst::move(u))
        {
            fst_assert(_ptr != nullptr, "Pointer is null.");
        }

        template <typename U, typename = __fst::enable_if_t<__fst::is_convertible_v<U, T>>>
        constexpr not_null(const not_null<U>& other)
            : not_null(other.get())
        {}

        not_null(const not_null& other) = default;
        not_null& operator=(const not_null& other) = default;
        constexpr __fst::conditional_t<__fst::is_copy_constructible_v<T>, T, const T&> get() const
        {
            fst_assert(_ptr != nullptr, "Pointer is null.");
            return _ptr;
        }

        constexpr operator T() const { return get(); }
        constexpr decltype(auto) operator->() const { return get(); }
        constexpr decltype(auto) operator*() const { return *get(); }

        // Prevents compilation when someone attempts to assign a null pointer constant.
        not_null(__fst::nullptr_t) = delete;
        not_null& operator=(__fst::nullptr_t) = delete;

        // Unwanted operators, pointers only point to single objects.
        not_null& operator++() = delete;
        not_null& operator--() = delete;
        not_null operator++(int) = delete;
        not_null operator--(int) = delete;
        not_null& operator+=(ptrdiff_t) = delete;
        not_null& operator-=(ptrdiff_t) = delete;
        void operator[](ptrdiff_t) const = delete;

      private:
        T _ptr;
    };

    template <class T, class U>
    auto operator==(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> decltype(lhs.get() == rhs.get())
    {
        return lhs.get() == rhs.get();
    }

    template <class T, class U>
    auto operator!=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> decltype(lhs.get() != rhs.get())
    {
        return lhs.get() != rhs.get();
    }

    template <class T, class U>
    auto operator<(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> decltype(lhs.get() < rhs.get())
    {
        return lhs.get() < rhs.get();
    }

    template <class T, class U>
    auto operator<=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> decltype(lhs.get() <= rhs.get())
    {
        return lhs.get() <= rhs.get();
    }

    template <class T, class U>
    auto operator>(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> decltype(lhs.get() > rhs.get())
    {
        return lhs.get() > rhs.get();
    }

    template <class T, class U>
    auto operator>=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> decltype(lhs.get() >= rhs.get())
    {
        return lhs.get() >= rhs.get();
    }

    // More unwanted operators.
    template <class T, class U>
    ptrdiff_t operator-(const not_null<T>&, const not_null<U>&) = delete;

    template <class T>
    not_null<T> operator-(const not_null<T>&, ptrdiff_t) = delete;

    template <class T>
    not_null<T> operator+(const not_null<T>&, ptrdiff_t) = delete;

    template <class T>
    not_null<T> operator+(ptrdiff_t, const not_null<T>&) = delete;

#else
    template <class T>
    using not_null = T;

#endif

    template <class T>
    auto make_not_null(T && t) noexcept
    {
        return not_null<__fst::remove_cv_t<__fst::remove_reference_t<T>>>{ __fst::forward<T>(t) };
    }
    //template <class T>
    //__fst::ostream& operator<<(__fst::ostream& os, const not_null<T>& val) {
    //  os << val.get();
    //  return os;
    //}

    /// packed_pointer
    class packed_pointer
    {
      public:
        packed_pointer() noexcept = default;
        packed_pointer(const packed_pointer&) noexcept = default;
        packed_pointer(packed_pointer&&) noexcept = default;

        template <typename TPointer, typename TInt>
        FST_ALWAYS_INLINE packed_pointer(TPointer ptr, TInt value) noexcept
        {
            set_pointer(ptr);
            set_int(value);
        }

        packed_pointer& operator=(const packed_pointer&) noexcept = default;
        packed_pointer& operator=(packed_pointer&&) noexcept = default;

        template <typename T>
        FST_NODISCARD FST_ALWAYS_INLINE T get_int() const noexcept
        {
            return static_cast<T>(_data & k_int_mask);
        }

        template <typename T>
        FST_NODISCARD FST_ALWAYS_INLINE T get_pointer() const noexcept
        {
            return static_cast<T>(reinterpret_cast<void*>(_data & k_pointer_mask));
        }

        template <typename T>
        FST_ALWAYS_INLINE void set_pointer(T ptr) noexcept
        {
            intptr_t ptrWord = reinterpret_cast<intptr_t>((void*) (ptr));
            fst_assert((ptrWord & ~k_pointer_mask) == 0, "Pointer is not sufficiently aligned");

            // Preserve all low bits, just update the pointer.
            _data = (intptr_t) (ptrWord | (_data & ~k_pointer_mask));
        }

        template <typename T>
        FST_ALWAYS_INLINE T exchange_pointer(T ptr) noexcept
        {
            T last = static_cast<T>(reinterpret_cast<void*>(_data & k_pointer_mask));

            intptr_t ptrWord = reinterpret_cast<intptr_t>((void*) (ptr));
            fst_assert((ptrWord & ~k_pointer_mask) == 0, "Pointer is not sufficiently aligned");

            // Preserve all low bits, just update the pointer.
            _data = (intptr_t) (ptrWord | (_data & ~k_pointer_mask));
            return last;
        }

        template <typename T>
        FST_ALWAYS_INLINE void set_int(T value) noexcept
        {
            intptr_t intWord = static_cast<intptr_t>(value);
            fst_assert((intWord & ~k_int_mask) == 0, "Integer too large for field");

            // Preserve all bits other than the ones we are updating.
            _data = (intptr_t) ((_data & ~k_int_mask) | intWord);
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool operator==(packed_pointer other) const noexcept { return _data == other._data; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool operator!=(packed_pointer other) const noexcept { return _data != other._data; }

      private:
        intptr_t _data = 0;

        static constexpr uintptr_t k_pointer_mask = (uintptr_t) -4;
        static constexpr uintptr_t k_int_mask = (uintptr_t) 3;
    };

    ///
    template <class _T, class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    class unique_ptr
    {
      public:
        using element_type = _T;
        using pointer = _T*;
        using memory_zone_type = _MemoryZone;
        using memory_category_type = _MemoryCategory;

        template <class _Zone>
        using enable_if_zone_default_constructible_t = __fst::enable_if_t<!__fst::is_pointer_v<_Zone> && __fst::is_default_constructible_v<_Zone>, int>;

        template <class _Zone>
        using enable_if_zone_copy_constructible_t = __fst::enable_if_t<__fst::is_copy_constructible_v<_Zone>, int>;

        template <class _Zone>
        using enable_if_zone_move_constructible_t = __fst::enable_if_t<!__fst::is_reference_v<_Zone> && __fst::is_move_constructible_v<_Zone>, int>;

        template <class... _Args>
        inline constexpr unique_ptr(make_tag, _Args&&... args)
            : _data{ nullptr, memory_zone_type{} }
        {
            _T* ptr = (_T*) memory_zone_type::aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
            fst_assert(ptr, "allocation failed");
            _data.first() = fst_placement_new(ptr) _T(__fst::forward<_Args>(args)...);
        }

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr() noexcept
            : _data{ nullptr, memory_zone_type{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr(__fst::nullptr_t) noexcept
            : _data{ nullptr, memory_zone_type{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline explicit unique_ptr(pointer ptr) noexcept
            : _data{ ptr, memory_zone_type{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_copy_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr(pointer ptr, const memory_zone_type& mem_zone) noexcept
            : _data{ ptr, mem_zone }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_move_constructible_t<_Zone> = 0>
        inline constexpr unique_ptr(pointer ptr, memory_zone_type&& mem_zone) noexcept
            : _data{ ptr, __fst::move(mem_zone) }
        {}

        template <class _Dx2 = memory_zone_type,
            __fst::enable_if_t<__fst::conjunction_v<__fst::is_reference<_Dx2>, __fst::is_constructible<_Dx2, __fst::remove_reference_t<_Dx2>>>, int> = 0>
        unique_ptr(pointer, __fst::remove_reference_t<memory_zone_type>&&) = delete;

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_move_constructible_v<_Zone>, int> = 0>
        unique_ptr(unique_ptr&& _Right) noexcept
            : _data{ _Right.get(), __fst::forward<memory_zone_type>(_Right.get_memory_zone()) }
        {
            (void) _Right.release();
        }

        template <class _Ty2, class _OtherMemoryCategory, class _OtherMemoryZone,
            __fst::enable_if_t<__fst::conjunction_v<__fst::negation<__fst::is_c_array<_Ty2>>,
                                   __fst::is_convertible<typename unique_ptr<_Ty2, _OtherMemoryCategory, _OtherMemoryZone>::pointer, pointer>,
                                   __fst::conditional_t<__fst::is_reference_v<memory_zone_type>, __fst::is_same<_OtherMemoryZone, memory_zone_type>,
                                       __fst::is_convertible<_OtherMemoryZone, memory_zone_type>>>,
                int>
            = 0>
        unique_ptr(unique_ptr<_Ty2, _OtherMemoryCategory, _OtherMemoryZone>&& _Right) noexcept
            : _data{ _Right.get(), __fst::forward<_OtherMemoryZone>(_Right.get_memory_zone()) }
        {

            _Right.release();
        }

        unique_ptr(const unique_ptr&) = delete;

        template <class... _Args>
        static inline unique_ptr make(_Args&&... args) noexcept
        {
            _T* ptr = (_T*) memory_zone_type::aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
            fst_assert(ptr, "allocation failed");

            ptr = fst_placement_new(ptr) _T(__fst::forward<_Args>(args)...);
            return unique_ptr(ptr);
        }

        inline ~unique_ptr() noexcept
        {
            if (_data.first())
            {
                _data.first()->~element_type();
                _data.second().aligned_deallocate(_data.first(), _MemoryCategory::id());
            }
        }

        unique_ptr& operator=(const unique_ptr&) = delete;

        inline constexpr unique_ptr& operator=(__fst::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        /*template <class _Ty2, class _Dx2, class _C2,
            __fst::enable_if_t<__fst::conjunction_v<__fst::negation<__fst::is_c_array<_Ty2>>, __fst::is_assignable<memory_zone_type&, _Dx2>,
                                  __fst::is_convertible<typename unique_ptr<_Ty2, _Dx2, _C2>::pointer, pointer>>,
                int> = 0>
        inline unique_ptr& operator=(unique_ptr<_Ty2, _Dx2, _C2>&& _Right) noexcept
        {
            reset(_Right.release());
            _data.second() = __fst::forward<_Dx2>(_Right._data.second());
            return *this;
        }*/

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_move_assignable_v<_Zone>, int> = 0>
        unique_ptr& operator=(unique_ptr&& _Right) noexcept
        {
            if (this != __fst::addressof(_Right))
            {
                reset(_Right.get());
                _data.second() = __fst::forward<memory_zone_type>(_Right._data.second());
                (void) _Right.release();
            }
            return *this;
        }

        FST_NODISCARD memory_zone_type& get_memory_zone() noexcept { return _data.second(); }
        FST_NODISCARD const memory_zone_type& get_memory_zone() const noexcept { return _data.second(); }

        FST_NODISCARD __fst::add_lvalue_reference_t<_T> operator*() const noexcept { return *_data.first(); }

        FST_NODISCARD pointer operator->() const noexcept { return _data.first(); }

        FST_NODISCARD pointer get() const noexcept { return _data.first(); }

        FST_NODISCARD inline explicit operator bool() const noexcept { return static_cast<bool>(_data.first()); }

        FST_NODISCARD inline pointer release() noexcept { return __fst::exchange(_data.first(), nullptr); }

        inline void reset(pointer ptr = nullptr) noexcept
        {
            if (pointer old_ptr = __fst::exchange(_data.first(), ptr)) { _data.second().aligned_deallocate(old_ptr, _MemoryCategory::id()); }
        }

      private:
        __fst::pair<pointer, _MemoryZone> _data;
    };

    ///
    template <class _T, class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    class optional_ptr
    {
      public:
        using element_type = _T;
        using pointer = _T*;
        using memory_zone_type = _MemoryZone;
        using memory_category_type = _MemoryCategory;

        using storage_type = __fst::pair<__fst::packed_pointer, _MemoryZone>;
        using zone_reference = typename storage_type::second_reference;
        using zone_const_reference = typename storage_type::second_const_reference;

        template <class _Zone>
        using enable_if_zone_default_constructible_t = __fst::enable_if_t<!__fst::is_pointer_v<_Zone> && __fst::is_default_constructible_v<_Zone>, int>;

        template <class _Zone>
        using enable_if_zone_copy_constructible_t = __fst::enable_if_t<__fst::is_copy_constructible_v<_Zone>, int>;

        template <class _Zone>
        using enable_if_zone_move_constructible_t = __fst::enable_if_t<!__fst::is_reference_v<_Zone> && __fst::is_move_constructible_v<_Zone>, int>;

        template <class... _Args>
        inline constexpr optional_ptr(make_tag, _Args&&... args)
            : _data{ { nullptr, true }, __fst::default_construct_t{} }
        {
            pointer ptr = (pointer) memory_zone_type::aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
            fst_assert(ptr, "allocation failed");
            _data.first().template set_pointer<pointer>(fst_placement_new(ptr) _T(__fst::forward<_Args>(args)...));
        }

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline constexpr optional_ptr() noexcept
            : _data{ { nullptr, false }, __fst::default_construct_t{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline constexpr optional_ptr(__fst::nullptr_t) noexcept
            : _data{ { nullptr, false }, __fst::default_construct_t{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_default_constructible_t<_Zone> = 0>
        inline explicit optional_ptr(pointer ptr, bool powned) noexcept
            : _data{ { ptr, powned }, __fst::default_construct_t{} }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_copy_constructible_t<_Zone> = 0>
        inline constexpr optional_ptr(pointer ptr, const memory_zone_type& mem_zone, bool powned) noexcept
            : _data{ { ptr, powned }, mem_zone }
        {}

        template <class _Zone = memory_zone_type, enable_if_zone_move_constructible_t<_Zone> = 0>
        inline constexpr optional_ptr(pointer ptr, memory_zone_type&& mem_zone, bool powned) noexcept
            : _data{ { ptr, powned }, __fst::move(mem_zone) }
        {}

        template <class _OtherMemoryCategory = memory_category_type, class _OtherMemoryZone = memory_zone_type,
            __fst::enable_if_t<
                __fst::conjunction_v<__fst::is_reference<_OtherMemoryZone>, __fst::is_constructible<_OtherMemoryZone, __fst::remove_reference_t<_OtherMemoryZone>>>, int>
            = 0>
        optional_ptr(pointer, __fst::remove_reference_t<memory_zone_type>&&) = delete;

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_move_constructible_v<_Zone>, int> = 0>
        optional_ptr(optional_ptr&& _Right) noexcept
            : _data{ { _Right.get(), _Right.owned() }, __fst::forward<memory_zone_type>(_Right.get_memory_zone()) }
        {
            (void) _Right.release();
        }

        template <class _Ty2, class _OtherMemoryCategory, class _OtherMemoryZone,
            __fst::enable_if_t<__fst::conjunction_v<__fst::negation<__fst::is_c_array<_Ty2>>,
                                   __fst::is_convertible<typename optional_ptr<_Ty2, _OtherMemoryCategory, _OtherMemoryZone>::pointer, pointer>,
                                   __fst::conditional_t<__fst::is_reference_v<memory_zone_type>, __fst::is_same<_OtherMemoryZone, memory_zone_type>,
                                       __fst::is_convertible<_OtherMemoryZone, memory_zone_type>>>,
                int>
            = 0>
        optional_ptr(optional_ptr<_Ty2, _OtherMemoryCategory, _OtherMemoryZone>&& _Right) noexcept
            : _data{ { _Right.get(), _Right.owned() }, __fst::forward<_OtherMemoryZone>(_Right.get_memory_zone()) }
        {
            (void) _Right.release();
        }

        optional_ptr(const optional_ptr&) = delete;

        template <class... _Args>
        static inline optional_ptr make(_Args&&... args) noexcept
        {
            return optional_ptr(__fst::make_tag{}, __fst::forward<_Args>(args)...);
        }

        inline ~optional_ptr() noexcept
        {
            if (pointer ptr = _data.first().template get_pointer<pointer>(); ptr && owned())
            {
                ptr->~element_type();
                _data.second().aligned_deallocate(ptr, _MemoryCategory::id());
            }
        }

        optional_ptr& operator=(const optional_ptr&) = delete;

        inline constexpr optional_ptr& operator=(__fst::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        /*template <class _Ty2, class _Dx2, class _C2,
            __fst::enable_if_t<__fst::conjunction_v<__fst::negation<__fst::is_c_array<_Ty2>>, __fst::is_assignable<memory_zone_type&, _Dx2>,
                                  __fst::is_convertible<typename optional_ptr<_Ty2, _Dx2, _C2>::pointer, pointer>>,
                int> = 0>
        inline optional_ptr& operator=(optional_ptr<_Ty2, _Dx2, _C2>&& _Right) noexcept
        {
            reset(_Right.release());
            _data.second() = __fst::forward<_Dx2>(_Right._data.second());
            return *this;
        }*/

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_move_assignable_v<_Zone>, int> = 0>
        optional_ptr& operator=(optional_ptr&& _Right) noexcept
        {
            if (this != __fst::addressof(_Right))
            {
                reset(_Right.get(), _Right.owned());
                _data.second() = __fst::forward<memory_zone_type>(_Right._data.second());
                (void) _Right.release();
            }
            return *this;
        }

        /// Returns true if the pointer is owned.
        FST_NODISCARD inline bool owned() const noexcept { return _data.first().template get_int<bool>(); }

        FST_NODISCARD FST_ALWAYS_INLINE zone_const_reference get_memory_zone() const noexcept { return _data.second(); }
        FST_NODISCARD FST_ALWAYS_INLINE zone_reference get_memory_zone() noexcept { return _data.second(); }

        FST_NODISCARD __fst::add_lvalue_reference_t<_T> operator*() const noexcept { return *_data.first().template get_pointer<pointer>(); }

        FST_NODISCARD pointer operator->() const noexcept { return _data.first().template get_pointer<pointer>(); }

        FST_NODISCARD pointer get() const noexcept { return _data.first().template get_pointer<pointer>(); }

        FST_NODISCARD inline explicit operator bool() const noexcept { return static_cast<bool>(_data.first().template get_pointer<pointer>()); }

        FST_NODISCARD inline pointer release() noexcept
        {
            _data.first().set_int(false);
            return _data.first().template exchange_pointer<pointer>(nullptr);
        }

        inline void reset() noexcept
        {
            if (pointer old_ptr = _data.first().template exchange_pointer<pointer>(nullptr); old_ptr && owned())
            {
                _data.second().aligned_deallocate(old_ptr, _MemoryCategory::id());
            }

            _data.first().set_int(false);
        }

        inline void reset(pointer ptr, bool powned) noexcept
        {
            if (pointer old_ptr = _data.first().template exchange_pointer<pointer>(ptr); old_ptr && owned())
            {
                _data.second().aligned_deallocate(old_ptr, _MemoryCategory::id());
            }

            _data.first().set_int(powned && ptr != nullptr);
        }

      private:
        storage_type _data;
    };

    template <typename T>
    inline bool operator==(const optional_ptr<T>& x, const optional_ptr<T>& y) noexcept
    {
        return x.get() == y.get();
    }

    template <typename T>
    inline bool operator!=(const optional_ptr<T>& x, const optional_ptr<T>& y) noexcept
    {
        return x.get() != y.get();
    }

    template <typename T>
    inline bool operator==(const optional_ptr<T>& x, __fst::nullptr_t) noexcept
    {
        return x.get() == nullptr;
    }

    template <typename T>
    inline bool operator==(__fst::nullptr_t, const optional_ptr<T>& x) noexcept
    {
        return x.get() == nullptr;
    }

    template <typename T>
    inline bool operator!=(const optional_ptr<T>& x, __fst::nullptr_t) noexcept
    {
        return x.get() != nullptr;
    }

    template <typename T>
    inline bool operator!=(__fst::nullptr_t, const optional_ptr<T>& x) noexcept
    {
        return x.get() != nullptr;
    }

    template <typename T>
    inline bool operator==(const optional_ptr<T>& x, const T* y) noexcept
    {
        return x.get() == y;
    }

    template <typename T>
    inline bool operator==(const T* x, const optional_ptr<T>& y) noexcept
    {
        return y.get() == x;
    }

    template <typename T>
    inline bool operator!=(const optional_ptr<T>& x, const T* y) noexcept
    {
        return x.get() != y;
    }

    template <typename T>
    inline bool operator!=(const T* x, const optional_ptr<T>& y) noexcept
    {
        return y.get() != x;
    }

    //template <class T, bool Owned, class... Args>
    //inline optional_ptr<T> make_optional_ptr(Args && ... args)
    //{
    //    return optional_ptr<T>(new T(__fst::forward<Args>(args)...), Owned);
    //}

    //template <class T, class... Args>
    //inline optional_ptr<T> make_owned_optional_ptr(Args && ... args)
    //{
    //    return optional_ptr<T>(new T(__fst::forward<Args>(args)...), true);
    //}

    //template <template <class> class T, class... Args>
    //inline optional_ptr<T<Args...>> make_owned_optional_ptr(Args && ... args)
    //{
    //    return optional_ptr<T<Args...>>(new T<Args...>(__fst::forward<Args>(args)...), true);
    //}

    ///
    template <class _T, class _MemoryZone = __fst::default_memory_zone, class _MemoryCategory = __fst::default_memory_category, class... _Args,
        __fst::enable_if_t<__fst::conjunction_v<__fst::is_static_memory_zone<_MemoryZone>, __fst::is_memory_category<_MemoryCategory>>, int> = 0>
    inline _T* make(_Args && ... args) noexcept
    {
        _T* ptr = (_T*) _MemoryZone::aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
        fst_assert(ptr, "allocation failed");
        return fst_placement_new(ptr) _T(__fst::forward<_Args>(args)...);
    }

    ///
    template <class _T, class _MemoryZone, class _MemoryCategory = __fst::default_memory_category, class... _Args,
        __fst::enable_if_t<__fst::conjunction_v<__fst::is_memory_zone<_MemoryZone>, __fst::is_memory_category<_MemoryCategory>>, int> = 0>
    inline _T* make(_MemoryZone & mem_zone, _Args && ... args) noexcept
    {
        _T* ptr = (_T*) mem_zone.aligned_allocate(sizeof(_T), alignof(_T), _MemoryCategory::id());
        fst_assert(ptr, "allocation failed");
        return fst_placement_new(ptr) _T(__fst::forward<_Args>(args)...);
    }

    ///
    template <class _T, class _MemoryZone, class... _Args, __fst::enable_if_t<__fst::is_memory_zone<_MemoryZone>::value, int> = 0>
    inline _T* make(_MemoryZone & mem_zone, __fst::memory_category_id mid, _Args && ... args) noexcept
    {
        _T* ptr = (_T*) mem_zone.aligned_allocate(sizeof(_T), alignof(_T), mid);
        fst_assert(ptr, "allocation failed");
        return fst_placement_new(ptr) _T(__fst::forward<_Args>(args)...);
    }

    ///
    template <class _MemoryZone = __fst::default_memory_zone, class _MemoryCategory = __fst::default_memory_category, class _T,
        __fst::enable_if_t<__fst::conjunction_v<__fst::is_static_memory_zone<_MemoryZone>, __fst::is_memory_category<_MemoryCategory>>, int> = 0>
    inline void destroy(_T * ptr) noexcept
    {
        fst_assert(ptr, "tried to destroy nullptr");
        ptr->~_T();
        _MemoryZone::aligned_deallocate(ptr, _MemoryCategory::id());
    }

    ///
    template <class _MemoryCategory = __fst::default_memory_category, class _MemoryZone, class _T,
        __fst::enable_if_t<__fst::conjunction_v<__fst::is_memory_zone<_MemoryZone>, __fst::is_memory_category<_MemoryCategory>>, int> = 0>
    inline void destroy(_T * ptr, _MemoryZone & mem_zone) noexcept
    {
        fst_assert(ptr, "tried to destroy nullptr");
        ptr->~_T();
        mem_zone.aligned_deallocate(ptr, _MemoryCategory::id());
    }

    ///
    template <class _MemoryZone, class _T, __fst::enable_if_t<__fst::is_memory_zone<_MemoryZone>::value, int> = 0>
    inline void destroy(_T * ptr, _MemoryZone & mem_zone, __fst::memory_category_id mid) noexcept
    {
        fst_assert(ptr, "tried to destroy nullptr");
        ptr->~_T();
        mem_zone.aligned_deallocate(ptr, mid);
    }

FST_END_NAMESPACE
