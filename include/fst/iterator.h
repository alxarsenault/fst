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
        using iterator_category = _FST::contiguous_iterator_tag;
    };

    template <class _T>
    struct iterator_traits<const _T*>
    {
        using value_type = _T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;
        using iterator_category = _FST::contiguous_iterator_tag;
    };
#endif

    // iterator_value_type
    template <class _T>
    using iterator_value_type = _FST::type_identity<typename _FST::iterator_traits<_T>::value_type>;
    template <class _T>
    using iterator_value_type_t = typename _FST::iterator_value_type<_T>::type;

    // iterator_pointer_type
    template <class _T>
    using iterator_pointer_type = _FST::type_identity<typename _FST::iterator_traits<_T>::pointer>;
    template <class _T>
    using iterator_pointer_type_t = typename _FST::iterator_pointer_type<_T>::type;

    // iterator_category
    template <class _T>
    using iterator_category = _FST::type_identity<typename _FST::iterator_traits<_T>::iterator_category>;

    template <class _T>
    using iterator_category_t = typename _FST::iterator_category<_T>::type;

    template <typename _Iterator>
    using is_random_access_iterator = _FST::is_base_of<_FST::random_access_iterator_tag, _FST::iterator_category_t<_Iterator>>;

    template <typename _Iterator>
    using is_contiguous_iterator = _FST::is_base_of<_FST::contiguous_iterator_tag, _FST::iterator_category_t<_Iterator>>;

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
            : container(_FST::addressof(_Cont))
            , iter(_FST::move(_Where))
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
            iter = container->insert(iter, _FST::move(_Val));
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
            //_Construct_in_place(*_Next, _FST::move(_Val));
            fst_placement_new(_Next) _Ty(_FST::move(_Val));
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
    { // wrap pushes to back of container as output iterator
      public:
        using iterator_category = output_iterator_tag;
        using value_type = void;
        using pointer = void;
        using reference = void;

        using container_type = _Container;
        using difference_type = ptrdiff_t;
        using container_value_type = typename _Container::value_type;

        constexpr explicit back_insert_iterator(_Container& _Cont) noexcept /* strengthened */
            : container(_FST::addressof(_Cont))
        {}

        constexpr back_insert_iterator& operator=(const container_value_type& _Val)
        {
            container->push_back(_Val);
            return *this;
        }

        constexpr back_insert_iterator& operator=(container_value_type&& _Val)
        {
            container->push_back(_FST::move(_Val));
            return *this;
        }

        FST_NODISCARD constexpr back_insert_iterator& operator*() noexcept { return *this; }

        constexpr back_insert_iterator& operator++() noexcept { return *this; }

        constexpr back_insert_iterator operator++(int) noexcept { return *this; }

        inline void reserve(size_t size) noexcept
        {
            //container->reserve(size);
        }

      protected:
        _Container* container;
    };

    template <class _Container>
    FST_NODISCARD constexpr back_insert_iterator<_Container> back_inserter(_Container & _Cont) noexcept
    {
        // return a back_insert_iterator
        return _FST::back_insert_iterator<_Container>(_Cont);
    }

    template <class T>
    struct output_iterator_traits : _FST::iterator_traits<T>
    {};

    template <class OutputIt, class T>
    struct output_iterator_traits<_FST::raw_storage_iterator<OutputIt, T>> : _FST::iterator_t<_FST::output_iterator_tag, T>
    {};

    template <class Container>
    struct output_iterator_traits<_FST::back_insert_iterator<Container>> : _FST::iterator_t<_FST::output_iterator_tag, typename Container::value_type>
    {};

    //template <class Container>
    //struct output_iterator_traits<_FST::front_insert_iterator<Container>>
    //: _FST::iterator<_FST::output_iterator_tag, typename Container::value_type> {};

    template <class Container>
    struct output_iterator_traits<_FST::insert_iterator<Container>> : _FST::iterator_t<_FST::output_iterator_tag, typename Container::value_type>
    {};

    //template <class T, class charT, class traits>
    //struct output_iterator_traits<_FST::ostream_iterator<T, charT, traits>> : _FST::iterator<_FST::output_iterator_tag, T> {
    //};

    //template <class charT, class traits>
    //struct output_iterator_traits<_FST::ostreambuf_iterator<charT, traits>>
    //: _FST::iterator<_FST::output_iterator_tag, charT> {};

    template <class T>
    using output_iterator_value_type_t = typename output_iterator_traits<T>::value_type;

    template <class It, class Distance>
    constexpr void advance(It & it, Distance n)
    {
        using category = typename _FST::iterator_traits<It>::iterator_category;
        static_assert(_FST::is_base_of_v<_FST::input_iterator_tag, category>);

        auto dist = typename _FST::iterator_traits<It>::difference_type(n);
        if constexpr (_FST::is_base_of_v<_FST::random_access_iterator_tag, category>)
            it += dist;
        else
        {
            while (dist > 0)
            {
                --dist;
                ++it;
            }
            if constexpr (_FST::is_base_of_v<_FST::bidirectional_iterator_tag, category>)
            {
                while (dist < 0)
                {
                    ++dist;
                    --it;
                }
            }
        }
    }

    template <class _IteratorType>
    class iterator
    {
      public:
        using iterator_type = _IteratorType;
        using iterator_category = typename _FST::iterator_traits<iterator_type>::iterator_category;
        using value_type = typename _FST::iterator_traits<iterator_type>::value_type;
        using difference_type = typename _FST::iterator_traits<iterator_type>::difference_type;
        using pointer = typename _FST::iterator_traits<iterator_type>::pointer;
        using reference = typename _FST::iterator_traits<iterator_type>::reference;

        iterator() noexcept = default;
        ~iterator() noexcept = default;

        inline explicit iterator(iterator_type x) noexcept
            : _it(x)
        {}

        template <class U>
        inline explicit iterator(const iterator<U>& other, typename _FST::enable_if<_FST::is_convertible<U, iterator_type>::value>::type* = 0) noexcept
            : _it(other.base())
        {}

        template <class U, typename _FST::enable_if<_FST::is_convertible<U, iterator_type>::value>::type* = 0>
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

    template <class _IndexIt, class _IteratorType>
    class index_iterator
    {
      public:
        using iterator_type = _IteratorType;
        using iterator_category = typename _FST::iterator_traits<iterator_type>::iterator_category;
        using value_type = typename _FST::iterator_traits<iterator_type>::value_type;
        using difference_type = typename _FST::iterator_traits<iterator_type>::difference_type;
        using pointer = typename _FST::iterator_traits<iterator_type>::pointer;
        using reference = typename _FST::iterator_traits<iterator_type>::reference;
        using index_iterator_type = _IndexIt;

        index_iterator() noexcept = default;
        ~index_iterator() noexcept = default;

        inline explicit index_iterator(index_iterator_type indexes, iterator_type it) noexcept
            : _it(indexes)
            , _data(it)
        {}

        template <class U>
        inline explicit index_iterator(
            const index_iterator<_IndexIt, U>& other, typename _FST::enable_if<_FST::is_convertible<U, iterator_type>::value>::type* = 0) noexcept
            : _it(other.index_base())
            , _data(other.base())
        {}

        template <class U, typename _FST::enable_if<_FST::is_convertible<U, iterator_type>::value>::type* = 0>
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

FST_END_NAMESPACE

template <typename T>
FST_NODISCARD inline _FST::iterator<T> begin(T* val) noexcept
{
    return _FST::iterator<T>(val);
}

template <typename T, typename Tsize>
FST_NODISCARD inline _FST::iterator<T> end(T* val, Tsize size) noexcept
{
    return _FST::iterator<T>(val) + size;
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator==(const _FST::iterator<_Iterator1>& lhs, const _FST::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator!=(const _FST::iterator<_Iterator1>& lhs, const _FST::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator>(const _FST::iterator<_Iterator1>& lhs, const _FST::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() > rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator>=(const _FST::iterator<_Iterator1>& lhs, const _FST::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() >= rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator<(const _FST::iterator<_Iterator1>& lhs, const _FST::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() < rhs.base();
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline bool operator<=(const _FST::iterator<_Iterator1>& lhs, const _FST::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() <= rhs.base();
}

template <class Iterator>
FST_NODISCARD inline _FST::iterator<Iterator> operator+(typename _FST::iterator<Iterator>::difference_type n, const _FST::iterator<Iterator>& it) noexcept
{
    return it.base() + n;
}

template <class _Iterator1, class _Iterator2>
FST_NODISCARD inline typename _FST::iterator<_Iterator1>::difference_type operator-(const _FST::iterator<_Iterator1>& lhs, const _FST::iterator<_Iterator2>& rhs) noexcept
{
    return lhs.base() - rhs.base();
}

//
//
//

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator==(const _FST::index_iterator<_Index1It, _Iterator1Type>& lhs, const _FST::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator!=(const _FST::index_iterator<_Index1It, _Iterator1Type>& lhs, const _FST::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() != rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator>(const _FST::index_iterator<_Index1It, _Iterator1Type>& lhs, const _FST::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() > rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator>=(const _FST::index_iterator<_Index1It, _Iterator1Type>& lhs, const _FST::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() >= rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator<(const _FST::index_iterator<_Index1It, _Iterator1Type>& lhs, const _FST::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() < rhs.base();
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline bool operator<=(const _FST::index_iterator<_Index1It, _Iterator1Type>& lhs, const _FST::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() <= rhs.base();
}

template <class _Index1It, class _Iterator1Type>
FST_NODISCARD inline _FST::index_iterator<_Index1It, _Iterator1Type> operator+(
    typename _FST::index_iterator<_Index1It, _Iterator1Type>::difference_type n, const _FST::index_iterator<_Index1It, _Iterator1Type>& it) noexcept
{
    return it.base() + n;
}

template <class _Index1It, class _Iterator1Type, class _IndexIt2, class _Iterator2Type>
FST_NODISCARD inline typename _FST::index_iterator<_Index1It, _Iterator1Type>::difference_type operator-(
    const _FST::index_iterator<_Index1It, _Iterator1Type>& lhs, const _FST::index_iterator<_IndexIt2, _Iterator2Type>& rhs) noexcept
{
    return lhs.base() - rhs.base();
}
