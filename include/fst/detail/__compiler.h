///
///
///

#undef __FST_CLANG__
#undef __FST_GCC__
#undef __FST_INTEL__
#undef __FST_MINGW__
#undef __FST_MSVC__
#undef __FST_WASM__

// Clang.
#if defined(__clang__)
#define __FST_CLANG__ 1

// GCC.
#elif defined(__GNUC__) || defined(__GNUG__)
#define __FST_GCC__ 1

// Intel.
#elif (defined(SYCL_LANGUAGE_VERSION) && defined(__INTEL_LLVM_COMPILER)) || defined(__INTEL_COMPILER)
#define __FST_INTEL__ 1

// MinGW.
#elif defined(__MINGW32__) || defined(__MINGW64__)
#define __FST_MINGW__ 1

// Microsoft visual studio.
#elif defined(_MSC_VER)
#define __FST_MSVC__ 1

// Web assembly.
#elif defined(__EMSCRIPTEN__)
#define __FST_WASM__ 1

// Unknown compiler.
#else
#error "fst unsupported compiler."
#endif

#ifndef __FST_CLANG__
#define __FST_CLANG__ 0
#endif
#ifndef __FST_GCC__
#define __FST_GCC__ 0
#endif
#ifndef __FST_INTEL__
#define __FST_INTEL__ 0
#endif
#ifndef __FST_MINGW__
#define __FST_MINGW__ 0
#endif
#ifndef __FST_MSVC__
#define __FST_MSVC__ 0
#endif
#ifndef __FST_WASM__
#define __FST_WASM__ 0
#endif

#undef __FST_CPP_20__
#undef __FST_CPP_17__
#undef __FST_CPP_14__

#undef __FST_CPP_VERSION__

// C++ 20.
#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#define __FST_CPP_VERSION__ 20
#define __FST_CPP_20__ 1
#define __FST_CPP_17__ 1
#define __FST_CPP_14__ 1

// C++ 17.
#elif __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define __FST_CPP_VERSION__ 17
#define __FST_CPP_20__ 0
#define __FST_CPP_17__ 1
#define __FST_CPP_14__ 1

#else
#define __FST_CPP_VERSION__ 11
#define __FST_CPP_20__ 0
#define __FST_CPP_17__ 0
#define __FST_CPP_14__ 1
#error "fst only support c++17 and up."
#endif

#if __FST_MSVC__
#define FST_PRAGMA_(x) __pragma(x)
#define FST_PRAGMA(x) FST_PRAGMA_(x)
#define FST_PRAGMA_PUSH() __pragma(warning(push))
#define FST_PRAGMA_POP() __pragma(warning(pop))
#define FST_PRAGMA_DIAGNOSTIC(TYPE, VALUE) FST_PRAGMA(warning(TYPE : VALUE))
#define FST_PRAGMA_DISABLE_WARNING_MSVC(VALUE) FST_PRAGMA_DIAGNOSTIC(disable, VALUE)
#define FST_DISABLE_ALL_WARNINGS_BEGIN __pragma(warning(push, 0))
#define FST_DISABLE_ALL_WARNINGS_END __pragma(warning(pop))

#define FST_MESSAGE(msg) FST_PRAGMA(message(msg))

#elif __FST_CLANG__
#define FST_PRAGMA_(x) _Pragma(#x)
#define FST_PRAGMA(x) FST_PRAGMA_(x)
#define FST_PRAGMA_PUSH() FST_PRAGMA(clang diagnostic push)
#define FST_PRAGMA_POP() FST_PRAGMA(clang diagnostic pop)
#define FST_PRAGMA_DIAGNOSTIC(TYPE, VALUE) FST_PRAGMA(clang diagnostic TYPE VALUE)
#define FST_PRAGMA_DISABLE_WARNING_CLANG(VALUE) FST_PRAGMA_DIAGNOSTIC(ignored, VALUE)
#define FST_DISABLE_ALL_WARNINGS_BEGIN FST_PRAGMA_PUSH() FST_PRAGMA_DISABLE_WARNING_CLANG("-Weverything")
#define FST_DISABLE_ALL_WARNINGS_END FST_PRAGMA_POP()
#define FST_MESSAGE(desc)

#elif __FST_GCC__
#define FST_PRAGMA_(x) _Pragma(#x)
#define FST_PRAGMA(x) FST_PRAGMA_(x)
#define FST_PRAGMA_PUSH() FST_PRAGMA(GCC diagnostic push)
#define FST_PRAGMA_POP() FST_PRAGMA(GCC diagnostic pop)
#define FST_PRAGMA_DIAGNOSTIC(TYPE, VALUE) FST_PRAGMA(GCC diagnostic TYPE VALUE)
#define FST_PRAGMA_DISABLE_WARNING_GCC(VALUE) FST_PRAGMA_DIAGNOSTIC(ignored, VALUE)
#define FST_DISABLE_ALL_WARNINGS_BEGIN \
    FST_PRAGMA_PUSH() FST_PRAGMA_DISABLE_WARNING_GCC("-Wall") FST_PRAGMA_DISABLE_WARNING_GCC("-Wextra") FST_PRAGMA_DISABLE_WARNING_GCC("-Wunused-value")
#define FST_DISABLE_ALL_WARNINGS_END FST_PRAGMA_POP()
#define FST_MESSAGE(desc)

#else
#define FST_PRAGMA_(x)
#define FST_PRAGMA(x)
#define FST_PRAGMA_PUSH()
#define FST_PRAGMA_POP()
#define FST_PRAGMA_DIAGNOSTIC(TYPE, VALUE)
#define FST_DISABLE_ALL_WARNINGS_BEGIN
#define FST_DISABLE_ALL_WARNINGS_END
#define FST_MESSAGE(desc)
#endif //

#ifndef FST_PRAGMA_DISABLE_WARNING_MSVC
#define FST_PRAGMA_DISABLE_WARNING_MSVC(X)
#endif

#ifndef FST_PRAGMA_DISABLE_WARNING_CLANG
#define FST_PRAGMA_DISABLE_WARNING_CLANG(X)
#endif

#ifndef FST_PRAGMA_DISABLE_WARNING_GCC
#define FST_PRAGMA_DISABLE_WARNING_GCC(X)
#endif

//
#define FST_TODO(desc)                    \
    FST_MESSAGE("fst - [TODO] : "__FILE__ \
                "(" FST_STRINGIFY(__LINE__) ") : " desc)
