
#undef __FST_ARCH_ARM__
#undef __FST_ARCH_INTEL__

#undef __FST_ARCH_X86_32__
#undef __FST_ARCH_X86_64__
#undef __FST_ARCH_ARM_32__
#undef __FST_ARCH_ARM_64__

#if defined(__arm64) || defined(__arm64__) || defined(_M_ARM64)
#define __FST_ARCH_ARM__ 1
#define __FST_ARCH_INTEL__ 0

#define __FST_ARCH_X86_32__ 0
#define __FST_ARCH_X86_64__ 0
#define __FST_ARCH_ARM_32__ 0
#define __FST_ARCH_ARM_64__ 1

#elif defined(__arm__) || defined(_M_ARM)
#define __FST_ARCH_ARM__ 1
#define __FST_ARCH_INTEL__ 0

#define __FST_ARCH_X86_32__ 0
#define __FST_ARCH_X86_64__ 0
#define __FST_ARCH_ARM_32__ 1
#define __FST_ARCH_ARM_64__ 0

#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) || defined(__amd64) || defined(_M_X64)
#define __FST_ARCH_ARM__ 0
#define __FST_ARCH_INTEL__ 1

#define __FST_ARCH_X86_32__ 0
#define __FST_ARCH_X86_64__ 1
#define __FST_ARCH_ARM_32__ 0
#define __FST_ARCH_ARM_64__ 0

#elif defined(i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__i386) || defined(_M_IX86) || defined(_X86_) \
    || defined(__I86__) || defined(__INTEL__)
#define __FST_ARCH_ARM__ 0
#define __FST_ARCH_INTEL__ 1

#define __FST_ARCH_X86_32__ 1
#define __FST_ARCH_X86_64__ 0
#define __FST_ARCH_ARM_32__ 0
#define __FST_ARCH_ARM_64__ 0

#else
#define __FST_ARCH_ARM__ 0
#define __FST_ARCH_INTEL__ 0

#define __FST_ARCH_X86_32__ 0
#define __FST_ARCH_X86_64__ 0
#define __FST_ARCH_ARM_32__ 0
#define __FST_ARCH_ARM_64__ 0
#endif
