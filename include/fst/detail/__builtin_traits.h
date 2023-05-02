
#if __FST_MSVC__ || __FST_CLANG__
#define FST_IS_CONVERTIBLE_TO(_From, _To) __is_convertible_to(_From, _To)
#define FST_IS_BASE_OF(_Base, _Derived) __is_base_of(_Base, _Derived)
#define FST_IS_TRIVIALLY_CONSTRUCTIBLE(...) (__is_trivially_constructible(__VA_ARGS__))
#define FST_IS_TRIVIAL(_T) (__is_trivially_constructible(_T) && __is_trivially_copyable(_T))
#define FST_IS_CONSTRUCTIBLE(...) __is_constructible(__VA_ARGS__)
#define FST_IS_TRIVIALLY_COPYABLE(X) __is_trivially_copyable(X)
#define FST_IS_TRIVIALLY_DESTRUCTIBLE(X) __is_trivially_destructible(X)
#define FST_IS_ASSIGNABLE(T, U) __is_assignable(T, U)
#define FST_IS_EMPTY(X) (__is_empty(X))
#define FST_IS_ENUM(X) (__is_enum(X))
#define FST_IS_CLASS(X) (__is_class(X))
#define FST_IS_FINAL(X) (__is_final(X))
#define FST_IS_ABSTRACT(X) (__is_abstract(X))
#define FST_IS_STANDARD_LAYOUT(X) (__is_standard_layout(X))

#define FST_UNDERLYING_TYPE(X) __underlying_type(X)
#define FST_ADDRESSOF(X) __builtin_addressof(X)
#else

#include <type_traits>
#include <memory>

#define FST_IS_CONVERTIBLE_TO(_From, _To) (std::is_convertible<_From, _To>::value)
#define FST_IS_BASE_OF(_Base, _Derived) (std::is_base_of<_Base, _Derived>::value)
#define FST_IS_TRIVIAL(_T) (std::is_trivial_v<_T>)
#define FST_IS_CONSTRUCTIBLE(...) (std::is_constructible<__VA_ARGS__>::value)
#define FST_IS_TRIVIALLY_CONSTRUCTIBLE(...) (std::is_trivially_constructible<__VA_ARGS__>::value)
#define FST_IS_TRIVIALLY_COPYABLE(X) (std::is_trivially_copyable<X>::value)
#define FST_IS_TRIVIALLY_DESTRUCTIBLE(X) (std::is_trivially_destructible<X>::value)
#define FST_IS_ASSIGNABLE(T, U) (std::is_assignable<T, U>::value)
#define FST_IS_EMPTY(X) (std::is_empty<X>::value)
#define FST_IS_ENUM(X) (std::is_enum<X>::value)
#define FST_IS_CLASS(X) (std::is_class<X>::value)
#define FST_IS_FINAL(X) (std::is_final<X>::value)
#define FST_IS_ABSTRACT(X) (std::is_abstract<X>::value)
#define FST_IS_STANDARD_LAYOUT(X) (std::is_standard_layout<X>::value)
#define FST_UNDERLYING_TYPE(X) std::underlying_type_t<X>
#define FST_ADDRESSOF(X) (std::addressof(X))
#endif

FST_BEGIN_NAMESPACE
FST_END_NAMESPACE
