///
///
///

#undef __FST_ANDROID__
#undef __FST_BSD__
#undef __FST_IOS__
#undef __FST_LINUX__
#undef __FST_MACOS__
#undef __FST_SOLARIS__
#undef __FST_WINDOWS__

// Android.
#if defined(__ANDROID__)
#define __FST_ANDROID__ 1

// Linux.
#elif defined(__linux__) || defined(__linux) || defined(linux)
#define __FST_LINUX__ 1

// Apple macos or ios.
#elif defined(__APPLE__)
// Apple.
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
// ios.
#define __FST_IOS__ 1

#elif TARGET_OS_MAC
// Mac OS.
#define __FST_MACOS__ 1
#else
#error "fst unsupported platform."
#endif

// BSD.
#elif defined(BSD) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#define __FST_BSD__ 1

// Solaris.
#elif defined(__sun) && defined(__SVR4)
#define __FST_SOLARIS__ 1

// Windows.
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
#define __FST_WINDOWS__ 1

// Unknown platform.
#else
#error "fst unsupported platform."
#endif

///
///
///
#ifndef __FST_ANDROID__
#define __FST_ANDROID__ 0
#endif

#ifndef __FST_BSD__
#define __FST_BSD__ 0
#endif

#ifndef __FST_IOS__
#define __FST_IOS__ 0
#endif

#ifndef __FST_LINUX__
#define __FST_LINUX__ 0
#endif

#ifndef __FST_MACOS__
#define __FST_MACOS__ 0
#endif

#ifndef __FST_SOLARIS__
#define __FST_SOLARIS__ 0
#endif

#ifndef __FST_WINDOWS__
#define __FST_WINDOWS__ 0
#endif
