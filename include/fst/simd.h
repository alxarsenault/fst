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
//
#include "fst/common.h"
#include "fst/math.h"
#include "fst/array.h"

//__SSE__
//__SSE2__
//__SSE3__
//__AVX__
//__AVX2__

#ifdef __AVX2__
// AVX2
//#error dsljh
#elif defined(__AVX__)
// AVX

#elif (defined(_M_AMD64) || defined(_M_X64))
// SSE2 x64

#elif _M_IX86_FP == 2
// SSE2 x32

#elif _M_IX86_FP == 1
// SSE x32

#else
// nothing
#endif

#include "fst/detail/simd/sse.h"

//#include <intrin.h>
//#include <emmintrin.h>
//#include <immintrin.h>
//#include <xmmintrin.h>

//
FST_BEGIN_SUB_NAMESPACE(simd)
    void print_version();

    namespace detail
    {
        template <class _T, size_t _Size, size_t _Alignment, class _DataBase, class _Derived>
        class container_base;

        ///
        template <class _T, size_t _Size, size_t _Alignment = vector_type_alignment_v<_T>>
        class range_base;

        ///
        template <class _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
        class buffer_base;
    } // namespace detail

    ///
    template <class _T, size_t _Size, size_t _Alignment = vector_type_alignment_v<_T>>
    class array;

    ///
    template <class _T, size_t _Size, size_t _Alignment = vector_type_alignment_v<_T>>
    using fixed_range = _FST::simd::detail::range_base<_T, _Size, _Alignment>;

    ///
    template <class _T, size_t _Alignment = vector_type_alignment_v<_T>>
    using range = _FST::simd::detail::range_base<_T, dynamic_size, _Alignment>;

    ///
    template <class _T, size_t _Alignment = vector_type_alignment_v<_T>, class _MemoryZone = _FST::simd_memory_zone, class _MemoryCategory = _FST::dsp_memory_category>
    using buffer = _FST::simd::detail::buffer_base<_T, _FST::dynamic_size, _Alignment, _MemoryZone, _MemoryCategory>;

    template <class _T, size_t _Size, size_t _Alignment = vector_type_alignment_v<_T>, class _MemoryZone = _FST::simd_memory_zone,
        class _MemoryCategory = _FST::dsp_memory_category>
    using fixed_buffer = _FST::simd::detail::buffer_base<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>;

    ///
    template <class T>
    struct is_array : _FST::false_t
    {};

    template <class _T, size_t _Size, size_t _Alignment>
    struct is_array<_FST::simd::array<_T, _Size, _Alignment>> : _FST::true_t
    {};

    ///
    template <class T>
    struct is_fixed_range : _FST::false_t
    {};

    template <class _T, size_t _Size, size_t _Alignment>
    struct is_fixed_range<_FST::simd::detail::range_base<_T, _Size, _Alignment>> : _FST::true_t
    {};

    template <class _T, size_t _Alignment>
    struct is_fixed_range<_FST::simd::detail::range_base<_T, dynamic_size, _Alignment>> : _FST::false_t
    {};

    ///
    template <class T>
    struct is_range : _FST::false_t
    {};

    template <class _T, size_t _Alignment>
    struct is_range<_FST::simd::detail::range_base<_T, dynamic_size, _Alignment>> : _FST::true_t
    {};

    template <class _T, size_t _Size, size_t _Alignment>
    struct is_range<_FST::simd::detail::range_base<_T, _Size, _Alignment>> : _FST::false_t
    {};

    ///
    template <class T>
    struct is_buffer : _FST::false_t
    {};

    template <class _T, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_buffer<_FST::simd::detail::buffer_base<_T, _FST::dynamic_size, _Alignment, _MemoryZone, _MemoryCategory>> : _FST::true_t
    {};

    template <class _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_buffer<_FST::simd::detail::buffer_base<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>> : _FST::false_t
    {};

    template <class T>
    struct is_fixed_buffer : _FST::false_t
    {};

    template <class _T, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_fixed_buffer<_FST::simd::detail::buffer_base<_T, _FST::dynamic_size, _Alignment, _MemoryZone, _MemoryCategory>> : _FST::false_t
    {};

    template <class _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
    struct is_fixed_buffer<_FST::simd::detail::buffer_base<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>> : _FST::true_t
    {};

    template <class T>
    struct has_dynamic_size : _FST::bool_t<_FST::simd::is_buffer<T>::value || _FST::simd::is_range<T>::value>
    {};

    template <class _T, size_t _Size, size_t _Alignment, class _DataBase, class _Derived>
    struct has_dynamic_size<detail::container_base<_T, _Size, _Alignment, _DataBase, _Derived>> : _FST::bool_t<_Size == dynamic_size>
    {};

    template <class T>
    struct has_fixed_size : _FST::bool_t<!_FST::simd::has_dynamic_size<T>::value>
    {};

    template <class _T, size_t _Size, size_t _Alignment, class _DataBase, class _Derived>
    struct has_fixed_size<detail::container_base<_T, _Size, _Alignment, _DataBase, _Derived>> : _FST::bool_t<_Size != dynamic_size>
    {};

    template <class _VectorType, class _T>
    struct vector_op;

    template <class _T>
    struct vector_op<_T, _T>
    {
        using value_type = _T;
        using vec_type = _T;
        static constexpr size_t size = 1;

        FST_ALWAYS_INLINE static vec_type load(const value_type* ptr) noexcept { return *ptr; }
        FST_ALWAYS_INLINE static vec_type load(value_type value) noexcept { return value; }
        FST_ALWAYS_INLINE static vec_type load_zero() noexcept { return 0; }
        FST_ALWAYS_INLINE static void store(value_type* dst, vec_type src) noexcept { *dst = src; }
        FST_ALWAYS_INLINE static vec_type add(vec_type a, vec_type b) noexcept { return a + b; }
        FST_ALWAYS_INLINE static vec_type sub(vec_type a, vec_type b) noexcept { return a - b; }
        FST_ALWAYS_INLINE static vec_type mul(vec_type a, vec_type b) noexcept { return a * b; }
        FST_ALWAYS_INLINE static vec_type div(vec_type a, vec_type b) noexcept { return a / b; }
        FST_ALWAYS_INLINE static vec_type sqrt(vec_type a) noexcept { return _FST::sqrt(a); }
        FST_ALWAYS_INLINE static vec_type invsqrt(vec_type a) noexcept { return value_type(1.0) / _FST::sqrt(a); }
        FST_ALWAYS_INLINE static vec_type sin(vec_type a) noexcept { return _FST::sin(a); }
        FST_ALWAYS_INLINE static vec_type cos(vec_type a) noexcept { return _FST::cos(a); }
        FST_ALWAYS_INLINE static vec_type tan(vec_type a) noexcept { return _FST::tan(a); }
        FST_ALWAYS_INLINE static vec_type log(vec_type a) noexcept { return _FST::log(a); }
        FST_ALWAYS_INLINE static vec_type log10(vec_type a) noexcept { return _FST::log10(a); }
        FST_ALWAYS_INLINE static vec_type log2(vec_type a) noexcept { return _FST::log2(a); }
        FST_ALWAYS_INLINE static vec_type exp10(vec_type a) noexcept { return _FST::pow((value_type) 10, a); }
        FST_ALWAYS_INLINE static vec_type exp2(vec_type a) noexcept { return _FST::exp2(a); }
        FST_ALWAYS_INLINE static vec_type pow(vec_type a, vec_type b) noexcept { return _FST::pow(a, b); }
        FST_ALWAYS_INLINE static vec_type fmod(vec_type a, vec_type b) noexcept { return _FST::fmod(a, b); }
        FST_ALWAYS_INLINE static vec_type min(vec_type a, vec_type b) noexcept { return _FST::minimum(a, b); }
        FST_ALWAYS_INLINE static vec_type max(vec_type a, vec_type b) noexcept { return _FST::maximum(a, b); }
        FST_ALWAYS_INLINE static vec_type hadd(vec_type a, vec_type b) noexcept { return a + b; }
        FST_ALWAYS_INLINE static value_type hsum(vec_type a) noexcept { return a; }
    };

    // clang-format off




    #if FST_SIMD_512
        // float
        #define FST_SIMD_VEC_VECTOR_TYPE __m512
        #define FST_SIMD_VEC_VALUE_TYPE float
        #define FST_SIMD_VEC_OP_PREFIX _mm512_
        #define FST_SIMD_VEC_SUFFIX _ps
        #include "fst/detail/simd/vector_op.h"
        #undef FST_SIMD_VEC_VECTOR_TYPE
        #undef FST_SIMD_VEC_VALUE_TYPE
        #undef FST_SIMD_VEC_OP_PREFIX
        #undef FST_SIMD_VEC_SUFFIX

        // double
        #define FST_SIMD_VEC_VECTOR_TYPE __m512d
        #define FST_SIMD_VEC_VALUE_TYPE double
        #define FST_SIMD_VEC_OP_PREFIX _mm512_
        #define FST_SIMD_VEC_SUFFIX _pd
        #include "fst/detail/simd/vector_op.h"
        #undef FST_SIMD_VEC_VECTOR_TYPE
        #undef FST_SIMD_VEC_VALUE_TYPE
        #undef FST_SIMD_VEC_OP_PREFIX
        #undef FST_SIMD_VEC_SUFFIX

        template <class T> struct vector_512_type;
        template <> struct vector_512_type<float> { using type = __m512; };
        template <> struct vector_512_type<double> { using type = __m512d; };
    #endif // FST_SIMD_512.

    #if FST_SIMD_256
        // float
        #define FST_SIMD_VEC_VECTOR_TYPE __m256
        #define FST_SIMD_VEC_VALUE_TYPE float
        #define FST_SIMD_VEC_OP_PREFIX _mm256_
        #define FST_SIMD_VEC_SUFFIX _ps
        #include "fst/detail/simd/vector_op.h"
        #undef FST_SIMD_VEC_VECTOR_TYPE
        #undef FST_SIMD_VEC_VALUE_TYPE
        #undef FST_SIMD_VEC_OP_PREFIX
        #undef FST_SIMD_VEC_SUFFIX

        // double
        #define FST_SIMD_VEC_VECTOR_TYPE __m256d
        #define FST_SIMD_VEC_VALUE_TYPE double
        #define FST_SIMD_VEC_OP_PREFIX _mm256_
        #define FST_SIMD_VEC_SUFFIX _pd
        #include "fst/detail/simd/vector_op.h"
        #undef FST_SIMD_VEC_VECTOR_TYPE
        #undef FST_SIMD_VEC_VALUE_TYPE
        #undef FST_SIMD_VEC_OP_PREFIX
        #undef FST_SIMD_VEC_SUFFIX

        template <class T> struct vector_256_type;
        template <> struct vector_256_type<float> { using type = __m256; };
        template <> struct vector_256_type<double> { using type = __m256d; };
    #endif // FST_SIMD_256.

    #if FST_SIMD_128
        // float
        #define FST_SIMD_VEC_VECTOR_TYPE __m128
        #define FST_SIMD_VEC_VALUE_TYPE float
        #define FST_SIMD_VEC_OP_PREFIX _mm_
        #define FST_SIMD_VEC_SUFFIX _ps
        #include "fst/detail/simd/vector_op.h"
        #undef FST_SIMD_VEC_VECTOR_TYPE
        #undef FST_SIMD_VEC_VALUE_TYPE
        #undef FST_SIMD_VEC_OP_PREFIX
        #undef FST_SIMD_VEC_SUFFIX

        // double
        #define FST_SIMD_VEC_VECTOR_TYPE __m128d
        #define FST_SIMD_VEC_VALUE_TYPE double
        #define FST_SIMD_VEC_OP_PREFIX _mm_
        #define FST_SIMD_VEC_SUFFIX _pd
        #include "fst/detail/simd/vector_op.h"
        #undef FST_SIMD_VEC_VECTOR_TYPE
        #undef FST_SIMD_VEC_VALUE_TYPE
        #undef FST_SIMD_VEC_OP_PREFIX
        #undef FST_SIMD_VEC_SUFFIX

        template <class T> struct vector_128_type;
        template <> struct vector_128_type<float> { using type = __m128; };
        template <> struct vector_128_type<double> { using type = __m128d; };
    #endif // FST_SIMD_128

    #if FST_SIMD_512
        template <> struct vector_type_base<float> { using type = typename vector_512_type<float>::type; };
        template <> struct vector_type_base<double> { using type = typename vector_512_type<double>::type; };
    #elif FST_SIMD_256
        template <> struct vector_type_base<float> { using type = typename vector_256_type<float>::type; };
        template <> struct vector_type_base<double> { using type = typename vector_256_type<double>::type; };
    #elif FST_SIMD_128
        template <> struct vector_type_base<float> { using type = typename vector_128_type<float>::type; };
        template <> struct vector_type_base<double> { using type = typename vector_128_type<double>::type; };
    #endif
    // clang-format on

#include "fst/detail/simd/container_base.h"

    template <class _T, size_t _Size, size_t _Alignment>
    class array : public _FST::simd::detail::container_base<_T, _Size, _Alignment, _FST::array<_T, _Size, _Alignment>, _FST::simd::array<_T, _Size, _Alignment>>
    {
      public:
        using base = _FST::simd::detail::container_base<_T, _Size, _Alignment, _FST::array<_T, _Size, _Alignment>, _FST::simd::array<_T, _Size, _Alignment>>;

        using base::base;
        using base::alignment;
        using base::data;
        using base::size;

        using value_type = typename base::value_type;
        using size_type = typename base::size_type;
        using difference_type = typename base::difference_type;
        using pointer = typename base::pointer;
        using const_pointer = typename base::const_pointer;
        using reference = typename base::reference;
        using const_reference = typename base::const_reference;
        using iterator = typename base::iterator;
        using const_iterator = typename base::const_iterator;

        // out[i] = data[i] + a
        FST_NODISCARD FST_ALWAYS_INLINE array operator+(value_type a) const noexcept FST_UNIT_TESTED { return array().assign_sum(*this, a); }

        // out[i] = data[i] - a
        FST_NODISCARD FST_ALWAYS_INLINE array operator-(value_type a) const noexcept FST_UNIT_TESTED { return array().assign_sub(*this, a); }

        // out[i] = data[i] * a
        FST_NODISCARD FST_ALWAYS_INLINE array operator*(value_type a) const noexcept FST_UNIT_TESTED { return array().assign_mul(*this, a); }

        // out[i] = data[i] / a
        FST_NODISCARD FST_ALWAYS_INLINE array operator/(value_type a) const noexcept FST_UNIT_TESTED { return array().assign_div(*this, a); }

        // out[i] = data[i] + a[i]
        template <class _ABase, class _AChild, size_t _ASize>
        FST_NODISCARD FST_ALWAYS_INLINE array operator+(
            const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
        {
            return array().assign_sum(*this, a);
        }

        // out[i] = data[i] - a[i]
        template <class _ABase, class _AChild, size_t _ASize>
        FST_NODISCARD FST_ALWAYS_INLINE array operator-(
            const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
        {
            return array().assign_sub(*this, a);
        }

        // out[i] = data[i] * a[i]
        template <class _ABase, class _AChild, size_t _ASize>
        FST_NODISCARD FST_ALWAYS_INLINE array operator*(
            const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
        {
            return array().assign_mul(*this, a);
        }

        // out[i] = data[i] / a[i]
        template <class _ABase, class _AChild, size_t _ASize>
        FST_NODISCARD FST_ALWAYS_INLINE array operator/(
            const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
        {
            return array().assign_div(*this, a);
        }
    };

    namespace detail
    {
        template <class _T, size_t _Size, size_t _Alignment>
        class range_base
            : public _FST::simd::detail::container_base<_T, _Size, _Alignment, _FST::fixed_array_range<_T, _Size, _Alignment>,
                  _FST::simd::detail::range_base<_T, _Size, _Alignment>>
        {
          public:
            using base = _FST::simd::detail::container_base<_T, _Size, _Alignment, _FST::fixed_array_range<_T, _Size, _Alignment>,
                _FST::simd::detail::range_base<_T, _Size, _Alignment>>;

            using base::alignment;
            using base::base;
            using base::data;
            using base::size;

            using value_type = typename base::value_type;
            using size_type = typename base::size_type;
            using difference_type = typename base::difference_type;
            using pointer = typename base::pointer;
            using const_pointer = typename base::const_pointer;
            using reference = typename base::reference;
            using const_reference = typename base::const_reference;
            using iterator = typename base::iterator;
            using const_iterator = typename base::const_iterator;

            using return_array_type = _FST::simd::array<_T, _Size, _Alignment>;

            // out[i] = data[i] + a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator+(value_type a) const noexcept FST_UNIT_TESTED { return return_array_type().assign_sum(*this, a); }

            // out[i] = data[i] - a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator-(value_type a) const noexcept FST_UNIT_TESTED { return return_array_type().assign_sub(*this, a); }

            // out[i] = data[i] * a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator*(value_type a) const noexcept FST_UNIT_TESTED { return return_array_type().assign_mul(*this, a); }

            // out[i] = data[i] / a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator/(value_type a) const noexcept FST_UNIT_TESTED { return return_array_type().assign_div(*this, a); }

            //
            //
            //

            // out[i] = data[i] + a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator+(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type().assign_sum(*this, a);
            }

            // out[i] = data[i] - a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator-(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type().assign_sub(*this, a);
            }

            // out[i] = data[i] * a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator*(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type().assign_mul(*this, a);
            }

            // out[i] = data[i] / a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator/(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type().assign_div(*this, a);
            }
        };

        template <class _T, size_t _Alignment>
        class range_base<_T, _FST::dynamic_size, _Alignment>
            : public _FST::simd::detail::container_base<_T, _FST::dynamic_size, _Alignment, _FST::array_range<_T, _Alignment>,
                  _FST::simd::detail::range_base<_T, _FST::dynamic_size, _Alignment>>
        {
          public:
            using base = _FST::simd::detail::container_base<_T, _FST::dynamic_size, _Alignment, _FST::array_range<_T, _Alignment>,
                _FST::simd::detail::range_base<_T, _FST::dynamic_size, _Alignment>>;

            using base::base;
            using base::alignment;
            using base::data;
            using base::size;

            using value_type = typename base::value_type;
            using size_type = typename base::size_type;
            using difference_type = typename base::difference_type;
            using pointer = typename base::pointer;
            using const_pointer = typename base::const_pointer;
            using reference = typename base::reference;
            using const_reference = typename base::const_reference;
            using iterator = typename base::iterator;
            using const_iterator = typename base::const_iterator;

            using return_array_type = _FST::simd::buffer<_T, _Alignment>;

            range_base() noexcept = default;
            range_base(const range_base&) noexcept = default;
            range_base(range_base&&) noexcept = default;

            range_base& operator=(const range_base&) noexcept = default;
            range_base& operator=(range_base&&) noexcept = default;

            // out[i] = data[i] + a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator+(value_type a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_sum(*this, a);
            }

            // out[i] = data[i] - a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator-(value_type a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_sub(*this, a);
            }

            // out[i] = data[i] * a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator*(value_type a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_mul(*this, a);
            }

            // out[i] = data[i] / a
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator/(value_type a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_div(*this, a);
            }

            //
            //
            //

            // out[i] = data[i] + a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator+(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_sum(*this, a);
            }

            // out[i] = data[i] - a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator-(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_sub(*this, a);
            }

            // out[i] = data[i] * a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator*(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_mul(*this, a);
            }

            // out[i] = data[i] / a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type operator/(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                return return_array_type(size()).assign_div(*this, a);
            }
        };

        template <class _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
        class buffer_base
            : public _FST::simd::detail::container_base<_T, _FST::dynamic_size, _Alignment, _FST::fixed_heap_array<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>,
                  buffer_base<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>>
        {
          public:
            using base = _FST::simd::detail::container_base<_T, _FST::dynamic_size, _Alignment,
                _FST::fixed_heap_array<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>, buffer_base<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>>;

            using base::base;
            using base::alignment;
            using base::data;
            using base::size;

            using value_type = typename base::value_type;
            using size_type = typename base::size_type;
            using difference_type = typename base::difference_type;
            using pointer = typename base::pointer;
            using const_pointer = typename base::const_pointer;
            using reference = typename base::reference;
            using const_reference = typename base::const_reference;
            using iterator = typename base::iterator;
            using const_iterator = typename base::const_iterator;

            buffer_base() noexcept = default;
            buffer_base(const buffer_base&) noexcept = default;
            buffer_base(buffer_base&&) noexcept = default;

            buffer_base& operator=(const buffer_base&) noexcept = default;
            buffer_base& operator=(buffer_base&&) noexcept = default;

            template <size_t _OtherSize>
            using return_array_type = _FST::conditional_t<_OtherSize == _FST::dynamic_size, buffer_base, _FST::simd::array<_T, _OtherSize, _Alignment>>;

            // out[i] = data[i] + a
            FST_NODISCARD FST_ALWAYS_INLINE buffer_base operator+(value_type a) const noexcept FST_UNIT_TESTED { return buffer_base(size()).assign_sum(*this, a); }

            // out[i] = data[i] - a
            FST_NODISCARD FST_ALWAYS_INLINE buffer_base operator-(value_type a) const noexcept FST_UNIT_TESTED { return buffer_base(size()).assign_sub(*this, a); }

            // out[i] = data[i] * a
            FST_NODISCARD FST_ALWAYS_INLINE buffer_base operator*(value_type a) const noexcept FST_UNIT_TESTED { return buffer_base(size()).assign_mul(*this, a); }

            // out[i] = data[i] / a
            FST_NODISCARD FST_ALWAYS_INLINE buffer_base operator/(value_type a) const noexcept FST_UNIT_TESTED { return buffer_base(size()).assign_div(*this, a); }

            ////
            ////
            ////

            // out[i] = data[i] + a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type<_ASize> operator+(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                if constexpr (_ASize == _FST::dynamic_size) { return buffer_base(size()).assign_sum(*this, a); }
                else { return _FST::simd::array<_T, _ASize, _Alignment>().assign_sum(*this, a); }
            }

            // out[i] = data[i] - a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type<_ASize> operator-(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                if constexpr (_ASize == _FST::dynamic_size) { return buffer_base(size()).assign_sub(*this, a); }
                else { return _FST::simd::array<_T, _ASize, _Alignment>().assign_sub(*this, a); }
            }

            // out[i] = data[i] * a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type<_ASize> operator*(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                if constexpr (_ASize == _FST::dynamic_size) { return buffer_base(size()).assign_mul(*this, a); }
                else { return _FST::simd::array<_T, _ASize, _Alignment>().assign_mul(*this, a); }
            }

            // out[i] = data[i] / a[i]
            template <class _ABase, class _AChild, size_t _ASize>
            FST_NODISCARD FST_ALWAYS_INLINE return_array_type<_ASize> operator/(
                const _FST::simd::detail::container_base<_T, _ASize, _Alignment, _ABase, _AChild>& a) const noexcept FST_UNIT_TESTED
            {
                if constexpr (_ASize == _FST::dynamic_size) { return buffer_base(size()).assign_div(*this, a); }
                else { return _FST::simd::array<_T, _ASize, _Alignment>().assign_div(*this, a); }
            }
        };
    } // namespace detail.
FST_END_SUB_NAMESPACE
