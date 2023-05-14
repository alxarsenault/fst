///
///
///

#include <float.h>
#include <limits.h>
#include <wchar.h>

FST_BEGIN_NAMESPACE

    template <class>
    struct numeric_limits;

#define FST_DECLARE_NUM_TYPE(TYPE, VALUE_MIN, VALUE_MAX, ESP)  \
    template <>                                                \
    struct numeric_limits<TYPE>                                \
    {                                                          \
        FST_NODISCARD static constexpr TYPE(min)() noexcept    \
        {                                                      \
            return VALUE_MIN;                                  \
        }                                                      \
        FST_NODISCARD static constexpr TYPE(max)() noexcept    \
        {                                                      \
            return VALUE_MAX;                                  \
        }                                                      \
        FST_NODISCARD static constexpr TYPE epsilon() noexcept \
        {                                                      \
            return ESP;                                        \
        }                                                      \
    }

    FST_DECLARE_NUM_TYPE(bool, false, true, false);
    FST_DECLARE_NUM_TYPE(char, CHAR_MIN, CHAR_MAX, (char) 0);
    FST_DECLARE_NUM_TYPE(signed char, SCHAR_MIN, SCHAR_MAX, 0);
    FST_DECLARE_NUM_TYPE(unsigned char, 0, UCHAR_MAX, 0);
    FST_DECLARE_NUM_TYPE(char16_t, 0, USHRT_MAX, 0);
    FST_DECLARE_NUM_TYPE(char32_t, 0, UINT_MAX, 0);
    FST_DECLARE_NUM_TYPE(wchar_t, WCHAR_MIN, WCHAR_MAX, 0);
    FST_DECLARE_NUM_TYPE(short, SHRT_MIN, SHRT_MAX, 0);
    FST_DECLARE_NUM_TYPE(int, INT_MIN, INT_MAX, 0);
    FST_DECLARE_NUM_TYPE(long, LONG_MIN, LONG_MAX, 0);
    FST_DECLARE_NUM_TYPE(long long, LLONG_MIN, LLONG_MAX, 0);
    FST_DECLARE_NUM_TYPE(unsigned short, 0, USHRT_MAX, 0);
    FST_DECLARE_NUM_TYPE(unsigned int, 0, UINT_MAX, 0);
    FST_DECLARE_NUM_TYPE(unsigned long, 0, ULONG_MAX, 0);
    FST_DECLARE_NUM_TYPE(unsigned long long, 0, ULLONG_MAX, 0);
    FST_DECLARE_NUM_TYPE(float, FLT_MIN, FLT_MAX, FLT_EPSILON);
    FST_DECLARE_NUM_TYPE(double, DBL_MIN, DBL_MAX, DBL_EPSILON);
    FST_DECLARE_NUM_TYPE(long double, (long double) LDBL_MIN, (long double) LDBL_MAX, (long double) LDBL_EPSILON);

#if FST_HAS_CHAR8_T
    FST_DECLARE_NUM_TYPE(char8_t, 0, UCHAR_MAX, 0);
#endif // FST_HAS_CHAR8_T

    template <class _T>
    struct numeric_limits<const _T> : __fst::numeric_limits<_T>
    {};

    template <class _T>
    struct numeric_limits<volatile _T> : __fst::numeric_limits<_T>
    {};

    template <class _T>
    struct numeric_limits<const volatile _T> : __fst::numeric_limits<_T>
    {};

FST_END_NAMESPACE
