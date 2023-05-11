

/// @macro FST_GNUC_PREREQ
/// Extend the default __GNUC_PREREQ even if glibc's features.h isn't available.
#ifndef FST_GNUC_PREREQ
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define FST_GNUC_PREREQ(maj, min, patch) ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >= ((maj) << 20) + ((min) << 10) + (patch))

#elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#define FST_GNUC_PREREQ(maj, min, patch) ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))

#else
#define FST_GNUC_PREREQ(maj, min, patch) 0
#endif
#endif

//
/// Only use __has_cpp_attribute in C++ mode. GCC defines __has_cpp_attribute in
/// C mode, but the :: in __has_cpp_attribute(scoped::attribute) is invalid.
#ifndef FST_HAS_CPP_ATTRIBUTE
#if defined(__cplusplus) && defined(__has_cpp_attribute)
#define FST_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define FST_HAS_CPP_ATTRIBUTE(x) 0
#endif
#endif

#if __FST_CPP_17__
#define FST_INLINE_VAR inline
#else
#define FST_INLINE_VAR static
#endif

/// @macro FST_ALWAYS_INLINE
/// On compilers where we have a directive to do so, mark a method "always
/// inline" because it is performance sensitive. GCC 3.4 supported this but is
/// buggy in various cases and produces unimplemented errors, just use it in
/// GCC 4.0 and later.
#if __has_attribute(always_inline) || FST_GNUC_PREREQ(4, 0, 0)
#define FST_ALWAYS_INLINE inline //__attribute__((always_inline))
#elif __FST_MSVC__
#define FST_ALWAYS_INLINE __forceinline
#else
#define FST_ALWAYS_INLINE inline
#endif

/// @macro FST_LIKELY and FST_UNLIKELY
#if __has_builtin(__builtin_expect) || FST_GNUC_PREREQ(4, 0, 0)
#define FST_LIKELY(EXPR) __builtin_expect((bool) (EXPR), true)
#define FST_UNLIKELY(EXPR) __builtin_expect((bool) (EXPR), false)
#else
#define FST_LIKELY(EXPR) (EXPR)
#define FST_UNLIKELY(EXPR) (EXPR)
#endif

// Microsoft Specific
//__declspec(noinline) tells the compiler to never inline a particular member function (function in a class).
// It may be worthwhile to not inline a function if it is small and not critical to the performance of your code. That
// is, if the function is small and not likely to be called often, such as a function that handles an error condition.
// Keep in mind that if a function is marked noinline, the calling function will be smaller and thus, itself a candidate
// for compiler inlining.
#if __FST_MSVC__
#define FST_NOINLINE __declspec(noinline)
#else
#define FST_NOINLINE
#endif //

/// @macro FST_ATTRIBUTE_UNUSED
/// Some compilers warn about unused functions. When a function is sometimes
/// used or not depending on build settings (e.g. a function only called from
/// within "assert"), this attribute can be used to suppress such warnings.
///
/// However, it shouldn't be used for unused *variables*, as those have a much
/// more portable solution:
///   (void)unused_var_name;
/// Prefer cast-to-void wherever it is sufficient.
#if defined(__cplusplus) && __cplusplus > 201402L && FST_HAS_CPP_ATTRIBUTE(maybe_unused)
#define FST_ATTRIBUTE_UNUSED [[maybe_unused]]

#elif __has_attribute(unused) || FST_GNUC_PREREQ(3, 1, 0)
#define FST_ATTRIBUTE_UNUSED __attribute__((__unused__))

#else
#define FST_ATTRIBUTE_UNUSED
#endif

/// @macro FST_NODISCARD
/// Warn if a type or return value is discarded.
#if __FST_CPP_17__ && FST_HAS_CPP_ATTRIBUTE(nodiscard)
// Use the 'nodiscard' attribute in C++17 or newer mode.
#define FST_NODISCARD [[nodiscard]]

#elif FST_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)
// Clang in C++14 mode claims that it has the 'nodiscard' attribute, but also
// warns in the pedantic mode that 'nodiscard' is a C++17 extension (PR33518).
// Use the 'nodiscard' attribute in C++14 mode only with GCC.
// TODO: remove this workaround when PR33518 is resolved.
#define FST_NODISCARD [[nodiscard]]

#elif defined(__GNUC__) && FST_HAS_CPP_ATTRIBUTE(nodiscard)
#define FST_NODISCARD [[nodiscard]]

#else
#define FST_NODISCARD
#endif

///
#if FST_HAS_CPP_ATTRIBUTE(nodiscard) >= 201907L
#define FST_NODISCARD_CTOR FST_NODISCARD
#else
#define FST_NODISCARD_CTOR
#endif

/// @macro FST_ATTRIBUTE_RETURNS_NONNULL.
#if __has_attribute(returns_nonnull) || FST_GNUC_PREREQ(4, 9, 0)
#define FST_ATTRIBUTE_RETURNS_NONNULL __attribute__((returns_nonnull))
#elif __FST_MSVC__
#define FST_ATTRIBUTE_RETURNS_NONNULL _Ret_notnull_
#else
#define FST_ATTRIBUTE_RETURNS_NONNULL
#endif

/// @macro FST_ATTRIBUTE_NO_RETURN.
#if __has_attribute(noreturn) || FST_GNUC_PREREQ(4, 9, 0)
#define FST_ATTRIBUTE_NO_RETURN __attribute__((noreturn))
#elif __FST_MSVC__
#define FST_ATTRIBUTE_NO_RETURN __declspec(noreturn)
#else
#define FST_ATTRIBUTE_NO_RETURN
#endif

/// @macro FST_ATTRIBUTE_HIDDEN.
#if __has_attribute(visibility) || FST_GNUC_PREREQ(4, 0, 0)
#define FST_ATTRIBUTE_HIDDEN __attribute__((visibility("hidden")))
#else
#define FST_ATTRIBUTE_HIDDEN
#endif

#if __FST_MSVC__
#define FST_API __declspec(dllexport)
#else
#define FST_API __attribute__((visibility("default")))
#endif

/// @macro FST_PACKED
/// Used to specify a packed structure.
/// FST_PACKED(
///   struct A {
///      int i;
///      int j;
///      int k;
///      long long l;
///   });
///
/// FST_PACKED_START
/// struct B {
///   int i;
///   int j;
///   int k;
///   long long l;
/// };
/// FST_PACKED_END
#if __FST_MSVC__
#define FST_PACKED(d) __pragma(pack(push, 1)) d __pragma(pack(pop))
#define FST_PACKED_START __pragma(pack(push, 1))
#define FST_PACKED_N_START(N) __pragma(pack(push, N))
#define FST_PACKED_END __pragma(pack(pop))

#else
#define FST_PACKED(d) d __attribute__((packed))
#define FST_PACKED_START _Pragma("pack(push, 1)")
#define FST_PACKED_N_START(N) FST_PRAGMA(pack(push, N))
#define FST_PACKED_END _Pragma("pack(pop)")
#endif

/// @macro FST_PRETTY_FUNCTION
/// Gets a user-friendly looking function signature for the current scope
/// using the best available method on each platform.  The exact format of the
/// resulting string is implementation specific and non-portable, so this should
/// only be used, for example, for logging or diagnostics.
#if __FST_MSVC__
#define FST_PRETTY_FUNCTION __FUNCSIG__
#elif __FST_GCC__ || __FST_CLANG__ || __FST_MINGW__
#define FST_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
#define FST_PRETTY_FUNCTION __func__
#endif

///
#define FST_FALLTHROUGH [[fallthrough]]

///
//#define __FST_ASSERT_1(Expr) FST_NAMESPACE::custom_assert(#Expr, Expr,
//__FILE__, __LINE__) #define __FST_ASSERT_2(Expr, Msg)
// FST_NAMESPACE::custom_assert(#Expr, Expr, __FILE__, __LINE__, Msg) #define
//__FST_ASSERT_NTH_ARG(_1, _2, NAME, ...) NAME

///
#if __FST_ARCH_INTEL__
#if __FST_MSVC__
#define FST_NOP() _mm_pause()
#else
#define FST_NOP() __builtin_ia32_pause()
#endif
#elif __FST_ARCH_ARM__
#define FST_NOP() asm volatile("yield")
#else
#define FST_NOP() ({ (void) 0; })
#endif

//#define FST_UNUSED(X) __pragma(warning(suppress : 4100)) X
#define FST_UNUSED(...) (void) (__VA_ARGS__)

///
#ifdef __cpp_char8_t
#define FST_HAS_CHAR8_T 1
#define FST_IF_CHAR8_T(...) __VA_ARGS__

#define FST_UTF_CHAR_TYPES wchar_t, char8_t, char16_t, char32_t

#else
#define FST_HAS_CHAR8_T 0
#define FST_IF_CHAR8_T(...)
#define FST_UTF_CHAR_TYPES wchar_t, char16_t, char32_t
#endif

#define FST_INTEGER_TYPES \
    bool, char, signed char, unsigned char, FST_UTF_CHAR_TYPES, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long


#define FST_SIGNED_INTEGER_TYPES char, signed char, short, int, long, long long
#define FST_UNSIGNED_INTEGER_TYPES unsigned char, FST_UTF_CHAR_TYPES, unsigned short, unsigned int, unsigned long, unsigned long long


#define FST_FLOAT_TYPES float, double, long double

// fst_offsetof
#ifdef offsetof
#define fst_offsetof(s, m) offsetof(s, m)

#elif __has_builtin(__builtin_offsetof)
#define fst_offsetof(s, m) __builtin_offsetof(s, m)

#else
#define fst_offsetof(s, m) ((::size_t) & reinterpret_cast<char const volatile&>((((s*) 0)->m)))
#endif
