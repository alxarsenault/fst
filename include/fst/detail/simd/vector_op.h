///
/// Not a normal header.
///
/// Needs to be included with these defines
/// * FST_SIMD_VEC_VECTOR_TYPE
/// * FST_SIMD_VEC_VALUE_TYPE
/// * FST_SIMD_VEC_OP_PREFIX
/// * FST_SIMD_VEC_SUFFIX
///
/// @code
///   #define FST_SIMD_VEC_VECTOR_TYPE __m128
///   #define FST_SIMD_VEC_VALUE_TYPE float
///   #define FST_SIMD_VEC_OP_PREFIX _mm_
///   #define FST_SIMD_VEC_SUFFIX _ps
///   #include "fst/simd//detail/vector_op.h"
///   #undef FST_SIMD_VEC_VECTOR_TYPE
///   #undef FST_SIMD_VEC_VALUE_TYPE
///   #undef FST_SIMD_VEC_OP_PREFIX
///   #undef FST_SIMD_VEC_SUFFIX
/// @endcode

template <>
struct vector_op<FST_SIMD_VEC_VECTOR_TYPE, FST_SIMD_VEC_VALUE_TYPE>
{
    using value_type = FST_SIMD_VEC_VALUE_TYPE;
    using vec_type = FST_SIMD_VEC_VECTOR_TYPE;
    static constexpr size_t size = sizeof(vec_type) / sizeof(value_type);

#define FST_SIMD_VEC_OP(OP) FST_CONCAT(FST_CONCAT(FST_SIMD_VEC_OP_PREFIX, OP), FST_SIMD_VEC_SUFFIX)

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type load(const value_type* ptr) noexcept { return FST_SIMD_VEC_OP(load)(ptr); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type load(value_type value) noexcept { return FST_SIMD_VEC_OP(set1)(value); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type load_zero() noexcept { return FST_SIMD_VEC_OP(setzero)(); }

    FST_ALWAYS_INLINE static void store(value_type* dst, vec_type src) noexcept { FST_SIMD_VEC_OP(store)(dst, src); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type add(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(add)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type sub(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(sub)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type mul(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(mul)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type div(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(div)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type sqrt(vec_type a) noexcept { return FST_SIMD_VEC_OP(sqrt)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type invsqrt(vec_type a) noexcept { return FST_SIMD_VEC_OP(invsqrt)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type sin(vec_type a) noexcept { return FST_SIMD_VEC_OP(sin)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type cos(vec_type a) noexcept { return FST_SIMD_VEC_OP(cos)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type tan(vec_type a) noexcept { return FST_SIMD_VEC_OP(tan)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type log(vec_type a) noexcept { return FST_SIMD_VEC_OP(log)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type log10(vec_type a) noexcept { return FST_SIMD_VEC_OP(log10)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type log2(vec_type a) noexcept { return FST_SIMD_VEC_OP(log2)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type exp10(vec_type a) noexcept { return FST_SIMD_VEC_OP(exp10)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type exp2(vec_type a) noexcept { return FST_SIMD_VEC_OP(exp2)(a); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type pow(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(pow)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type fmod(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(fmod)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type min(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(min)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type max(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(max)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static vec_type hadd(vec_type a, vec_type b) noexcept { return FST_SIMD_VEC_OP(hadd)(a, b); }

    FST_NODISCARD FST_ALWAYS_INLINE static value_type hsum(vec_type a) noexcept
    {
        value_type out = 0;
        for (size_t i = 0; i < size; i++)
        {
            out += ((const value_type*) &a)[i];
        }
        return out;
    }
};
