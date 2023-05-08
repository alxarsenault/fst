//#include <stdio.h>
//extern "C" {
//using FILE_HANDLE = void;
//extern int printf(const char* const _Format, ...);
//
//extern int fprintf(FILE_HANDLE* const _Stream, const char* const _Format, ...);
//
//}

//#define fst_stderr 0

#if !defined(_HAS_EXCEPTIONS) || (defined(_HAS_EXCEPTIONS) && _HAS_EXCEPTIONS == 0)
#define FST_HAS_EXCEPT 0
#else
#define FST_HAS_EXCEPT 1
#endif

//
#if FST_USE_RELEASE_ASSERT
#define FST_HAS_ASSERT 1
#elif defined(NDEBUG)
#define FST_HAS_ASSERT 0
#else
#define FST_HAS_ASSERT 1
#endif // NDEBUG

/// @macro FST_DEBUGTRAP
/// On compilers which support it, expands to an expression which causes the
/// program to break while running under a debugger.
#if __has_builtin(__builtin_debugtrap)
#define FST_DEBUGTRAP() __builtin_debugtrap()

#elif __FST_MSVC__
// The __debugbreak intrinsic is supported by MSVC and breaks while
// running under the debugger, and also supports invoking a debugger
// when the OS is configured appropriately.
#define FST_DEBUGTRAP() __debugbreak()

#elif __FST_CLANG__ && (defined(unix) || defined(__unix__) || defined(__unix) || defined(__MACH__))
#include <signal.h>
#define FST_DEBUGTRAP() raise(SIGTRAP)

#else
#define FST_DEBUGTRAP() ::abort()
#endif

#if FST_USE_ASSERT_COLOR
#define fst_terminal_color(X) "\033[" #X "m"
#else
#define fst_terminal_color(X)
#endif

#if FST_HAS_ASSERT

///
#define fst_assert(...) FST_EXPAND(__FST_ASSERT_NTH_ARG(__VA_ARGS__, __FST_ASSERT_2, __FST_ASSERT_1)(__VA_ARGS__))
#define fst_cxpr_assert(...) FST_EXPAND(__FST_ASSERT_NTH_ARG(__VA_ARGS__, __FST_CXPR_ASSERT_2, __FST_CXPR_ASSERT_1)(__VA_ARGS__))

///
#define fst_error(Msg)                                                                            \
    FST_NAMESPACE::printf(fst_terminal_color(31) "error" fst_terminal_color(0) " : %s\n", (Msg)); \
    FST_ASSERT_HOOK((Msg), __FILE__, __LINE__);                                                   \
    FST_ASSERT_BREAK()

///
#define fst_warning(Msg) FST_NAMESPACE::printf(fst_terminal_color(33) "warning" fst_terminal_color(0) " : %s\n", (Msg))

#define __FST_ASSERT_NTH_ARG(_1, _2, NAME, ...) NAME

#ifndef FST_ASSERT_HOOK
#define FST_ASSERT_HOOK(Expr, File, Line)
#endif

#ifndef FST_ASSERT_BREAK
#define FST_ASSERT_BREAK() FST_DEBUGTRAP()
#endif
  //#define fst_cxpr_assert(CHECK) (FST_LIKELY(CHECK) ? void(0) : [] { assert(!#CHECK); }())




// No message in assert
#define __FST_CXPR_ASSERT_1(expr)                                   \
    (void) ((!(expr)) &&                                       \
            [](const char* expr_str)                           \
            {                                                  \
if(!__fst::is_constant_evaluated()){fst_assert(false);}\
                return false;                                  \
            }(#expr))

#define __FST_CXPR_ASSERT_2(expr, msg)                              \
    (void) ((!(expr)) &&                                       \
            [](const char* expr_str, const char* msg_str)      \
            {                                                  \
                return false;                                  \
            }(#expr, msg))


#if FST_USE_ASSERT_MSG
#define __FST_ASSERT_1(expr)                                                                          \
    (void) ((!(expr)) &&                                                                              \
            [](const char* expr_str)                                                                  \
            {                                                                                         \
                FST_NAMESPACE::warnprintf("assert failed\nexpected:\t %s\nsource:\t\t %s\nline:\t\t " \
                                          "%d\n",                                                     \
                    expr_str, __FILE__, __LINE__);                                                    \
                FST_ASSERT_HOOK(expr_str, __FILE__, __LINE__);                                        \
                FST_ASSERT_BREAK();                                                                   \
                return false;                                                                         \
            }(#expr))

#define __FST_ASSERT_2(expr, msg)                                                             \
    (void) ((!(expr)) &&                                                                      \
            [](const char* expr_str, const char* msg_str)                                     \
            {                                                                                 \
                FST_NAMESPACE::warnprintf("assert failed:\t %s\nexpected:\t %s\nsource:\t\t " \
                                          "%s\nline:\t\t %d\n",                               \
                    msg_str, expr_str, __FILE__, __LINE__);                                   \
                FST_ASSERT_HOOK(expr_str, __FILE__, __LINE__);                                \
                FST_ASSERT_BREAK();                                                           \
                return false;                                                                 \
            }(#expr, msg))

#else
// No message in assert
#define __FST_ASSERT_1(expr)                                   \
    (void) ((!(expr)) &&                                       \
            [](const char* expr_str)                           \
            {                                                  \
                FST_ASSERT_HOOK(expr_str, __FILE__, __LINE__); \
                FST_ASSERT_BREAK();                            \
                return false;                                  \
            }(#expr))

#define __FST_ASSERT_2(expr, msg)                              \
    (void) ((!(expr)) &&                                       \
            [](const char* expr_str, const char* msg_str)      \
            {                                                  \
                FST_ASSERT_HOOK(expr_str, __FILE__, __LINE__); \
                FST_ASSERT_BREAK();                            \
                return false;                                  \
            }(#expr, msg))

#endif // FST_USE_ASSERT_MSG
#else

///
#define fst_assert(...) ((void) 0)

///
#define fst_error(Msg) fst_printf(fst_terminal_color(31) "wrror" fst_terminal_color(0) " : %s\n", (Msg))
#define fst_warning(Msg) fst_printf(fst_terminal_color(33) "warning" fst_terminal_color(0) " : %s\n", (Msg))
#endif
