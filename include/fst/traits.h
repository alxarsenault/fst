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
//#include "fst/array.h"

#if FST_USE_STL
#include <iterator>
#endif

// clang-format off
FST_BEGIN_NAMESPACE

// array
//namespace detail { template <class _T, size_t _Size, size_t _Alignment> struct basic_array; } // namespace detail
//template <typename _T, size_t _Size, size_t _Alignment = alignof(_T)> using array = _FST::detail::basic_array<_T, _Size, _Alignment>;

/// is_array_of_type_convertible.
// template <typename A, typename B>
// using is_array_of_type_convertible = _FST::is_convertible<array_of_type<A>,
// array_of_type<B>>;

// template <typename A, typename B>
// inline constexpr bool is_array_of_type_convertible_v =
// is_array_of_type_convertible<A, B>::value;

// declval
template <class _T> _FST::add_rvalue_reference_t<_T> declval() noexcept;

//
template <class _T> FST_NODISCARD constexpr _T* addressof(_T& _Val) noexcept { return FST_ADDRESSOF(_Val); }
template <class _T> const _T* addressof(const _T&&) = delete;

//
template <class _T> FST_NODISCARD constexpr _T&& forward(_FST::remove_reference_t<_T>& _Arg) noexcept { return static_cast<_T&&>(_Arg); }

//
template <class _T>
FST_NODISCARD constexpr _T&& forward(_FST::remove_reference_t<_T>&& _Arg) noexcept {
    static_assert(!_FST::is_lvalue_reference_v<_T>, "bad forward call");
    return static_cast<_T&&>(_Arg);
}

//
template <class _T>
FST_NODISCARD constexpr _FST::remove_reference_t<_T>&& move(_T&& _Arg) noexcept {
    return static_cast<_FST::remove_reference_t<_T>&&>(_Arg);
}

template <class _T, _FST::enable_if_t<_FST::is_move_constructible_v<_T> && _FST::is_move_assignable_v<_T>, int> = 0>
FST_ALWAYS_INLINE constexpr void memswap(_T& _Left, _T& _Right) noexcept {
    _T _Tmp = _FST::move(_Left);
    _Left = _FST::move(_Right);
    _Right = _FST::move(_Tmp);
}

template <class _T1, class _T2 = _T1>
FST_ALWAYS_INLINE constexpr _T1 exchange(_T1& obj, _T2&& new_value) noexcept {
    _T1 old_value = _FST::move(obj);
    obj = _FST::forward<_T2>(new_value);
    return old_value;
}

template <class _T> FST_ALWAYS_INLINE constexpr _FST::add_const_t<_T>& as_const(_T& t) noexcept { return t; }

///////////////////////////////

template <class _Container> FST_ALWAYS_INLINE constexpr auto container_size(const _Container& _Cont) -> decltype(_Cont.size()) { return _Cont.size(); }
template <class _T, size_t _Size> FST_ALWAYS_INLINE constexpr size_t container_size(const _T (&)[_Size]) noexcept { return _Size; }
template <class _Container> FST_ALWAYS_INLINE constexpr auto container_data(_Container& _Cont) -> decltype(_Cont.data()) { return _Cont.data(); }
template <class _Container> FST_ALWAYS_INLINE constexpr auto container_data(const _Container& _Cont) -> decltype(_Cont.data()) { return _Cont.data(); }
template <class _T, size_t _Size> FST_ALWAYS_INLINE constexpr _T* container_data(_T (&_Array)[_Size]) noexcept { return _Array; }

// template <class _T>
// FST_NODISCARD constexpr const _T* data(_FST::initializer_list<_T> list)
// noexcept { return list.begin(); }

template <class _Container> FST_ALWAYS_INLINE constexpr auto begin(_Container& _Cont) -> decltype(_Cont.begin()) { return _Cont.begin(); }
template <class _Container> FST_ALWAYS_INLINE constexpr auto begin(const _Container& _Cont) -> decltype(_Cont.begin()) { return _Cont.begin(); }
template <class _T, size_t _Size> FST_ALWAYS_INLINE constexpr _T* begin(_T (&_Array)[_Size]) noexcept { return _Array; }

template <class _Container> FST_ALWAYS_INLINE constexpr auto end(_Container& _Cont) -> decltype(_Cont.end()) { return _Cont.end(); }
template <class _Container> FST_ALWAYS_INLINE constexpr auto end(const _Container& _Cont) -> decltype(_Cont.end()) { return _Cont.end(); }
template <class _T, size_t _Size> FST_ALWAYS_INLINE constexpr _T* end(_T (&_Array)[_Size]) noexcept { return _Array + _Size; }

template <class _Container> FST_ALWAYS_INLINE constexpr auto cbegin(const _Container& _Cont) -> decltype(_Cont.cbegin()) { return _Cont.cbegin(); }
template <class _Container> FST_ALWAYS_INLINE constexpr auto cend(const _Container& _Cont) -> decltype(_Cont.cend()) { return _Cont.cend(); }

namespace detail
{
    template <typename T> auto is_container(int) -> decltype(_FST::container_data(_FST::declval<T&>()), _FST::container_size(_FST::declval<T&>()), void(), _FST::true_t{});
    template <typename T> _FST::false_t is_container(...);

    template <typename T> auto has_static_size(int) -> decltype(T::size(), void(), _FST::true_t{});
    template <typename T> _FST::false_t has_static_size(...);

    template <typename T> auto has_static_capacity(int) -> decltype(T::capacity(), void(), _FST::true_t{});
    template <typename T> _FST::false_t has_static_capacity(...);

    template <typename T> auto has_static_max_size(int) -> decltype(T::max_size(), void(), _FST::true_t{});
    template <typename T> _FST::false_t has_static_max_size(...);

    template <typename T>
    auto is_iterable_impl(int)
        -> decltype(_FST::begin(_FST::declval<T&>()) != _FST::end(_FST::declval<T&>()), // begin/end and operator !=
            void(), // Handle evil operator ,
            ++_FST::declval<decltype(_FST::begin(_FST::declval<T&>()))&>(), // operator
                                                                            // ++
            void(*_FST::begin(_FST::declval<T&>())), // operator*
            _FST::true_t{});

    template <typename T> _FST::false_t is_iterable_impl(...);

    template <typename T>
    auto is_const_iterable_impl(int)
        -> decltype(_FST::cbegin(_FST::declval<const T&>()) != _FST::cend(_FST::declval<const T&>()), // begin/end
                                                                                                    // and
                                                                                                    // operator !=
            void(), // Handle evil operator ,
            ++_FST::declval<const decltype(_FST::cbegin(_FST::declval<const T&>()))&>(), // operator
                                                                                        // ++
            void(*_FST::cbegin(_FST::declval<const T&>())), // operator*
            _FST::true_t{});

    template <typename T> _FST::false_t is_const_iterable_impl(...);
} // namespace detail

// is_container
template <typename T> using is_container = decltype(detail::is_container<T>(0));
template <class _T> FST_INLINE_VAR constexpr bool is_container_v = _FST::is_container<_T>::value;



// has_static_size
template <typename T> using has_static_size = decltype(detail::has_static_size<T>(0));
template <class _T> FST_INLINE_VAR constexpr bool has_static_size_v = _FST::has_static_size<_T>::value;

// has_static_capacity
template <typename T> using has_static_capacity = decltype(detail::has_static_capacity<T>(0));
template <class _T> FST_INLINE_VAR constexpr bool has_static_capacity_v = _FST::has_static_capacity<_T>::value;

// has_static_max_size
template <typename T> using has_static_max_size = decltype(detail::has_static_max_size<T>(0));
template <class _T> FST_INLINE_VAR constexpr bool has_static_max_size_v = _FST::has_static_max_size<_T>::value;

template <class _T, bool _HasStaticSize = _FST::has_static_size<_T>::value> struct static_size : _FST::integral_constant<size_t, 0> {};
template <class _T> struct static_size<_T, true> : _FST::integral_constant<size_t, _T::size()> {};
template <class _T> FST_INLINE_VAR constexpr size_t static_size_v = _FST::static_size<_T>::value;

template <class _T, bool _HasStaticSize = _FST::has_static_capacity<_T>::value> struct static_capacity : _FST::integral_constant<size_t, 0> {};
template <class _T> struct static_capacity<_T, true> : _FST::integral_constant<size_t, _T::capacity()> {};
template <class _T> FST_INLINE_VAR constexpr size_t static_capacity_v = _FST::static_capacity<_T>::value;


// is_iterable
template <typename T> using is_iterable = decltype(detail::is_iterable_impl<T>(0));
template <class _T> FST_INLINE_VAR constexpr bool is_iterable_v = _FST::is_iterable<_T>::value;

// is_const_iterable
template <typename T> using is_const_iterable = decltype(detail::is_const_iterable_impl<T>(0));
template <class _T> FST_INLINE_VAR constexpr bool is_const_iterable_v = _FST::is_const_iterable<_T>::value;

// container_data_type
template <class _T> using container_data_type = _FST::remove_pointer<decltype(_FST::container_data(_FST::declval<_T&>()))>;
template <class _T> using container_data_type_t = typename container_data_type<_T>::type;

// container_value_type
template <class _T> using container_value_type = _FST::remove_pointer<decltype(_FST::container_data(_FST::declval<_T&>()))>;
template <class _T> using container_value_type_t = typename container_value_type<_T>::type;

// container_iterator_type
template <class _Container, class = void> struct container_iterator_type {};

template <class _Container> struct container_iterator_type<_Container, _FST::enable_if_t<_FST::is_iterable_v<_Container>>> {
    using type = typename _Container::iterator;
};

template <class _T> using container_iterator_type_t = typename container_iterator_type<_T>::type;



        template <typename T>
        using has_push_back_t = decltype(_FST::declval<T>().push_back(_FST::declval<_FST::container_data_type_t<T>>() ) );



// has_push_back
template <typename T> using has_push_back = _FST::is_detected<has_push_back_t, T>;
template <class _T> FST_INLINE_VAR constexpr bool has_push_back_v = _FST::has_push_back<_T>::value;



namespace detail {
namespace { // avoid ODR-violation
    //template <class T> auto test_sizable(int) -> decltype(sizeof(T), _FST::true_t{});
    //template <class> auto test_sizable(...) -> _FST::false_t;

    //template <class T> auto test_nonconvertible_to_int(int) -> decltype(static_cast<_FST::false_t (*)(int)>(nullptr)(_FST::declval<T>()));
    //template <class> auto test_nonconvertible_to_int(...) -> _FST::true_t;

    //template <class T> constexpr bool is_scoped_enum_impl = _FST::conjunction_v<decltype(test_sizable<T>(0)), decltype(test_nonconvertible_to_int<T>(0))>;
} // namespace
} // namespace detail

//template <class> struct is_scoped_enum : _FST::false_t {};
//template <class E> struct is_scoped_enum<E> : _FST::bool_t<detail::is_scoped_enum_impl<E>> {};
//template <class _T> FST_INLINE_VAR constexpr bool is_scoped_enum_v = _FST::is_scoped_enum<_T>::value;

// is_allocator
template <class _T, class = void> struct is_allocator : _FST::false_t {};

template <class _T>
struct is_allocator<_T,
    _FST::void_t<typename _T::value_type,
        decltype(_FST::declval<_T&>().deallocate(_FST::declval<_T&>().allocate(size_t{ 1 }), size_t{ 1 }))>>
    : _FST::true_t {};


template <class StringType> FST_NODISCARD const typename StringType::value_type* to_cstr( const StringType& str) noexcept { return str.c_str(); }

template <class T, _FST::enable_if_t<_FST::is_same<T, const char*>::value, int> = 0>
FST_NODISCARD const char* to_cstr(T str) noexcept { return str; }

template <class T, size_t Size> FST_NODISCARD const char* to_cstr(const T (&str)[Size]) noexcept { return str; }

template <auto V, typename E = _FST::decay_t<decltype(V)>, _FST::enable_if_t<_FST::is_enum_v<E>, int> = 0>
using enum_constant = _FST::integral_constant<E, V>;

template <class _T, _T... _Values>
struct integer_sequence {
    static_assert(_FST::is_integral_v<_T>, "integer_sequence<T, I...> requires T to be an integral type.");
    using value_type = _T;
    FST_NODISCARD static constexpr size_t size() noexcept { return sizeof...(_Values); }
};

template <size_t... _Indexes> using index_sequence = _FST::integer_sequence<size_t, _Indexes...>;

template <class _T, size_t N, size_t... _Values>
FST_NODISCARD FST_ALWAYS_INLINE constexpr auto make_index_sequence_impl() noexcept {
    if constexpr (N == 0) {
        return _FST::integer_sequence<_T, _Values...>();
    } else {
        return _FST::make_index_sequence_impl<_T, N - 1, N - 1, _Values...>();
    }
}
template <class _T, size_t N> using make_integer_sequence = _FST::decay_t<decltype(_FST::make_index_sequence_impl<_T, N>())>;

// conditional_result_t
template <class T1, class T2> using conditional_result_t = decltype(false ? _FST::declval<T1>() : _FST::declval<T2>());

// decay_conditional_result
template <class, class, class = void> struct decay_conditional_result {};

template <class T1, class T2>
struct decay_conditional_result<T1, T2, _FST::void_t<conditional_result_t<T1, T2>>> : _FST::decay<conditional_result_t<T1, T2>> {};

// common_type
template <class...> struct common_type {};
template <class... _Ts> using common_type_t = typename common_type<_Ts...>::type;

namespace detail {
    template <class T1, class T2> using d_common_res = decay_conditional_result<T1, T2>;
    template <class T1, class T2> using void_cond_res = _FST::void_t<conditional_result_t<T1, T2>>;

    template <class T1, class T2, class = void> struct common_type_2_impl : d_common_res<const T1&, const T2&> {};
    template <class T1, class T2> struct common_type_2_impl<T1, T2, void_cond_res<T1, T2>> : d_common_res<T1, T2> {};

    template <class _Void, class T1, class T2, class... R>
    struct common_type_multi_impl {};

    template <class T1, class T2, class... R>
    struct common_type_multi_impl<_FST::void_t<common_type_t<T1, T2>>, T1, T2, R...> : common_type<common_type_t<T1, T2>, R...> {};
} // namespace detail

template <class T> struct common_type<T> : common_type<T, T> {};

template <class T1, class T2>
struct common_type<T1, T2> : _FST::conditional_t<_FST::are_same_decay_v<T1, T2>, detail::common_type_2_impl<T1, T2>,
                                 common_type<_FST::decay_t<T1>, _FST::decay_t<T2>>> {};

template <class T1, class T2, class... R> struct common_type<T1, T2, R...> : detail::common_type_multi_impl<void, T1, T2, R...> {};

// underlying_type
template <class _T, class = _FST::enable_if_t<_FST::is_enum_v<_T>, int>> struct underlying_type { using type = FST_UNDERLYING_TYPE(_T); };
template <class _T> using underlying_type_t = typename underlying_type<_T>::type;

template <class _Enum>
FST_ALWAYS_INLINE constexpr _FST::underlying_type_t<_Enum> to_underlying(_Enum e) noexcept {
    return static_cast<_FST::underlying_type_t<_Enum>>(e);
}

template <size_t N> using make_index_sequence = _FST::decay_t<decltype(_FST::make_index_sequence_impl<size_t, N>())>;
template <class... _Types> using index_sequence_for = _FST::make_index_sequence<sizeof...(_Types)>;

namespace detail
{
    template <class _Stream, typename = void, typename... Args> struct has_stream_operator : _FST::false_t {};

    template <class _Stream, typename... Args> struct has_stream_operator<_Stream,
        _FST::void_t<decltype(_FST::declval<_Stream>().operator<<(_FST::declval<Args>()...))>, Args...> : _FST::true_t {};

    template <class _Stream, typename = void, typename... Args> struct has_global_stream_operator : _FST::false_t {};

    template <class _Stream, typename... Args> struct has_global_stream_operator<_Stream,
        decltype(operator<<(_FST::declval<_Stream&>(), _FST::declval<const Args&>()...)), Args...> : _FST::true_t {};
} // namespace detail.

template <class _Stream, typename... Args>
using has_stream_operator = _FST::disjunction<detail::has_stream_operator<_Stream, void, Args...>,
    detail::has_global_stream_operator<_Stream, _Stream&, Args...>>;

template <class _Stream, typename... Args>
using has_global_stream_operator = 
    detail::has_global_stream_operator<_Stream, _Stream&, Args...>;


namespace detail {
template <class T>
struct invoke_impl {
    template <class F, class... Args>
    static auto call(F&& f, Args&&... args) -> decltype(_FST::forward<F>(f)(_FST::forward<Args>(args)...));
};

template <class B, class MT>
struct invoke_impl<MT B::*> {
    template <class T, class Td = _FST::decay_t<T>, class = _FST::enable_if_t<_FST::is_base_of_v<B, Td>>>
    static auto get(T&& t) -> T&&;

    template <class T, class Td = _FST::decay_t<T>, class = _FST::enable_if_t<!_FST::is_base_of_v<B, Td>>>
    static auto get(T&& t) -> decltype(*_FST::forward<T>(t));

    template <class T, class... Args, class MT1, class = _FST::enable_if_t<_FST::is_function_v<MT1>>>
    static auto call(MT1 B::*pmf, T&& t, Args&&... args) -> decltype((invoke_impl::get(_FST::forward<T>(t)).*pmf)(_FST::forward<Args>(args)...));

    template <class T> static auto call(MT B::*pmd, T&& t) -> decltype(invoke_impl::get(_FST::forward<T>(t)).*pmd);
};

template <class F, class... Args, class Fd = _FST::decay_t<F>>
auto INVOKE(F&& f, Args&&... args) -> decltype(invoke_impl<Fd>::call(_FST::forward<F>(f), _FST::forward<Args>(args)...));

//
template <class _Void, class, class...> struct invoke_result {};

template <class F, class... Args>
struct invoke_result<decltype(void(INVOKE(_FST::declval<F>(), _FST::declval<Args>()...))), F, Args...> {
    using type = decltype(INVOKE(_FST::declval<F>(), _FST::declval<Args>()...));
};

template <class C, class Pointed, class T1, class... Args>
constexpr decltype(auto) invoke_memptr(Pointed C::*f, T1&& t1, Args&&... args) {
    if constexpr (_FST::is_function_v<Pointed>) {
        if constexpr (_FST::is_base_of_v<C, _FST::decay_t<T1>>) {
            return (_FST::forward<T1>(t1).*f)(_FST::forward<Args>(args)...);
        } else {
            return ((*_FST::forward<T1>(t1)).*f)(_FST::forward<Args>(args)...);
        }
    } else {
        static_assert(_FST::is_object_v<Pointed> && sizeof...(args) == 0);
        if constexpr (_FST::is_base_of_v<C, _FST::decay_t<T1>>) {
            return _FST::forward<T1>(t1).*f;
        } else {
            return (*_FST::forward<T1>(t1)).*f;
        }
    }
}

} // namespace detail

//
template <class _To> void implicitly_convert_to(_To) noexcept;

//
template <class _T> _T returns_exactly() noexcept;

// is_invoke_convertible
template <class _From, class _To, class = void> struct is_invoke_convertible : _FST::false_t {};

template <class _From, class _To> struct is_invoke_convertible<_From, _To,
    _FST::void_t<decltype(_FST::implicitly_convert_to<_To>(_FST::returns_exactly<_From>()))>> : _FST::true_t {};

// invoke_result
template <class F, class... ArgTypes> struct invoke_result : detail::invoke_result<void, F, ArgTypes...> {};
template <class F, class... ArgTypes> using invoke_result_t = typename _FST::invoke_result<F, ArgTypes...>::type;

// is_invokable
template <class F, class... ArgTypes> struct is_invocable : _FST::type_exist<_FST::type_t, _FST::invoke_result<F, ArgTypes...>> {};
template <class Fn, class... ArgTypes> FST_INLINE_VAR constexpr bool is_invocable_v = _FST::is_invocable<Fn, ArgTypes...>::value;

namespace detail {
    template <bool, class R, class F, class... ArgTypes> struct is_invocable_r : _FST::false_t {};

    template <class R, class F, class... ArgTypes>
    struct is_invocable_r<true, R, F, ArgTypes...> : _FST::is_invoke_convertible<_FST::invoke_result_t<F, ArgTypes...>, R> { };
} // namespace detail

// is_invocable_r
template <class R, class F, class... ArgTypes> struct is_invocable_r : detail::is_invocable_r<_FST::is_invocable_v<F, ArgTypes...>, R, F, ArgTypes...> {};

template <class R, class Fn, class... ArgTypes> FST_INLINE_VAR constexpr bool is_invocable_r_v = _FST::is_invocable_r<R, Fn, ArgTypes...>::value;

// invoke
template <class _Fct, class... _Args, _FST::enable_if_t<_FST::is_invocable_v<_Fct, _Args...>, int> = 0>
FST_ALWAYS_INLINE constexpr _FST::invoke_result_t<_Fct, _Args...> invoke(_Fct&& f, _Args&&... args) noexcept {
    if constexpr (_FST::is_member_pointer_v<_FST::decay_t<_Fct>>) {
        return detail::invoke_memptr(f, _FST::forward<_Args>(args)...);
    } else {
        return _FST::forward<_Fct>(f)(_FST::forward<_Args>(args)...);
    }
}

template <class _Ret, class _Fct, class... _Args, _FST::enable_if_t<_FST::is_invocable_r_v<_Ret, _Fct, _Args...>, int> = 0>
FST_ALWAYS_INLINE constexpr _Ret invoke_r(_Fct&& f, _Args&&... args) noexcept {
    if constexpr (_FST::is_void_v<_Ret>) {
        _FST::invoke(_FST::forward<_Fct>(f), _FST::forward<_Args>(args)...);
    } else {
        return _FST::invoke(_FST::forward<_Fct>(f), _FST::forward<_Args>(args)...);
    }
}

// /////////////////////////////////////////////////////////////////////////////
// Detect if provided type has operator(). Use inside fea::is_detected
template <class _T> using has_paren_operator = decltype(&_T::operator());

namespace detail {
template <class, class, bool, class...> struct member_func_ptr { using type = void*; };
template <class Ret, class _T, class... Rest> struct member_func_ptr<Ret, _T, true, Rest...> { using type = Ret (_T::*)(Rest...); };
} // namespace detail

template <class... _Args> struct type_list{

   template<template <class... > class _T>
    using apply = _T<_Args...>;
};

/*
fea::member_func_ptr is a trait which constructs a member function pointer,
given Ret and Args...
It uses the first argument of Args as the class type.
If no Args are provided, aliases void*.
*/

template <class, class...> struct member_func_ptr { using type = void*; };
template <class Ret, class T, class... Rest> struct member_func_ptr<Ret, T*, Rest...> : detail::member_func_ptr<Ret, T, _FST::is_class<T>::value, Rest...> {};
template <class Ret, class... Args> using member_func_ptr_t = typename member_func_ptr<Ret, Args...>::type;

// func_ret
template <class> struct function_return;
template <class Ret, class... Args> struct function_return<Ret(Args...)> { using type = Ret; };
template <class Ret, class... Args> struct function_return<Ret (*)(Args...)> { using type = Ret; };
template <class Ret, class T, class... Args> struct function_return<Ret (T::*)(Args...)> { using type = Ret; };
template <class Ret, class T, class... Args> struct function_return<Ret (T::*)(Args...) const> { using type = Ret; };
template <class Func> struct function_return { using type = typename function_return<decltype(&Func::operator())>::type; };
template <class Func> using function_return_t = typename function_return<Func>::type;

// func_args
template <class> struct function_args;
template <class Ret, class... Args> struct function_args<Ret(Args...)> { using type = _FST::type_list<Args...>; };
template <class Ret, class... Args> struct function_args<Ret (*)(Args...)> { using type = _FST::type_list<Args...>; };
template <class Ret, class T, class... Args> struct function_args<Ret (T::*)(Args...)> { using type = _FST::type_list<Args...>; };
template <class Ret, class T, class... Args> struct function_args<Ret (T::*)(Args...) const> { using type = _FST::type_list<Args...>; };
template <class Func> struct function_args { using type = typename function_args<decltype(&Func::operator())>::type; };
template <class Func> using function_args_t = typename function_args<Func>::type;

/*
The following based off of Functional C++ blog post :
https://functionalcpp.wordpress.com/2013/08/05/function-traits/
Extracts pretty much verything from a function pointer.
*/

 //drop_first
 //Drop the first type from a parameter pack.
template <class...> struct drop_first;

template <class T, class... _Args> struct drop_first<_FST::type_list<T, _Args...>> { using type = _FST::type_list<_Args...>; };
//template <class T, class... _Args> struct drop_first<_FST::tuple<T, _Args...>> { using type = _FST::tuple<_Args...>; };

// function_traits
template <class F> struct function_traits;
template <class F> struct function_traits<F&> : function_traits<F> {};
template <class F> struct function_traits<F&&> : function_traits<F> {};
template <class R, class... _Args> struct function_traits<R (*)(_Args...)> : function_traits<R(_Args...)> {};
template <class C, class R, class... _Args> struct function_traits<R (C::*)(_Args...)> : function_traits<R(C&, _Args...)> {};
template <class C, class R, class... _Args> struct function_traits<R (C::*)(_Args...) const> : function_traits<R(C&, _Args...)> {};
template <class C, class R> struct function_traits<R C::*> : function_traits<R(C&)> {};

template <class R, class... _Args>
struct function_traits<R(_Args...)> {
    using call_type = function_traits;
    using return_t = R;
    using args_tuple = _FST::type_list<_Args...>;
    using args_decay_tuple = _FST::type_list<_FST::decay_t<_Args>...>;
    static constexpr size_t arity = sizeof...(_Args);

    template <size_t N>
    struct argument {
        static_assert(N < arity, "error: invalid parameter index.");
        //using type = _FST::type_tuple_element_t<N, _FST::type_list<_Args...>>;
    };
};

// functor
template <class F>
struct function_traits {
    using call_type = function_traits<decltype(&F::operator())>;
    using return_t = typename call_type::return_t;
    using args = typename drop_first<typename call_type::args>::type;
    using args_decay = typename drop_first<typename call_type::args_decay>::type;
    static constexpr size_t arity = call_type::arity - 1;

    template <size_t N>
    struct argument {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename call_type::template argument<N + 1>::type;
    };
};

    // clang-format on

    //struct source_location {
    //    using size_type = uint_least32_t;
    //    FST_NODISCARD static consteval source_location current(const size_type nline = __builtin_LINE(),
    //        const size_type col = __builtin_COLUMN(), const char* const file = __builtin_FILE(),
    //        const char* const fct = __builtin_FUNCTION()) noexcept {
    //        source_location loc;
    //        loc._line = nline;
    //        loc._column = col;
    //        loc._file = file;
    //        loc._function = fct;
    //        return loc;
    //    }
    //
    //    FST_NODISCARD_CTOR constexpr source_location() noexcept = default;
    //    FST_NODISCARD constexpr size_type line() const noexcept { return _line; }
    //    FST_NODISCARD constexpr size_type column() const noexcept { return _column; }
    //    FST_NODISCARD constexpr const char* file_name() const noexcept { return _file; }
    //    FST_NODISCARD constexpr const char* function_name() const noexcept { return _function; }
    //
    //  private:
    //    size_type _line{};
    //    size_type _column{};
    //    const char* _file = "";
    //    const char* _function = "";
    //};
    //
    //template <class _CharT>
    //inline _FST::output_stream<_CharT>& operator<<(_FST::output_stream<_CharT>& stream, const _FST::source_location& loc) {
    //    stream << loc.file_name() << _FST::space << loc.function_name() << _FST::space << loc.line() << _FST::space
    //           << loc.column();
    //    return stream;
    //}

    //
    //
    //

FST_END_NAMESPACE
