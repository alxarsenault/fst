

// clang-format off
FST_BEGIN_NAMESPACE

// array
namespace detail { template <class _T, size_t _Size, size_t _Alignment> struct basic_array; } // namespace detail
template <typename _T, size_t _Size, size_t _Alignment = alignof(_T)> using array = __fst::detail::basic_array<_T, _Size, _Alignment>;

// nullptr_t
using nullptr_t = decltype(nullptr);

// always_false
template <typename T> FST_INLINE_VAR constexpr bool always_false = false;

// empty
// template<class ...> struct empty_t {};

// void_t
template <class... _Types> using void_t = void;

// type_identity
template <class _T> struct type_identity { using type = _T; };
template <class _T> using type_identity_t = typename type_identity<_T>::type;

template <class _T, _T _Value>
struct integral_constant {
    static constexpr _T value = _Value;
    using value_type = _T;
    using type = integral_constant;
    FST_NODISCARD FST_ALWAYS_INLINE constexpr operator value_type() const noexcept { return value; }
    FST_NODISCARD FST_ALWAYS_INLINE constexpr value_type operator()() const noexcept { return value; }
};

// bool_constant
template <bool _Value> using bool_constant = __fst::integral_constant<bool, _Value>;
template <bool _Value> using bool_t = __fst::bool_constant<_Value>;
using true_t = bool_t<true>;
using false_t = bool_t<false>;

template <class _CharT> class output_stream;


struct empty_t {};

// nonesuch
struct nonesuch {
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

// enable_if
template <bool _Test, class _T = void> struct enable_if {};
template <class _T> struct enable_if<true, _T> { using type = _T; };
template <bool _Test, class _T = void> using enable_if_t = typename enable_if<_Test, _T>::type;

// conditional
template <bool _Test, class _T1, class _T2> struct conditional { using type = _T1; };
template <class _T1, class _T2> struct conditional<false, _T1, _T2> { using type = _T2; };
template <bool _Test, class _T1, class _T2> using conditional_t = typename conditional<_Test, _T1, _T2>::type;

// is_same
template <class, class> FST_INLINE_VAR constexpr bool is_same_v = false;
template <class _T> FST_INLINE_VAR constexpr bool is_same_v<_T, _T> = true;
template <class _T1, class _T2> struct is_same : __fst::bool_t<__fst::is_same_v<_T1, _T2>> {};


// is_different
template <class _T1, class _T2> FST_INLINE_VAR constexpr bool is_different_v = !__fst::is_same_v<_T1, _T2>;
template <class _T1, class _T2> struct is_different : __fst::bool_t<__fst::is_different_v<_T1, _T2>> {};

// is_const
template <class _T> FST_INLINE_VAR constexpr bool is_const_v = false;
template <class _T> FST_INLINE_VAR constexpr bool is_const_v<const _T> = true;
template <class _T> struct is_const : __fst::bool_t<__fst::is_const_v<_T>> {};

// is_volatile
template <class _T> FST_INLINE_VAR constexpr bool is_volatile_v = false;
template <class _T> FST_INLINE_VAR constexpr bool is_volatile_v<volatile _T> = true;
template <class _T> struct is_volatile : __fst::bool_t<__fst::is_volatile_v<_T>> {};

// is_lvalue_reference
template <class _T> FST_INLINE_VAR constexpr bool is_lvalue_reference_v = false;
template <class _T> FST_INLINE_VAR constexpr bool is_lvalue_reference_v<_T&> = true;
template <class _T> struct is_lvalue_reference : __fst::bool_t<__fst::is_lvalue_reference_v<_T>> {};

// is_rvalue_reference
template <class _T> FST_INLINE_VAR constexpr bool is_rvalue_reference_v = false;
template <class _T> FST_INLINE_VAR constexpr bool is_rvalue_reference_v<_T&&> = true;
template <class _T> struct is_rvalue_reference : __fst::bool_t<__fst::is_rvalue_reference_v<_T>> {};

// is_reference
template <class _T> FST_INLINE_VAR constexpr bool is_reference_v = false;
template <class _T> FST_INLINE_VAR constexpr bool is_reference_v<_T&> = true;
template <class _T> FST_INLINE_VAR constexpr bool is_reference_v<_T&&> = true;
template <class _T> struct is_reference : __fst::bool_t<__fst::is_reference_v<_T>> {};

// is_pointer
template <class> FST_INLINE_VAR constexpr bool is_pointer_v = false;
template <class _T> FST_INLINE_VAR constexpr bool is_pointer_v<_T*> = true;
template <class _T> FST_INLINE_VAR constexpr bool is_pointer_v<_T* const> = true;
template <class _T> FST_INLINE_VAR constexpr bool is_pointer_v<_T* volatile> = true;
template <class _T> FST_INLINE_VAR constexpr bool is_pointer_v<_T* const volatile> = true;
template <class _T> struct is_pointer : __fst::bool_t<__fst::is_pointer_v<_T>> {};

// remove_pointer
template <class _T> struct remove_pointer { using type = _T; };
template <class _T> struct remove_pointer<_T*> { using type = _T; };
template <class _T> struct remove_pointer<_T* const> { using type = _T; };
template <class _T> struct remove_pointer<_T* volatile> { using type = _T; };
template <class _T> struct remove_pointer<_T* const volatile> { using type = _T; };
template <class _T> using remove_pointer_t = typename __fst::remove_pointer<_T>::type;

// remove_reference
template <class _T> struct remove_reference { using type = _T; };
template <class _T> struct remove_reference<_T&> { using type = _T; };
template <class _T> struct remove_reference<_T&&> { using type = _T; };
template <class _T> using remove_reference_t = typename __fst::remove_reference<_T>::type;

// remove_cv
template <class _T> struct remove_cv { using type = _T; };
template <class _T> struct remove_cv<const _T> { using type = _T; };
template <class _T> struct remove_cv<volatile _T> { using type = _T; };
template <class _T> struct remove_cv<const volatile _T> { using type = _T; };
template <class _T> using remove_cv_t = typename __fst::remove_cv<_T>::type;

template <class _T, class _T2>
using is_same_rcv = __fst::is_same<_T, __fst::remove_cv_t<_T2>>;

// remove_cvref
template <typename T> using remove_cvref = __fst::remove_cv<__fst::remove_reference_t<T>>;
template <typename T> using remove_cvref_t = typename __fst::remove_cvref<T>::type;

// is_void
template <class _T> FST_INLINE_VAR constexpr bool is_void_v = __fst::is_same_v<void, __fst::remove_cv_t<_T>>;
template <class _T> struct is_void : __fst::bool_t<is_void_v<_T>> {};

// add_const
template <class _T> struct add_const { using type = const _T; };
template <class _T> using add_const_t = typename __fst::add_const<_T>::type;

// remove_const
template <class _T> struct remove_const { using type = _T; };
template <class _T> struct remove_const<const _T> { using type = _T; };
template <class _T> using remove_const_t = typename __fst::remove_const<_T>::type;

// add_lvalue_reference
template <class _T, class = void> struct add_lvalue_reference { using type = _T; };
template <class _T> struct add_lvalue_reference<_T, __fst::void_t<_T&>> { using type = _T&; };
template <class _T> using add_lvalue_reference_t = typename __fst::add_lvalue_reference<_T>::type;

// add_rvalue_reference
template <class _T, class = void> struct add_rvalue_reference { using type = _T; };
template <class _T> struct add_rvalue_reference<_T, __fst::void_t<_T&>> { using type = _T&&; };
template <class _T> using add_rvalue_reference_t = typename __fst::add_rvalue_reference<_T>::type;

// is_array
template <class _T> FST_INLINE_VAR constexpr bool is_c_array_v = false;
template <class _T, size_t _Size> FST_INLINE_VAR constexpr bool is_c_array_v<_T[_Size]> = true;
template <class _T> FST_INLINE_VAR constexpr bool is_c_array_v<_T[]> = true;
template <class _T> struct is_c_array : __fst::bool_t<__fst::is_c_array_v<_T>> {};

// is_convertible
template <class _From, class _To> struct is_convertible : __fst::bool_t<FST_IS_CONVERTIBLE_TO(_From, _To)> {};
template <class _From, class _To> FST_INLINE_VAR constexpr bool is_convertible_v = __fst::is_convertible<_From, _To>::value;

// is_constructible
template <class _T, class... _Args> struct is_constructible : __fst::bool_t<FST_IS_CONSTRUCTIBLE(_T, _Args...)> {};
template <class _T, class... _Args> FST_INLINE_VAR constexpr bool is_constructible_v = __fst::is_constructible<_T, _Args...>::value;

// is_default_constructible
template <class _T> struct is_default_constructible : __fst::bool_t<FST_IS_CONSTRUCTIBLE(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_default_constructible_v = __fst::is_default_constructible<_T>::value;

// is_move_constructible
template <class _T> struct is_move_constructible : __fst::bool_t<__fst::is_constructible_v<_T, _T>> {};
template <class _T> FST_INLINE_VAR constexpr bool is_move_constructible_v = __fst::is_move_constructible<_T>::value;

// is_copy_constructible
template <class _T> struct is_copy_constructible : __fst::bool_t<__fst::is_constructible_v<_T, __fst::add_lvalue_reference_t<const _T>>> {};
template <class _T> FST_INLINE_VAR constexpr bool is_copy_constructible_v = __fst::is_copy_constructible<_T>::value;

// is_trivially_default_constructible
template <class _T> struct is_trivially_default_constructible : __fst::bool_t<FST_IS_TRIVIALLY_CONSTRUCTIBLE(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_trivially_default_constructible_v
    = __fst::is_trivially_default_constructible<_T>::value;

// is_trivially_copy_constructible
template <class _T> struct is_trivially_copy_constructible
    : __fst::bool_t<FST_IS_TRIVIALLY_CONSTRUCTIBLE(_T, __fst::add_lvalue_reference_t<const _T>)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_trivially_copy_constructible_v = __fst::is_trivially_copy_constructible<_T>::value;

// is_trivially_move_constructible
template <class _T> struct is_trivially_move_constructible : __fst::bool_t<FST_IS_TRIVIALLY_CONSTRUCTIBLE(_T, _T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_trivially_move_constructible_v = __fst::is_trivially_move_constructible<_T>::value;

// is_trivially_copyable
template <class _T> struct is_trivially_copyable : __fst::bool_t<FST_IS_TRIVIALLY_COPYABLE(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_trivially_copyable_v = __fst::is_trivially_copyable<_T>::value;

// is_trivially_destructible
template <class _T> struct is_trivially_destructible : __fst::bool_t<FST_IS_TRIVIALLY_DESTRUCTIBLE(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_trivially_destructible_v = __fst::is_trivially_destructible<_T>::value;

// is_assignable
template <class _To, class _From> struct is_assignable : __fst::bool_t<FST_IS_ASSIGNABLE(_To, _From)> {};
template <class _To, class _From> FST_INLINE_VAR constexpr bool is_assignable_v = __fst::is_assignable<_To, _From>::value;

// is_copy_assignable
template <class _T> struct is_copy_assignable
    : __fst::bool_t<FST_IS_ASSIGNABLE(__fst::add_lvalue_reference_t<_T>, __fst::add_lvalue_reference_t<const _T>)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_copy_assignable_v = __fst::is_copy_assignable<_T>::value;

// is_move_assignable
template <class _T> struct is_move_assignable : __fst::bool_t<FST_IS_ASSIGNABLE(__fst::add_lvalue_reference_t<_T>, _T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_move_assignable_v = __fst::is_move_assignable<_T>::value;

// is_base_of
template <class _Base, class _Derived> struct is_base_of : __fst::bool_t<FST_IS_BASE_OF(_Base, _Derived)> {};
template <class _Base, class _Derived> FST_INLINE_VAR constexpr bool is_base_of_v = __fst::is_base_of<_Base, _Derived>::value;

// is_trivial
template <class _T> struct is_trivial : __fst::bool_t<FST_IS_TRIVIAL(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_trivial_v = __fst::is_trivial<_T>::value;

// is_empty
template <class _T> struct is_empty : __fst::bool_t<FST_IS_EMPTY(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_empty_v = __fst::is_empty<_T>::value;

// is_enum
template <class _T> struct is_enum : __fst::bool_t<FST_IS_ENUM(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_enum_v = __fst::is_enum<_T>::value;

// is_class
template <class _T> struct is_class : __fst::bool_t<FST_IS_CLASS(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_class_v = __fst::is_class<_T>::value;

// is_final
template <class _T> struct is_final : __fst::bool_t<FST_IS_FINAL(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_final_v = __fst::is_final<_T>::value;

// is_abstract
template <class _T> struct is_abstract : __fst::bool_t<FST_IS_ABSTRACT(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_abstract_v = __fst::is_abstract<_T>::value;

// is_standard_layout
template <class _T> struct is_standard_layout : __fst::bool_t<FST_IS_STANDARD_LAYOUT(_T)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_standard_layout_v = __fst::is_standard_layout<_T>::value;

// is_function
FST_PRAGMA_PUSH()
FST_PRAGMA_DISABLE_WARNING_MSVC(4180)
template <class _T>
FST_INLINE_VAR constexpr bool is_function_v = !__fst::is_const_v<const _T> && !__fst::is_reference_v<_T>;
template <class _T>
struct is_function : __fst::bool_t<__fst::is_function_v<_T>> {};
FST_PRAGMA_POP()

// trait to check if a type is a pointer to function
    template <class T>
    struct is_function_pointer :__fst::false_t {};
    
    template <class T>
    struct is_function_pointer<T*> :__fst::is_function<T> {};
    
    template <class T>
    inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;

// extent
template <class _Ty, unsigned int _Ix = 0> FST_INLINE_VAR constexpr size_t extent_v = 0;
template <class _Ty, size_t _Nx> FST_INLINE_VAR constexpr size_t extent_v<_Ty[_Nx], 0> = _Nx;
template <class _Ty, unsigned int _Ix, size_t _Nx> FST_INLINE_VAR constexpr size_t extent_v<_Ty[_Nx], _Ix> = extent_v<_Ty, _Ix - 1>;
template <class _Ty, unsigned int _Ix> FST_INLINE_VAR constexpr size_t extent_v<_Ty[], _Ix> = extent_v<_Ty, _Ix - 1>;
template <class _Ty, unsigned int _Ix = 0> struct extent : __fst::integral_constant<size_t, extent_v<_Ty, _Ix>> {};

// remove_extent
template <class _Ty> struct remove_extent { using type = _Ty; };
template <class _Ty, size_t _Ix> struct remove_extent<_Ty[_Ix]> { using type = _Ty; };
template <class _Ty> struct remove_extent<_Ty[]> { using type = _Ty; };
template <class _Ty> using remove_extent_t = typename remove_extent<_Ty>::type;

// remove_all_extents
template <class _Ty> struct remove_all_extents { using type = _Ty; };
template <class _Ty, size_t _Ix> struct remove_all_extents<_Ty[_Ix]> { using type = typename remove_all_extents<_Ty>::type; };
template <class _Ty> struct remove_all_extents<_Ty[]> { using type = typename remove_all_extents<_Ty>::type; };
template <class _Ty> using remove_all_extents_t = typename remove_all_extents<_Ty>::type;

// add_pointer
template <class _T, class = void> struct add_pointer { using type = _T; };
template <class _Ty> struct add_pointer<_Ty, void_t<__fst::remove_reference_t<_Ty>*>> { using type = __fst::remove_reference_t<_Ty>*; };
template <class _T> using add_pointer_t = typename add_pointer<_T>::type;

// select
template <bool> struct select { template <class _T, class> using apply = _T; };
template <> struct select<false> { template <class, class _T> using apply = _T; };

// decay
template <class _T>
struct decay {
    using _T1 = __fst::remove_reference_t<_T>;
    using _T2 = typename __fst::select<is_function_v<_T1>>::template apply<__fst::add_pointer<_T1>, __fst::remove_cv<_T1>>;
    using type = typename __fst::select<is_c_array_v<_T1>>::template apply<__fst::add_pointer<__fst::remove_extent_t<_T1>>, _T2>::type;
};

template <class _T> using decay_t = typename decay<_T>::type;

// template <class _T, typename U>
// FST_INLINE_VAR constexpr bool is_decay_equal =
// __fst::is_same_v<__fst::decay_t<_T>, U>;

template <class _T> struct is_same_decay : __fst::is_same<__fst::decay_t<_T>, _T> {};
template <class _T> FST_INLINE_VAR constexpr bool is_same_decay_v = __fst::is_same_decay<_T>::value;

// is_null_pointer
template <class _T> struct is_null_pointer : __fst::is_same<__fst::nullptr_t, __fst::remove_cv_t<_T>> {};
template <class _T> FST_INLINE_VAR constexpr bool is_null_pointer_v = __fst::is_null_pointer<_T>::value;

// is_fst_array
//template <class T> struct is_fst_array : __fst::false_t {};
//template <class _T, size_t _Size> struct is_fst_array<__fst::array<_T, _Size>> : __fst::true_t {};

namespace detail
{
    template <bool _Value, class _T, class... _Ts> struct conjunction { using type = _T; };
    template <class _True, class _T, class... _Ts> struct conjunction<true, _True, _T, _Ts...> { using type = typename conjunction<_T::value, _T, _Ts...>::type; };

    template <bool _Value, class _T, class... _Ts> struct disjunction { using type = _T; };
    template <class _False, class _T, class... _Ts> struct disjunction<false, _False, _T, _Ts...> { using type = typename disjunction<_T::value, _T, _Ts...>::type; };
} // namespace detail

// conjunction
template <class... _Ts> struct conjunction : __fst::true_t {};
template <class _T, class... _Ts> struct conjunction<_T, _Ts...> : __fst::detail::conjunction<_T::value, _T, _Ts...>::type {};
template <class... _Ts> FST_INLINE_VAR constexpr bool conjunction_v = __fst::conjunction<_Ts...>::value;

// disjunction
template <class... _Traits> struct disjunction : __fst::false_t {};
template <class _T, class... _Ts> struct disjunction<_T, _Ts...> : __fst::detail::disjunction<_T::value, _T, _Ts...>::type {};
template <class... _Ts> FST_INLINE_VAR constexpr bool disjunction_v = __fst::disjunction<_Ts...>::value;

// negation
template <class _Trait> struct negation : __fst::bool_t<!static_cast<bool>(_Trait::value)> {};
template <class _Trait> FST_INLINE_VAR constexpr bool negation_v = __fst::negation<_Trait>::value;

// is_any_of
template <class _T, class... _Ts> struct is_any_of : __fst::disjunction<__fst::is_same<_T, _Ts>...> {};
template <class _T, class... _Ts> FST_INLINE_VAR constexpr bool is_any_of_v = __fst::disjunction_v<__fst::is_same<_T, _Ts>...>;

// is_all_same
template <typename _T, typename... _Ts> struct is_all_same : __fst::conjunction<__fst::is_same<_T, _Ts>...> {};
template <typename _T, typename... _Ts> FST_INLINE_VAR constexpr bool is_all_same_v = is_all_same<_T, _Ts...>::value;

// is_integral
template <class _T> FST_INLINE_VAR constexpr bool is_integral_v = __fst::is_any_of_v<__fst::remove_cv_t<_T>, FST_INTEGER_TYPES>;
template <class _T> struct is_integral : __fst::bool_t<__fst::is_integral_v<_T>> {};

// is_floating_point
template <class _T> FST_INLINE_VAR constexpr bool is_floating_point_v = __fst::is_any_of_v<__fst::remove_cv_t<_T>, FST_FLOAT_TYPES>;
template <class _T> struct is_floating_point : __fst::bool_t<is_floating_point_v<_T>> {};

// is_arithmetic
template <class _T> FST_INLINE_VAR constexpr bool is_arithmetic_v = __fst::is_integral_v<_T> || __fst::is_floating_point_v<_T>;
template <class _T> struct is_arithmetic : __fst::bool_t<__fst::is_arithmetic_v<_T>> {};

// is_object
template <class _T> FST_INLINE_VAR constexpr bool is_object_v = __fst::is_const_v<const _T> && !__fst::is_void_v<_T>;
template <class _T> struct is_object : __fst::bool_t<__fst::is_object_v<_T>> {};

// is_signed
template <class _T> struct is_signed : __fst::conditional_t<__fst::is_integral_v<_T>,
                       __fst::bool_t<(static_cast<__fst::remove_cv_t<_T>>(-1) < static_cast<__fst::remove_cv_t<_T>>(0))>,
                       __fst::bool_t<__fst::is_floating_point_v<_T>>> {};
template <class _T> FST_INLINE_VAR constexpr bool is_signed_v = __fst::is_signed<_T>::value;

FST_PRAGMA_PUSH()
FST_PRAGMA_DISABLE_WARNING_MSVC(4296) // prevent expression is always true
// is_unsigned
template <class _T> struct is_unsigned
    : __fst::conditional_t<__fst::is_integral_v<_T>,
          __fst::bool_t<(static_cast<__fst::remove_cv_t<_T>>(-1) >= static_cast<__fst::remove_cv_t<_T>>(0))>,
          __fst::false_t> {};
template <class _T> FST_INLINE_VAR constexpr bool is_unsigned_v = __fst::is_unsigned<_T>::value;
FST_PRAGMA_POP()

// make_unsigned
template <typename T> struct make_unsigned { using type = T; };
template <> struct make_unsigned<char> { using type = unsigned char; };
template <> struct make_unsigned<short> { using type = unsigned short; };
template <> struct make_unsigned<int> { using type = unsigned int; };
template <> struct make_unsigned<long> { using type = unsigned long; };
template <> struct make_unsigned<long long> { using type = unsigned long long; };
template <class _T> using make_unsigned_t = typename __fst::make_unsigned<_T>::type;

// is_nonbool_integral
template <class _T>
FST_INLINE_VAR constexpr bool is_nonbool_integral_v
    = __fst::is_integral_v<_T> && !__fst::is_same_v<__fst::remove_cv_t<_T>, bool>;
template <class _T>
struct is_nonbool_integral : __fst::bool_t<is_nonbool_integral_v<_T>> {};

// is_integer_convertible
template <typename T> using is_integer_convertible = __fst::bool_t<__fst::is_unsigned_v<T> || __fst::is_signed_v<T>>;
template <typename T> FST_INLINE_VAR constexpr bool is_integer_convertible_v = is_integer_convertible<T>::value;

// is_nonbool_integral_convertible
template <typename _T> FST_INLINE_VAR constexpr bool is_nonbool_integral_convertible_v
    = __fst::is_integer_convertible<_T>::value && !__fst::is_same_v<__fst::remove_cv_t<_T>, bool>;

template <typename _T> using is_nonbool_integral_convertible = __fst::bool_t<is_nonbool_integral_convertible_v<_T>>;

template <class _T> struct is_fundamental : __fst::disjunction<__fst::is_arithmetic<_T>, __fst::is_void<_T>, __fst::is_null_pointer<_T>> {};
template <class _T> FST_INLINE_VAR constexpr bool is_fundamental_v = __fst::is_fundamental<_T>::value;

template <class... _Ts> struct are_same_decay : __fst::conjunction<__fst::is_same_decay<_Ts>...> {};
template <class... _Ts> FST_INLINE_VAR constexpr bool are_same_decay_v = __fst::are_same_decay<_Ts...>::value;

template <class _T> struct is_trivial_cref : __fst::bool_t<__fst::is_trivially_copyable_v<_T> && sizeof(_T) <= sizeof(size_t)> {};
template <class _T> FST_INLINE_VAR constexpr bool is_trivial_cref_v = __fst::is_trivial_cref<_T>::value;

template <class _T> struct cref : __fst::conditional<__fst::is_trivial_cref_v<__fst::remove_cvref_t<_T>>, _T, const _T&> {};
template <class _T> using cref_t = typename __fst::cref<_T>::type;

namespace detail
{
    template <class _T> struct is_member_function_pointer : __fst::false_t {};
    template <class _T, class _U> struct is_member_function_pointer<_T _U::*> : __fst::is_function<_T> {};

    template <class> struct is_member_object_pointer : __fst::false_t {};
    template <class _T1, class _T2> struct is_member_object_pointer<_T1 _T2::*> : __fst::negation<__fst::is_function<_T1>> {};
} // namespace detail

// member_pointer_class
template <class> struct member_pointer_class {};
template <class _T, class _Class> struct member_pointer_class<_T _Class::*> { using type = _Class; };
template <class _T> using member_pointer_class_t = typename __fst::member_pointer_class<_T>::type;

// is_member_function_pointer
template <class _T> struct is_member_function_pointer : detail::is_member_function_pointer<__fst::remove_cv_t<_T>> {};
template <class _T> FST_INLINE_VAR constexpr bool is_member_function_pointer_v = __fst::is_member_function_pointer<_T>::value;

// is_member_object_pointer
template <class _T> struct is_member_object_pointer : detail::is_member_object_pointer<__fst::remove_cv_t<_T>> {};
template <class _T> FST_INLINE_VAR constexpr bool is_member_object_pointer_v = __fst::is_member_object_pointer<_T>::value;

// is_member_pointer
template <class _T> FST_INLINE_VAR constexpr bool is_member_pointer_v = __fst::is_member_object_pointer_v<_T> || __fst::is_member_function_pointer_v<_T>;
template <class _T> struct is_member_pointer : __fst::bool_t<__fst::is_member_pointer_v<_T>> {};

template <class _T> struct is_scalar : __fst::disjunction<is_arithmetic<_T>, __fst::is_enum<_T>, __fst::is_pointer<_T>,
                       __fst::is_member_pointer<_T>, __fst::is_null_pointer<_T>> {};

template <typename _T> FST_INLINE_VAR constexpr bool is_scalar_v = __fst::is_scalar<_T>::value;


template <class _Type, template <class...> class _Template>
FST_INLINE_VAR constexpr bool is_specialization_v = false; // true if and only if _Type is a specialization of _Template

template <template <class...> class _Template, class... _Types>
FST_INLINE_VAR constexpr bool is_specialization_v<_Template<_Types...>, _Template> = true;

template <class _Type, template <class...> class _Template>
struct is_specialization : __fst::bool_t<is_specialization_v<_Type, _Template>> {};


// variadic_arg_0
template <class _T0, class...> struct variadic_arg_0 { using type = _T0; };
template <class... _Ts> using variadic_arg_0_t = typename __fst::variadic_arg_0<_Ts...>::type;

// variadic_arg_1
template <class, class _T1, class...> struct variadic_arg_1 { using type = _T1; };
template <class... _Ts> using variadic_arg_1_t = typename __fst::variadic_arg_1<_Ts...>::type;

// variadic_arg_2
template <class, class, class _T2, class...> struct variadic_arg_2 { using type = _T2; };
template <class... _Ts> using variadic_arg_2_t = typename __fst::variadic_arg_2<_Ts...>::type;

// variadic_arg_3
template <class, class, class, class _T3, class...> struct variadic_arg_3 { using type = _T3; };
template <class... _Ts> using variadic_arg_3_t = typename __fst::variadic_arg_3<_Ts...>::type;

// variadic_arg_4
template <class, class, class, class, class _T4, class...> struct variadic_arg_4 { using type = _T4; };
template <class... _Ts> using variadic_arg_4_t = typename __fst::variadic_arg_4<_Ts...>::type;

// variadic_arg_5
template <class, class, class, class, class, class _T5, class...> struct variadic_arg_5 { using type = _T5; };
template <class... _Ts> using variadic_arg_5_t = typename __fst::variadic_arg_5<_Ts...>::type;

// variadic_arg_6
template <class, class, class, class, class, class, class _T6, class...> struct variadic_arg_6 { using type = _T6; };
template <class... _Ts> using variadic_arg_6_t = typename __fst::variadic_arg_6<_Ts...>::type;

// variadic_arg_7
template <class, class, class, class, class, class, class, class _T7, class...> struct variadic_arg_7 { using type = _T7; };
template <class... _Ts> using variadic_arg_7_t = typename __fst::variadic_arg_7<_Ts...>::type;

// variadic_arg_8
template <class, class, class, class, class, class, class, class, class _T8, class...> struct variadic_arg_8 { using type = _T8; };
template <class... _Ts> using variadic_arg_8_t = typename __fst::variadic_arg_8<_Ts...>::type;

// variadic_arg
template <size_t _Index, class... _Ts> struct variadic_arg;
template <class... _Ts> struct variadic_arg<0, _Ts...> : __fst::variadic_arg_0<_Ts...> {};
template <class... _Ts> struct variadic_arg<1, _Ts...> : __fst::variadic_arg_1<_Ts...> {};
template <class... _Ts> struct variadic_arg<2, _Ts...> : __fst::variadic_arg_2<_Ts...> {};
template <class... _Ts> struct variadic_arg<3, _Ts...> : __fst::variadic_arg_3<_Ts...> {};
template <class... _Ts> struct variadic_arg<4, _Ts...> : __fst::variadic_arg_4<_Ts...> {};
template <class... _Ts> struct variadic_arg<5, _Ts...> : __fst::variadic_arg_5<_Ts...> {};
template <class... _Ts> struct variadic_arg<6, _Ts...> : __fst::variadic_arg_6<_Ts...> {};
template <class... _Ts> struct variadic_arg<7, _Ts...> : __fst::variadic_arg_7<_Ts...> {};
template <class... _Ts> struct variadic_arg<8, _Ts...> : __fst::variadic_arg_8<_Ts...> {};
template <size_t _Index, class... _Ts> using variadic_arg_t = typename __fst::variadic_arg<_Index, _Ts...>::type;

// is_buffer_convertible
template <class _From, class _To> using is_buffer_convertible = __fst::is_convertible<_From (*)[], _To (*)[]>;

// detector
template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
struct detector {
    using value_t = __fst::false_t;
    using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, __fst::void_t<Op<Args...>>, Op, Args...> {
    using value_t = __fst::true_t;
    using type = Op<Args...>;
};

// is_detected
template <template <class...> class Op, class... Args>
using is_detected = typename __fst::detector<__fst::nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
FST_INLINE_VAR constexpr bool is_detected_v = is_detected<Op, Args...>::value;

template <template <class...> class Op, class... Args>
using is_detected_t = typename __fst::detector<__fst::nonesuch, void, Op, Args...>::type;

// are_detected
template <class T, template <class...> class... Ops>
using are_detected = __fst::conjunction<is_detected<Ops, T>...>;

template <class _T> using type_t = typename _T::type;

// type_exist
template <template <class...> class _Op, typename K> using type_exist = is_detected<_Op, K>;

// is_defined
template <class, class = void> struct is_defined : __fst::false_t {};

template <class T> struct is_defined<T, __fst::enable_if_t<__fst::is_object_v<T> && !__fst::is_pointer_v<T> && (sizeof(T) > 0)>> : __fst::true_t {};

template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
struct detector_value : __fst::false_t {};

///
template <class Default, template <class...> class Op, class... Args>
struct detector_value<Default, __fst::void_t<Op<Args...>>, Op, Args...> : __fst::true_t {};

template <class T, template <class...> class... Ops>
using has_members = __fst::conjunction<__fst::detector_value<__fst::nonesuch, void, Ops, T>...>;

template <class T, template <class...> class... Ops>
using enable_if_has_members_t = __fst::enable_if_t<__fst::has_members<T, Ops...>::value, __fst::nullptr_t>;

template <class T, template <class...> class... Ops>
using if_members = __fst::enable_if_has_members_t<T, Ops...>;

// dependent_type
template <class T, bool> struct dependent_type : T {};
template <bool Dummy, class D> using dependent_type_condition = typename dependent_type<__fst::type_identity<D>, Dummy>::type;
template <bool Dummy, class D> using enable_if_dependant_same = __fst::enable_if<__fst::is_same_v<D, __fst::true_t>>;
template <bool Dummy, class D> using enable_if_dependant_same_t = typename enable_if_dependant_same<Dummy, D>::type;

//
template <template <class...> class C> struct is_template_of_base { template <class T> struct type : __fst::false_t {}; template <class T> struct type<C<T>> : __fst::true_t {}; };

//
template <template <class...> class C, class T> using is_template_of = typename is_template_of_base<C>::template type<__fst::remove_cv_t<T>>;

//
template <typename T> using array_of_type = T (*)[];

/// is_array_of_type_convertible.
FST_END_NAMESPACE
// clang-format on
