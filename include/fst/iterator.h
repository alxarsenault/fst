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

FST_BEGIN_NAMESPACE

#if FST_USE_STL

    using input_iterator_tag = std::input_iterator_tag;
    using output_iterator_tag = std::output_iterator_tag;
    using forward_iterator_tag = std::forward_iterator_tag;
    using bidirectional_iterator_tag = std::bidirectional_iterator_tag;
    using random_access_iterator_tag = std::random_access_iterator_tag;
    using contiguous_iterator_tag = std::contiguous_iterator_tag;

    template <class _T>
    using iterator_traits = std::iterator_traits<_T>;

#else
    struct input_iterator_tag
    {};
    struct output_iterator_tag
    {};
    struct forward_iterator_tag : input_iterator_tag
    {};
    struct bidirectional_iterator_tag : forward_iterator_tag
    {};
    struct random_access_iterator_tag : bidirectional_iterator_tag
    {};
    struct contiguous_iterator_tag : random_access_iterator_tag
    {};

    template <class _Ty>
    struct iterator_traits;

    template <class _T>
    struct iterator_traits<_T*>
    {
        using value_type = _T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = __fst::contiguous_iterator_tag;
    };

    template <class _T>
    struct iterator_traits<const _T*>
    {
        using value_type = _T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = __fst::contiguous_iterator_tag;
    };
#endif

    // iterator_value_type
    template <class _T>
    using iterator_value_type = __fst::type_identity<typename __fst::iterator_traits<_T>::value_type>;

    template <class _T>
    using iterator_value_type_t = typename __fst::iterator_value_type<_T>::type;

    // iterator_pointer_type
    template <class _T>
    using iterator_pointer_type = __fst::type_identity<typename __fst::iterator_traits<_T>::pointer>;

    template <class _T>
    using iterator_pointer_type_t = typename __fst::iterator_pointer_type<_T>::type;

    // iterator_category
    template <class _T>
    using iterator_category = __fst::type_identity<typename __fst::iterator_traits<_T>::iterator_category>;

    template <class _T>
    using iterator_category_t = typename __fst::iterator_category<_T>::type;

    template <typename _Iterator>
    using is_random_access_iterator = __fst::is_base_of<__fst::random_access_iterator_tag, __fst::iterator_category_t<_Iterator>>;

    template <typename _Iterator>
    using is_contiguous_iterator = __fst::is_base_of<__fst::contiguous_iterator_tag, __fst::iterator_category_t<_Iterator>>;

    template <class _Category, class _Ty, class _Diff = ptrdiff_t, class _Pointer = _Ty*, class _Reference = _Ty&>
    struct iterator_t
    { // base type for iterator classes
        using iterator_category = _Category;
        using value_type = _Ty;
        using difference_type = _Diff;
        using pointer = _Pointer;
        using reference = _Reference;
    };

    template <class _Container>
    class insert_iterator
    {
      public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using pointer = void;
        using reference = void;

        using container_type = _Container;

        using difference_type = void;
        using _Wrapped_iter = typename _Container::iterator;

        constexpr insert_iterator(_Container& _Cont, _Wrapped_iter _Where)
            : container(__fst::addressof(_Cont))
            , iter(__fst::move(_Where))
        {}

        constexpr insert_iterator& operator=(const typename _Container::value_type& _Val)
        {
            // insert into container and increment stored iterator
            iter = container->insert(iter, _Val);
            ++iter;
            return *this;
        }

        constexpr insert_iterator& operator=(typename _Container::value_type&& _Val)
        { // push value into container
            iter = container->insert(iter, __fst::move(_Val));
            ++iter;
            return *this;
        }

        FST_NODISCARD constexpr insert_iterator& operator*()
        { // pretend to return designated value
            return *this;
        }

        constexpr insert_iterator& operator++()
        { // pretend to preincrement
            return *this;
        }

        constexpr insert_iterator& operator++(int)
        { // pretend to postincrement
            return *this;
        }

      protected:
        _Container* container;
        _Wrapped_iter iter;
    };

    template <class _OutIt, class _Ty>
    class raw_storage_iterator
    { // wrap stores to raw buffer as output iterator
      public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using difference_type = void;
        using pointer = void;
        using reference = void;

        explicit raw_storage_iterator(_OutIt _First)
            : _Next(_First)
        {}

        FST_NODISCARD raw_storage_iterator& operator*()
        { // pretend to return designated value
            return *this;
        }

        raw_storage_iterator& operator=(const _Ty& _Val)
        { // construct value designated by stored iterator
            //_Construct_in_place(*_Next, _Val);
            fst_placement_new(_Next) _Ty(_Val);

            return *this;
        }

        raw_storage_iterator& operator=(_Ty&& _Val)
        { // construct value designated by stored iterator
            //_Construct_in_place(*_Next, __fst::move(_Val));
            fst_placement_new(_Next) _Ty(__fst::move(_Val));
            return *this;
        }

        raw_storage_iterator& operator++()
        {
            ++_Next;
            return *this;
        }

        raw_storage_iterator operator++(int)
        {
            raw_storage_iterator _Ans = *this;
            ++_Next;
            return _Ans;
        }

        FST_NODISCARD _OutIt base() const { return _Next; }

      private:
        _OutIt _Next;
    };

    template <class _Container>
    class back_insert_iterator
    {
      public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using pointer = void;
        using reference = void;

        using container_type = _Container;
        using difference_type = ptrdiff_t;
        using container_value_type = typename _Container::value_type;

        constexpr explicit back_insert_iterator(_Container& _Cont) noexcept /* strengthened */
            : container(__fst::addressof(_Cont))
        {}

        constexpr back_insert_iterator& operator=(const container_value_type& _Val)
        {
            container->push_back(_Val);
            return *this;
        }

        constexpr back_insert_iterator& operator=(container_value_type&& _Val)
        {
            container->push_back(__fst::move(_Val));
            return *this;
        }

        FST_NODISCARD constexpr back_insert_iterator& operator*() noexcept { return *this; }

        constexpr back_insert_iterator& operator++() noexcept { return *this; }

        constexpr back_insert_iterator operator++(int) noexcept { return *this; }

      protected:
        _Container* container;
    };

    template <class _Container>
    FST_NODISCARD constexpr back_insert_iterator<_Container> back_inserter(_Container & _Cont) noexcept
    {
        // return a back_insert_iterator
        return __fst::back_insert_iterator<_Container>(_Cont);
    }

    template <class T>
    struct output_iterator_traits : __fst::iterator_traits<T>
    {};

    template <class OutputIt, class T>
    struct output_iterator_traits<__fst::raw_storage_iterator<OutputIt, T>> : __fst::iterator_t<__fst::output_iterator_tag, T>
    {};

    template <class Container>
    struct output_iterator_traits<__fst::back_insert_iterator<Container>> : __fst::iterator_t<__fst::output_iterator_tag, typename Container::value_type>
    {};

    //template <class Container>
    //struct output_iterator_traits<__fst::front_insert_iterator<Container>>
    //: __fst::iterator<__fst::output_iterator_tag, typename Container::value_type> {};

    template <class Container>
    struct output_iterator_traits<__fst::insert_iterator<Container>> : __fst::iterator_t<__fst::output_iterator_tag, typename Container::value_type>
    {};

    //template <class T, class charT, class traits>
    //struct output_iterator_traits<__fst::ostream_iterator<T, charT, traits>> : __fst::iterator<__fst::output_iterator_tag, T> {
    //};

    //template <class charT, class traits>
    //struct output_iterator_traits<__fst::ostreambuf_iterator<charT, traits>>
    //: __fst::iterator<__fst::output_iterator_tag, charT> {};

    template <class T>
    using output_iterator_value_type_t = typename output_iterator_traits<T>::value_type;

    template <class It, class Distance>
    constexpr void advance(It & it, Distance n)
    {
        using category = typename __fst::iterator_traits<It>::iterator_category;
        static_assert(__fst::is_base_of_v<__fst::input_iterator_tag, category>);

        auto dist = typename __fst::iterator_traits<It>::difference_type(n);
        if constexpr (__fst::is_base_of_v<__fst::random_access_iterator_tag, category>)
            it += dist;
        else
        {
            while (dist > 0)
            {
                --dist;
                ++it;
            }
            if constexpr (__fst::is_base_of_v<__fst::bidirectional_iterator_tag, category>)
            {
                while (dist < 0)
                {
                    ++dist;
                    --it;
                }
            }
        }
    }

    ///
    template <class _IteratorType>
    class iterator
    {
      public:
        using iterator_type = _IteratorType;
        using iterator_category = typename __fst::iterator_traits<iterator_type>::iterator_category;
        using value_type = typename __fst::iterator_traits<iterator_type>::value_type;
        using difference_type = typename __fst::iterator_traits<iterator_type>::difference_type;
        using pointer = typename __fst::iterator_traits<iterator_type>::pointer;
        using reference = typename __fst::iterator_traits<iterator_type>::reference;

        iterator() noexcept = default;
        ~iterator() noexcept = default;

        inline explicit iterator(iterator_type x) noexcept
            : _it(x)
        {}

        template <class U>
        inline explicit iterator(const iterator<U>& other, typename __fst::enable_if<__fst::is_convertible<U, iterator_type>::value>::type* = 0) noexcept
            : _it(other.base())
        {}

        template <class U, typename __fst::enable_if<__fst::is_convertible<U, iterator_type>::value>::type* = 0>
        inline iterator& operator=(const iterator<U>& other) noexcept
        {
            _it = other.base();
            return *this;
        }

        inline iterator operator++(int) noexcept { return _it++; }
        inline iterator& operator++() noexcept
        {
            ++_it;
            return *this;
        }

        inline iterator operator--(int) noexcept { return _it--; }
        inline iterator& operator--() noexcept
        {
            --_it;
            return *this;
        }

        inline iterator& operator+=(difference_type n) noexcept
        {
            _it += n;
            return *this;
        }

        inline iterator& operator-=(difference_type n) noexcept
        {
            _it -= n;
            return *this;
        }

        FST_NODISCARD inline iterator operator+(difference_type v) const noexcept { return _it + v; }
        FST_NODISCARD inline iterator operator-(difference_type v) const noexcept { return _it - v; }

        FST_NODISCARD inline difference_type operator-(iterator it) const noexcept { return _it - it._it; }

        FST_NODISCARD inline reference operator*() const noexcept { return *_it; }
        FST_NODISCARD inline pointer operator->() const noexcept { return _it; }

        FST_NODISCARD inline operator pointer() noexcept { return _it; }

        FST_NODISCARD inline reference operator[](difference_type __n) const noexcept { return _it[__n]; }

        FST_NODISCARD inline bool operator==(const iterator& rhs) const noexcept { return _it == rhs._it; }
        FST_NODISCARD inline bool operator!=(const iterator& rhs) const noexcept { return _it != rhs._it; }
        FST_NODISCARD inline iterator_type base() const noexcept { return _it; }

      private:
        iterator_type _it = nullptr;
    };

    ///
    template <class _IndexIt, class _IteratorType>
    class index_iterator
    {
      public:
        using iterator_type = _IteratorType;
        using iterator_category = typename __fst::iterator_traits<iterator_type>::iterator_category;
        using value_type = typename __fst::iterator_traits<iterator_type>::value_type;
        using difference_type = typename __fst::iterator_traits<iterator_type>::difference_type;
        using pointer = typename __fst::iterator_traits<iterator_type>::pointer;
        using reference = typename __fst::iterator_traits<iterator_type>::reference;
        using index_iterator_type = _IndexIt;

        index_iterator() noexcept = default;
        ~index_iterator() noexcept = default;

        inline explicit index_iterator(index_iterator_type indexes, iterator_type it) noexcept
            : _it(indexes)
            , _data(it)
        {}

        template <class U>
        inline explicit index_iterator(
            const index_iterator<_IndexIt, U>& other, typename __fst::enable_if<__fst::is_convertible<U, iterator_type>::value>::type* = 0) noexcept
            : _it(other.index_base())
            , _data(other.base())
        {}

        template <class U, typename __fst::enable_if<__fst::is_convertible<U, iterator_type>::value>::type* = 0>
        inline index_iterator& operator=(const index_iterator<_IndexIt, U>& other) noexcept
        {
            _it = other.index_base();
            _data = other.base();
            return *this;
        }

        inline index_iterator operator++(int) noexcept { return _it++; }
        inline index_iterator& operator++() noexcept
        {
            ++_it;
            return *this;
        }

        inline index_iterator operator--(int) noexcept { return _it--; }
        inline index_iterator& operator--() noexcept
        {
            --_it;
            return *this;
        }

        inline index_iterator& operator+=(difference_type n) noexcept
        {
            _it += n;
            return *this;
        }

        inline index_iterator& operator-=(difference_type n) noexcept
        {
            _it -= n;
            return *this;
        }

        FST_NODISCARD inline index_iterator operator+(difference_type v) const noexcept { return index_iterator(_it + v, _data); }
        FST_NODISCARD inline index_iterator operator-(difference_type v) const noexcept { return index_iterator(_it - v, _data); }
        FST_NODISCARD inline difference_type operator-(index_iterator it) const noexcept { return _it - it._it; }

        FST_NODISCARD inline reference operator*() const noexcept { return *(_data + *_it); }
        FST_NODISCARD inline pointer operator->() const noexcept { return (_data + *_it); }
        FST_NODISCARD inline explicit operator pointer() noexcept { return _data + *_it; }

        FST_NODISCARD inline reference operator[](difference_type __n) const noexcept { return *(_data + _it[__n]); }

        FST_NODISCARD inline bool operator==(const index_iterator& rhs) const noexcept { return _it == rhs._it; }
        FST_NODISCARD inline bool operator!=(const index_iterator& rhs) const noexcept { return _it != rhs._it; }

        FST_NODISCARD inline index_iterator_type index_base() const noexcept { return _it; }
        FST_NODISCARD inline iterator_type base() const noexcept { return _data; }

      private:
        index_iterator_type _it = nullptr;
        iterator_type _data = nullptr;
    };

    ///
    template <typename _Iterator>
    class iterator_range
    {
        _Iterator _begin_iterator;
        _Iterator _end_iterator;

      public:
        template <typename Container>
        inline iterator_range(Container&& c) noexcept
            : _begin_iterator(c.begin())
            , _end_iterator(c.end())
        {}

        inline iterator_range(_Iterator begin_iterator, _Iterator end_iterator) noexcept
            : _begin_iterator(__fst::move(begin_iterator))
            , _end_iterator(__fst::move(end_iterator))
        {}

        inline _Iterator begin() const noexcept { return _begin_iterator; }
        inline _Iterator end() const noexcept { return _end_iterator; }
        inline bool empty() const noexcept { return _begin_iterator == _end_iterator; }
    };

FST_END_NAMESPACE

template <typename T>
FST_NODISCARD inline __fst::iterator<T> begin(T* val) noexcept
{
    return __fst::iterator<T>(val);
}

template <typename T, typename Tsize>
FST_NODISCARD inline __fst::iterator<T> end(T* val, Tsize size) noexcept
{
    return __fst::iterator<T>(val) + size;
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator==(const __fst::iterator<_Iterator1>& lhs, const __fst::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator!=(const __fst::iterator<_Iterator1>& lhs, const __fst::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator>(const __fst::iterator<_Iterator1>& lhs, const __fst::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() > rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator>=(const __fst::iterator<_Iterator1>& lhs, const __fst::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() >= rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator<(const __fst::iterator<_Iterator1>& lhs, const __fst::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() < rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator<=(const __fst::iterator<_Iterator1>& lhs, const __fst::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() <= rhs.base();
}

template <class Iterator>
FST_NODISCARD inline __fst::iterator<Iterator> operator+(typename __fst::iterator<Iterator>::difference_type n, const __fst::iterator<Iterator>& it) noexcept
{
    return it.base() + n;
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline typename __fst::iterator<_Iterator1>::difference_type operator-(
    const __fst::iterator<_Iterator1>& lhs, const __fst::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() - rhs.base();
}

//
//
//

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator==(const __fst::index_iterator<_Index1It, _Iterator1Type>& lhs, const __fst::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator!=(const __fst::index_iterator<_Index1It, _Iterator1Type>& lhs, const __fst::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator>(const __fst::index_iterator<_Index1It, _Iterator1Type>& lhs, const __fst::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() > rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator>=(const __fst::index_iterator<_Index1It, _Iterator1Type>& lhs, const __fst::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() >= rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator<(const __fst::index_iterator<_Index1It, _Iterator1Type>& lhs, const __fst::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() < rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator<=(const __fst::index_iterator<_Index1It, _Iterator1Type>& lhs, const __fst::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() <= rhs.base();
}

template <class _Index1It, class _Iterator1Type>
FST_NODISCARD inline __fst::index_iterator<_Index1It, _Iterator1Type> operator+(
    typename __fst::index_iterator<_Index1It, _Iterator1Type>::difference_type n, const __fst::index_iterator<_Index1It, _Iterator1Type>& it) noexcept
{
    return it.base() + n;
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline typename __fst::index_iterator<_Index1It, _Iterator1Type>::difference_type operator-(
    const __fst::index_iterator<_Index1It, _Iterator1Type>& lhs, const __fst::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() - rhs.base();
}
