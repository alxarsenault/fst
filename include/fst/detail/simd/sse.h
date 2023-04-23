#pragma once

#include "fst/common.h"
#include "fst/array.h"
#include "fst/math.h"

#if FST_SIMD_128 || FST_SIMD_256 || FST_SIMD_512
#define FST_HAS_SIMD 1

#include <intrin.h>
#include <immintrin.h>

#if FST_PLATFORM_HAS_EMMINTRIN_H
#include <emmintrin.h>
#endif

#if FST_PLATFORM_HAS_XMMINTRIN_H
#include <xmmintrin.h>
#endif
//https://github.com/raedwulf/gmath/tree/master/include/cephes
FST_BEGIN_SUB_NAMESPACE(simd)

    template <class T>
    inline constexpr int to_int(T v) noexcept
    {
        return (int) v;
    }

    template <class T>
    inline constexpr short to_short(T v) noexcept
    {
        return (short) v;
    }

    //typedef struct __declspec(intrin_type) __declspec(align(16)) __m128d {
    //    double              m128d_f64[2];
    //} __m128d;

    union m128
    {
        __m128 v; // SSE 4 x float vector
        float a[4]; // scalar array of 4 floats

        inline m128(__m128 vv) noexcept
            : v(vv)
        {}

        inline float& operator[](size_t index) noexcept { return a[index]; }
        inline float operator[](size_t index) const noexcept { return a[index]; }
    };

    union m128d
    {
        __m128d v; // SSE 4 x float vector
        double a[2]; // scalar array of 4 floats

        inline m128d(__m128d vv) noexcept
            : v(vv)
        {}

        inline double& operator[](size_t index) noexcept { return a[index]; }
        inline double operator[](size_t index) const noexcept { return a[index]; }
    };

    static const _FST::array<int, 4, 16> _mm_cst_sign_mask_ps = { to_int(0x80000000), to_int(0x80000000), to_int(0x80000000), to_int(0x80000000) };
    static const _FST::array<int, 4, 16> _mm_cst_inv_sign_mask_ps = { to_int(~0x80000000), to_int(~0x80000000), to_int(~0x80000000), to_int(~0x80000000) };
    static const _FST::array<int, 4, 16> _mm_cst_sign_mask_pd = { 0, to_int(0x80000000), 0, to_int(0x80000000) };
    static const _FST::array<int, 4, 16> _mm_cst_inv_sign_mask_pd = { to_int(~0), to_int(~0x80000000), to_int(~0), to_int(~0x80000000) };

    // static const _FST::array<int, 4, 16> _mm_cst_one = { 1, 1, 1, 1 };
    // static const _FST::array<int, 4, 16> _mm_cst_inv1 = { ~1, ~1, ~1, ~1 };
    // static const _FST::array<int, 4, 16> _mm_cst_two = { 2, 2, 2, 2 };
    // static const _FST::array<int, 4, 16> _mm_cst_four = { 4, 4, 4, 4 };

    static const __m128 _mm_cst_ps_one = _mm_set_ps1(1.0f);
    static const __m128 _mm_cst_ps_mone = _mm_set_ps1(-1.0f);
    static const __m128 _mm_cst_ps_mtwo = _mm_set_ps1(-2.0f);
    static const __m128 _mm_cst_ps_0p5 = _mm_set_ps1(0.5f);

    static const __m128d _mm_cst_pd_one = _mm_set1_pd(1.0);
    static const __m128d _mm_cst_pd_mone = _mm_set1_pd(-1.0);
    static const __m128d _mm_cst_pd_mtwo = _mm_set1_pd(-2.0);
    static const __m128d _mm_cst_pd_0p5 = _mm_set1_pd(0.5);

    static const __m128 _mm_cst_ps_tan3pio8 = _mm_set1_ps(2.414213562373095f);
    static const __m128 _mm_cst_ps_tanpio8 = _mm_set1_ps(0.4142135623730950f);

    static const __m128d _mm_cst_pd_tan3pio8 = _mm_set1_pd(2.414213562373095);
    static const __m128d _mm_cst_pd_tanpio8 = _mm_set1_pd(0.66);

    static const __m128 _mm_cst_ps_fopi = _mm_set1_ps(1.27323954473516f);

    static const __m128 _mm_cst_ps_pi = _mm_set1_ps(3.14159265358979f);
    static const __m128 _mm_cst_ps_mpi = _mm_set1_ps(-3.14159265358979f);
    static const __m128 _mm_cst_ps_pio2 = _mm_set1_ps(1.5707963267948966f);
    static const __m128 _mm_cst_ps_mpio2 = _mm_set1_ps(-1.5707963267948966f);
    static const __m128 _mm_cst_ps_pio4 = _mm_set1_ps(0.7853981633974483f);

    static const __m128d _mm_cst_pd_pi = _mm_set1_pd(_FST::pi<double>);
    static const __m128d _mm_cst_pd_mpi = _mm_set1_pd(-_FST::pi<double>);
    static const __m128d _mm_cst_pd_pio2 = _mm_set1_pd(_FST::pi_over_two<double>);
    static const __m128d _mm_cst_pd_mpio2 = _mm_set1_pd(-_FST::pi_over_two<double>);
    static const __m128d _mm_cst_pd_pio4 = _mm_set1_pd(_FST::pi_over_four<double>);

    static const __m128 _mm_cst_ps_DP1 = _mm_set1_ps(0.78515625f);
    static const __m128 _mm_cst_ps_DP2 = _mm_set1_ps(2.4187564849853515625e-4f);
    static const __m128 _mm_cst_ps_DP3 = _mm_set1_ps(3.77489497744594108e-8f);

    static const __m128 _mm_cst_ps_sincof_p0 = _mm_set1_ps(-1.9515295891e-4f);
    static const __m128 _mm_cst_ps_sincof_p1 = _mm_set1_ps(8.3321608736e-3f);
    static const __m128 _mm_cst_ps_sincof_p2 = _mm_set1_ps(-1.6666654611e-1f);

    static const __m128 _mm_cst_ps_coscof_p0 = _mm_set1_ps(2.443315711809948e-5f);
    static const __m128 _mm_cst_ps_coscof_p1 = _mm_set1_ps(-1.388731625493765e-3f);
    static const __m128 _mm_cst_ps_coscof_p2 = _mm_set1_ps(4.166664568298827e-2f);

    FST_INLINE_VAR constexpr double asincof_s0 = -21.947795316429207;
    FST_INLINE_VAR constexpr double asincof_s1 = 147.06563540268149;
    FST_INLINE_VAR constexpr double asincof_s2 = -383.87709576036912;
    FST_INLINE_VAR constexpr double asincof_s3 = 342.43986579130785;
    FST_INLINE_VAR constexpr double asincof_r0 = 0.002967721961301243;
    FST_INLINE_VAR constexpr double asincof_r1 = -0.56342427800089634;
    FST_INLINE_VAR constexpr double asincof_r2 = 6.9687108241047131;
    FST_INLINE_VAR constexpr double asincof_r3 = -25.56901049652825;
    FST_INLINE_VAR constexpr double asincof_r4 = 28.536655482610616;
    FST_INLINE_VAR constexpr double asincof_q0 = -14.740913729888538;
    FST_INLINE_VAR constexpr double asincof_q1 = 70.496102808568423;
    FST_INLINE_VAR constexpr double asincof_q2 = -147.17912922327261;
    FST_INLINE_VAR constexpr double asincof_q3 = 139.51056146574857;
    FST_INLINE_VAR constexpr double asincof_q4 = -49.18853881490881;
    FST_INLINE_VAR constexpr double asincof_p0 = 0.0042530113690044278;
    FST_INLINE_VAR constexpr double asincof_p1 = -0.60195980080141243;
    FST_INLINE_VAR constexpr double asincof_p2 = 5.4446223905647111;
    FST_INLINE_VAR constexpr double asincof_p3 = -16.262479672107002;
    FST_INLINE_VAR constexpr double asincof_p4 = 19.562619833175948;
    FST_INLINE_VAR constexpr double asincof_p5 = -8.1980898024848248;
    FST_INLINE_VAR constexpr double atancof_q0 = 24.858464901423062;
    FST_INLINE_VAR constexpr double atancof_q1 = 165.02700983169885;
    FST_INLINE_VAR constexpr double atancof_q2 = 432.88106049129027;
    FST_INLINE_VAR constexpr double atancof_q3 = 485.3903996359137;
    FST_INLINE_VAR constexpr double atancof_q4 = 194.55065714826139;
    FST_INLINE_VAR constexpr double atancof_p0 = -0.87506086000319039;
    FST_INLINE_VAR constexpr double atancof_p1 = -16.157537187333652;
    FST_INLINE_VAR constexpr double atancof_p2 = -75.008557923147052;
    FST_INLINE_VAR constexpr double atancof_p3 = -122.88666844901361;
    FST_INLINE_VAR constexpr double atancof_p4 = -64.850219049420247;

    /*
 * Range reduction is into intervals of pi/4.  The reduction error is nearly
 * eliminated by contriving an extended precision modular arithmetic.
 *
 * Two polynomial approximating functions are employed.
 * Between 0 and pi/4 the sine is approximated by
 *     x  +  x**3 P(x**2).
 * Between pi/4 and pi/2 the cosine is represented as
 *     1  -  x**2 Q(x**2).
 *
 * Partial loss of accuracy begins to occur at x = 2**30 = 1.074e9. The loss
 * is not gradual, but jumps suddenly to about 1 part in 10e7. Results may be
 * meaningless for x > 2**49 = 5.6e14.  The routine as implemented flags a
 * TLOSS error for x > 2**30 and returns 0.0.
 *
 */
#if !FST_PLATFORM_HAS_MM_SIN_PS
    inline __m128 _mm_sin_ps(__m128 x)
    {
        // Extract the sign bit and work with absolute values
        __m128 signbit = _mm_and_ps(x, *(__m128*) _mm_cst_sign_mask_ps.data());
        x = _mm_and_ps(x, *(__m128*) _mm_cst_inv_sign_mask_ps.data());

        // Cephes method for scaling between 0 et pi/4
        __m128 y = _mm_mul_ps(x, _mm_cst_ps_fopi);
        __m128i yf = _mm_cvttps_epi32(y); // floor
        // see j = (j+1) & (~1) in Cephes
        yf = _mm_add_epi32(yf, _mm_set1_epi32(1));
        yf = _mm_and_si128(yf, _mm_set1_epi32(~1));
        y = _mm_cvtepi32_ps(yf);

        // Swap sign flag
        __m128i flag = _mm_and_si128(yf, _mm_set1_epi32(4));
        flag = _mm_slli_epi32(flag, 29); // flag << 29

        // Polynom selection mask
        yf = _mm_and_si128(yf, _mm_set1_epi32(2));
        yf = _mm_cmpeq_epi32(yf, _mm_setzero_si128());

        __m128 swapsign = _mm_castsi128_ps(flag);
        __m128 polymask = _mm_castsi128_ps(yf);
        signbit = _mm_xor_ps(signbit, swapsign);

        // Extended precision modular arithmetic (Cody and Waite)
        // z = ((x - y * DP1) - y * DP2) - y * DP3
        __m128 xmm1 = _mm_mul_ps(y, _mm_cst_ps_DP1);
        __m128 xmm2 = _mm_mul_ps(y, _mm_cst_ps_DP2);
        __m128 xmm3 = _mm_mul_ps(y, _mm_cst_ps_DP3);
        x = _mm_sub_ps(x, xmm1);
        x = _mm_sub_ps(x, xmm2);
        x = _mm_sub_ps(x, xmm3);

        // First polynom x \in [0, pi/4]
        __m128 z = _mm_mul_ps(x, x);

        y = _mm_mul_ps(_mm_cst_ps_coscof_p0, z);
        y = _mm_add_ps(y, _mm_cst_ps_coscof_p1);
        y = _mm_mul_ps(y, z);
        y = _mm_add_ps(y, _mm_cst_ps_coscof_p2);
        y = _mm_mul_ps(y, z);
        y = _mm_mul_ps(y, z);
        __m128 tmp = _mm_mul_ps(z, _mm_cst_ps_0p5);
        y = _mm_sub_ps(y, tmp);
        y = _mm_add_ps(y, _mm_cst_ps_one);

        // Second polynom
        __m128 y2 = _mm_mul_ps(_mm_cst_ps_sincof_p0, z);
        y2 = _mm_add_ps(y2, _mm_cst_ps_sincof_p1);
        y2 = _mm_mul_ps(y2, z);
        y2 = _mm_add_ps(y2, _mm_cst_ps_sincof_p2);
        y2 = _mm_mul_ps(y2, z);
        y2 = _mm_mul_ps(y2, x);
        y2 = _mm_add_ps(y2, x);

        // Select the correct result
        y2 = _mm_and_ps(polymask, y2);
        y = _mm_andnot_ps(polymask, y);
        y = _mm_add_ps(y, y2);

        // Update the sign
        y = _mm_xor_ps(y, signbit);

        return y;
    }
#endif
//

/*
 * Range reduction is into intervals of pi/4.  The reduction
 * error is nearly eliminated by contriving an extended precision
 * modular arithmetic.
 *
 * Two polynomial approximating functions are employed.
 * Between 0 and pi/4 the cosine is approximated by
 *      1  -  x**2 Q(x**2).
 * Between pi/4 and pi/2 the sine is represented as
 *      x  +  x**3 P(x**2).
 */
#if !FST_PLATFORM_HAS_MM_COS_PS
    inline __m128 _mm_cos_ps(__m128 x)
    {
        // Extract the absolute values
        x = _mm_and_ps(x, *(__m128*) _mm_cst_inv_sign_mask_ps.data());

        // Cephes method for scaling between 0 et pi/4
        __m128 y = _mm_mul_ps(x, _mm_cst_ps_fopi);
        __m128i yf = _mm_cvttps_epi32(y); // floor
        // see j = (j+1) & (~1) in Cephes
        yf = _mm_add_epi32(yf, _mm_set1_epi32(1));
        yf = _mm_and_si128(yf, _mm_set1_epi32(~1));
        y = _mm_cvtepi32_ps(yf);

        yf = _mm_sub_epi32(yf, _mm_set1_epi32(2));

        // Swap sign flag
        __m128i flag = _mm_andnot_si128(yf, _mm_set1_epi32(4));
        flag = _mm_slli_epi32(flag, 29); // flag << 29

        // Polynom selection mask
        yf = _mm_and_si128(yf, _mm_set1_epi32(2));
        yf = _mm_cmpeq_epi32(yf, _mm_setzero_si128());

        __m128 signbit = _mm_castsi128_ps(flag);
        __m128 polymask = _mm_castsi128_ps(yf);

        // Extended precision modular arithmetic (Cody and Waite)
        // z = ((x - y * DP1) - y * DP2) - y * DP3
        __m128 xmm1 = _mm_mul_ps(y, _mm_cst_ps_DP1);
        __m128 xmm2 = _mm_mul_ps(y, _mm_cst_ps_DP2);
        __m128 xmm3 = _mm_mul_ps(y, _mm_cst_ps_DP3);
        x = _mm_sub_ps(x, xmm1);
        x = _mm_sub_ps(x, xmm2);
        x = _mm_sub_ps(x, xmm3);

        // First polynom x \in [0, pi/4]
        __m128 z = _mm_mul_ps(x, x);

        y = _mm_mul_ps(_mm_cst_ps_coscof_p0, z);
        y = _mm_add_ps(y, _mm_cst_ps_coscof_p1);
        y = _mm_mul_ps(y, z);
        y = _mm_add_ps(y, _mm_cst_ps_coscof_p2);
        y = _mm_mul_ps(y, z);
        y = _mm_mul_ps(y, z);
        __m128 tmp = _mm_mul_ps(z, _mm_cst_ps_0p5);
        y = _mm_sub_ps(y, tmp);
        y = _mm_add_ps(y, _mm_cst_ps_one);

        // Second polynom
        __m128 y2 = _mm_mul_ps(_mm_cst_ps_sincof_p0, z);
        y2 = _mm_add_ps(y2, _mm_cst_ps_sincof_p1);
        y2 = _mm_mul_ps(y2, z);
        y2 = _mm_add_ps(y2, _mm_cst_ps_sincof_p2);
        y2 = _mm_mul_ps(y2, z);
        y2 = _mm_mul_ps(y2, x);
        y2 = _mm_add_ps(y2, x);

        // Select the correct result
        y2 = _mm_and_ps(polymask, y2);
        y = _mm_andnot_ps(polymask, y);
        y = _mm_add_ps(y, y2);

        // Update the sign
        y = _mm_xor_ps(y, signbit);

        return y;
    }
#endif
//

/*
 * Sine and cosine computations are very similar.
 * Both functions can be merged for computing sine and cosine at minimum
 * computation cost
 */
#if !FST_PLATFORM_HAS_MM_SINCOS_PS
    inline __m128 _mm_sincos_ps(__m128 * c, __m128 x)
    {
        // Extract the sign bit and work with absolute values
        __m128 sin_signbit = _mm_and_ps(x, *(__m128*) _mm_cst_sign_mask_ps.data());
        x = _mm_and_ps(x, *(__m128*) _mm_cst_inv_sign_mask_ps.data());

        // Cephes method for scaling between 0 et pi/4
        __m128 y = _mm_mul_ps(x, _mm_cst_ps_fopi);
        __m128i yf = _mm_cvttps_epi32(y); // floor
        // see j = (j+1) & (~1) in Cephes
        yf = _mm_add_epi32(yf, _mm_set1_epi32(1));
        yf = _mm_and_si128(yf, _mm_set1_epi32(~1));
        y = _mm_cvtepi32_ps(yf);

        // Fork yf to yf_cos for cosine
        __m128i yf_cos = _mm_sub_epi32(yf, _mm_set1_epi32(2));
        yf_cos = _mm_andnot_si128(yf_cos, _mm_set1_epi32(4));
        yf_cos = _mm_slli_epi32(yf_cos, 29); // flag << 29
        __m128 cos_signbit = _mm_castsi128_ps(yf_cos);

        // Back to sine computation: swap sign flag
        __m128i flag = _mm_and_si128(yf, _mm_set1_epi32(4));
        flag = _mm_slli_epi32(flag, 29); // flag << 29

        // Polynom selection mask
        yf = _mm_and_si128(yf, _mm_set1_epi32(2));
        yf = _mm_cmpeq_epi32(yf, _mm_setzero_si128());

        __m128 swapsign = _mm_castsi128_ps(flag);
        __m128 polymask = _mm_castsi128_ps(yf);
        sin_signbit = _mm_xor_ps(sin_signbit, swapsign);

        // Extended precision modular arithmetic (Cody and Waite)
        // z = ((x - y * DP1) - y * DP2) - y * DP3
        __m128 xmm1 = _mm_mul_ps(y, _mm_cst_ps_DP1);
        __m128 xmm2 = _mm_mul_ps(y, _mm_cst_ps_DP2);
        __m128 xmm3 = _mm_mul_ps(y, _mm_cst_ps_DP3);
        x = _mm_sub_ps(x, xmm1);
        x = _mm_sub_ps(x, xmm2);
        x = _mm_sub_ps(x, xmm3);

        // First polynom x \in [0, pi/4]
        __m128 z = _mm_mul_ps(x, x);

        y = _mm_mul_ps(_mm_cst_ps_coscof_p0, z);
        y = _mm_add_ps(y, _mm_cst_ps_coscof_p1);
        y = _mm_mul_ps(y, z);
        y = _mm_add_ps(y, _mm_cst_ps_coscof_p2);
        y = _mm_mul_ps(y, z);
        y = _mm_mul_ps(y, z);
        __m128 tmp = _mm_mul_ps(z, _mm_cst_ps_0p5);
        y = _mm_sub_ps(y, tmp);
        y = _mm_add_ps(y, _mm_cst_ps_one);

        // Second polynom
        __m128 y2 = _mm_mul_ps(_mm_cst_ps_sincof_p0, z);
        y2 = _mm_add_ps(y2, _mm_cst_ps_sincof_p1);
        y2 = _mm_mul_ps(y2, z);
        y2 = _mm_add_ps(y2, _mm_cst_ps_sincof_p2);
        y2 = _mm_mul_ps(y2, z);
        y2 = _mm_mul_ps(y2, x);
        y2 = _mm_add_ps(y2, x);

        // Select the correct result
        __m128 ysin2 = _mm_and_ps(polymask, y2);
        __m128 ysin1 = _mm_andnot_ps(polymask, y);

        __m128 ycos2 = _mm_sub_ps(y2, ysin2);
        __m128 ycos1 = _mm_sub_ps(y, ysin1);

        __m128 ys = _mm_add_ps(ysin1, ysin2);
        __m128 yc = _mm_add_ps(ycos1, ycos2);

        // Update the sign
        *c = _mm_xor_ps(yc, cos_signbit);
        return _mm_xor_ps(ys, sin_signbit);
    }
#endif

/* Returns radian angle between -pi/2 and +pi/2 whose cosine is x.
 *
 * Analytically, acos(x) = pi/2 - asin(x).
 *
 * However if |x| is near 1, there is cancellation error in subtracting
 * asin(x) from pi/2.  Hence if x < -0.5,
 *    acos(x) = pi - 2.0 * asin( sqrt((1+x)/2) );
 *
 * or if x > +0.5,
 *    acos(x) = 2.0 * asin(  sqrt((1-x)/2)
 */
#if !FST_PLATFORM_HAS_MM_ASIN_PS

    static const __m128 _mm_cst_ps_asincof_p0 = _mm_set1_ps(4.2163199048e-2f);
    static const __m128 _mm_cst_ps_asincof_p1 = _mm_set1_ps(2.4181311049e-2f);
    static const __m128 _mm_cst_ps_asincof_p2 = _mm_set1_ps(4.5470025998e-2f);
    static const __m128 _mm_cst_ps_asincof_p3 = _mm_set1_ps(7.4953002686e-2f);
    static const __m128 _mm_cst_ps_asincof_p4 = _mm_set1_ps(1.6666752422e-1f);
    inline __m128 _mm_asin_ps(__m128 x)
    {
        __m128 signbit = _mm_and_ps(x, *(__m128*) _mm_cst_sign_mask_ps.data());
        __m128 nanmask = _mm_cmplt_ps(x, _mm_cst_ps_one);
        x = _mm_and_ps(x, *(__m128*) _mm_cst_inv_sign_mask_ps.data());

        // if a > 0.5
        __m128 z1 = _mm_mul_ps(_mm_cst_ps_0p5, _mm_sub_ps(_mm_cst_ps_one, x));
        __m128 x1 = _mm_sqrt_ps(z1);

        // else
        __m128 z = _mm_mul_ps(x, x);

        __m128 mask = _mm_cmpgt_ps(x, _mm_cst_ps_0p5);
        z = _mm_andnot_ps(mask, z);
        z = _mm_add_ps(z, _mm_and_ps(mask, z1));

        x = _mm_andnot_ps(mask, x);
        x = _mm_add_ps(x, _mm_and_ps(mask, x1));

        __m128 tmp = _mm_mul_ps(z, _mm_cst_ps_asincof_p0);

        tmp = _mm_add_ps(tmp, _mm_cst_ps_asincof_p1);
        tmp = _mm_mul_ps(tmp, z);
        tmp = _mm_add_ps(tmp, _mm_cst_ps_asincof_p2);
        tmp = _mm_mul_ps(tmp, z);
        tmp = _mm_add_ps(tmp, _mm_cst_ps_asincof_p3);
        tmp = _mm_mul_ps(tmp, z);
        tmp = _mm_add_ps(tmp, _mm_cst_ps_asincof_p4);
        tmp = _mm_mul_ps(tmp, z);

        tmp = _mm_mul_ps(tmp, x);
        tmp = _mm_add_ps(tmp, x);

        __m128 tmp2 = _mm_add_ps(tmp, tmp);
        tmp2 = _mm_sub_ps(_mm_cst_ps_pio2, tmp2);

        tmp = _mm_andnot_ps(mask, tmp);
        tmp = _mm_add_ps(tmp, _mm_and_ps(mask, tmp2));

        tmp = _mm_xor_ps(tmp, signbit);

        // if x > 1 return 0
        tmp = _mm_and_ps(tmp, nanmask);

        return tmp;
    }
#endif

#if !FST_PLATFORM_HAS_MM_ASIN_PD
    // FST_INLINE_VAR constexpr short asincof_s0[] = { to_short(0x5d8c), to_short(0xb6bf), to_short(0xf2a2), to_short(0xc035) };
    // FST_INLINE_VAR constexpr short asincof_s1[] = { to_short(0x7f42), to_short(0xaf6a), to_short(0x6219), to_short(0x4062) };
    // FST_INLINE_VAR constexpr short asincof_s2[] = { to_short(0x63ee), to_short(0x9590), to_short(0xfe08), to_short(0xc077) };
    // FST_INLINE_VAR constexpr short asincof_s3[] = { to_short(0x44be), to_short(0xb0b6), to_short(0x6709), to_short(0x4075) };

    static const __m128d _mm_cst_pd_asincof_s0 = _mm_set1_pd(asincof_s0);
    static const __m128d _mm_cst_pd_asincof_s1 = _mm_set1_pd(asincof_s1);
    static const __m128d _mm_cst_pd_asincof_s2 = _mm_set1_pd(asincof_s2);
    static const __m128d _mm_cst_pd_asincof_s3 = _mm_set1_pd(asincof_s3);

    // FST_INLINE_VAR constexpr short asincof_r0[] = { to_short(0x9f08), to_short(0x988e), to_short(0x4fc3), to_short(0x3f68) };
    // FST_INLINE_VAR constexpr short asincof_r1[] = { to_short(0x290f), to_short(0x59f9), to_short(0x0792), to_short(0xbfe2) };
    // FST_INLINE_VAR constexpr short asincof_r2[] = { to_short(0x3e6a), to_short(0xbaf3), to_short(0xdff5), to_short(0x401b) };
    // FST_INLINE_VAR constexpr short asincof_r3[] = { to_short(0xab68), to_short(0xac01), to_short(0x91aa), to_short(0xc039) };
    // FST_INLINE_VAR constexpr short asincof_r4[] = { to_short(0x081d), to_short(0x40f3), to_short(0x8962), to_short(0x403c) };

    static const __m128d _mm_cst_pd_asincof_r0 = _mm_set1_pd(asincof_r0);
    static const __m128d _mm_cst_pd_asincof_r1 = _mm_set1_pd(asincof_r1);
    static const __m128d _mm_cst_pd_asincof_r2 = _mm_set1_pd(asincof_r2);
    static const __m128d _mm_cst_pd_asincof_r3 = _mm_set1_pd(asincof_r3);
    static const __m128d _mm_cst_pd_asincof_r4 = _mm_set1_pd(asincof_r4);

    // FST_INLINE_VAR constexpr short asincof_q0[] = { to_short(0x0eab), to_short(0x0b5e), to_short(0x7b59), to_short(0xc02d) };
    // FST_INLINE_VAR constexpr short asincof_q1[] = { to_short(0x9054), to_short(0x25fe), to_short(0x9fc0), to_short(0x4051) };
    // FST_INLINE_VAR constexpr short asincof_q2[] = { to_short(0x76d7), to_short(0x6d35), to_short(0x65bb), to_short(0xc062) };
    // FST_INLINE_VAR constexpr short asincof_q3[] = { to_short(0xbf9d), to_short(0x84ff), to_short(0x7056), to_short(0x4061) };
    // FST_INLINE_VAR constexpr short asincof_q4[] = { to_short(0x07ac), to_short(0x0a36), to_short(0x9822), to_short(0xc048) };

    static const __m128d _mm_cst_pd_asincof_q0 = _mm_set1_pd(asincof_q0);
    static const __m128d _mm_cst_pd_asincof_q1 = _mm_set1_pd(asincof_q1);
    static const __m128d _mm_cst_pd_asincof_q2 = _mm_set1_pd(asincof_q2);
    static const __m128d _mm_cst_pd_asincof_q3 = _mm_set1_pd(asincof_q3);
    static const __m128d _mm_cst_pd_asincof_q4 = _mm_set1_pd(asincof_q4);

    // FST_INLINE_VAR constexpr short asincof_p0[] = { to_short(0x8ad3), to_short(0x0bd4), to_short(0x6b9b), to_short(0x3f71) };
    // FST_INLINE_VAR constexpr short asincof_p1[] = { to_short(0x5c16), to_short(0x333e), to_short(0x4341), to_short(0xbfe3) };
    // FST_INLINE_VAR constexpr short asincof_p2[] = { to_short(0x2dd9), to_short(0x178a), to_short(0xc74b), to_short(0x4015) };
    // FST_INLINE_VAR constexpr short asincof_p3[] = { to_short(0x907b), to_short(0xde27), to_short(0x4331), to_short(0xc030) };
    // FST_INLINE_VAR constexpr short asincof_p4[] = { to_short(0x9259), to_short(0xda77), to_short(0x9007), to_short(0x4033) };
    // FST_INLINE_VAR constexpr short asincof_p5[] = { to_short(0xafd5), to_short(0x06ce), to_short(0x656c), to_short(0xc020) };

    static const __m128d _mm_cst_pd_asincof_p0 = _mm_set1_pd(asincof_p0);
    static const __m128d _mm_cst_pd_asincof_p1 = _mm_set1_pd(asincof_p1);
    static const __m128d _mm_cst_pd_asincof_p2 = _mm_set1_pd(asincof_p2);
    static const __m128d _mm_cst_pd_asincof_p3 = _mm_set1_pd(asincof_p3);
    static const __m128d _mm_cst_pd_asincof_p4 = _mm_set1_pd(asincof_p4);
    static const __m128d _mm_cst_pd_asincof_p5 = _mm_set1_pd(asincof_p5);
    inline __m128d _mm_asin_pd(__m128d x)
    {
        // Extract the sign bit and work with absolute values
        __m128d signbit = _mm_and_pd(x, *(__m128d*) _mm_cst_sign_mask_pd.data());
        __m128d nanmask = _mm_cmplt_pd(x, _mm_cst_pd_one);

        x = _mm_and_pd(x, *(__m128d*) _mm_cst_inv_sign_mask_pd.data());

        __m128d mask = _mm_cmpgt_pd(x, _mm_cst_pd_0p5);

        // if x > 0.5
        /* arcsin(1-x) = pi/2 - sqrt(2x)(1+R(x))  */
        __m128d z1 = _mm_sub_pd(_mm_cst_pd_one, x);
        __m128d z = _mm_mul_pd(x, x);
        z = _mm_add_pd(_mm_and_pd(mask, z1), _mm_andnot_pd(mask, z));

        __m128d tmp2 = _mm_mul_pd(z, _mm_cst_pd_asincof_r0);

        tmp2 = _mm_add_pd(tmp2, _mm_cst_pd_asincof_r1);
        tmp2 = _mm_mul_pd(tmp2, z);
        tmp2 = _mm_add_pd(tmp2, _mm_cst_pd_asincof_r2);
        tmp2 = _mm_mul_pd(tmp2, z);
        tmp2 = _mm_add_pd(tmp2, _mm_cst_pd_asincof_r3);
        tmp2 = _mm_mul_pd(tmp2, z);
        tmp2 = _mm_add_pd(tmp2, _mm_cst_pd_asincof_r4);
        tmp2 = _mm_mul_pd(tmp2, z);

        __m128d den2 = _mm_add_pd(z, _mm_cst_pd_asincof_s0);
        den2 = _mm_mul_pd(den2, z);
        den2 = _mm_add_pd(den2, _mm_cst_pd_asincof_s1);
        den2 = _mm_mul_pd(den2, z);
        den2 = _mm_add_pd(den2, _mm_cst_pd_asincof_s2);
        den2 = _mm_mul_pd(den2, z);
        den2 = _mm_add_pd(den2, _mm_cst_pd_asincof_s3);

        tmp2 = _mm_div_pd(tmp2, den2);

        __m128d zz = _mm_sqrt_pd(_mm_add_pd(z, z));
        zz = _mm_sub_pd(_mm_sub_pd(_mm_cst_pd_pio2, zz), _mm_mul_pd(zz, tmp2));
        tmp2 = _mm_and_pd(mask, zz);

        // else
        __m128d tmp = _mm_mul_pd(z, _mm_cst_pd_asincof_p0);

        tmp = _mm_add_pd(tmp, _mm_cst_pd_asincof_p1);
        tmp = _mm_mul_pd(tmp, z);
        tmp = _mm_add_pd(tmp, _mm_cst_pd_asincof_p2);
        tmp = _mm_mul_pd(tmp, z);
        tmp = _mm_add_pd(tmp, _mm_cst_pd_asincof_p3);
        tmp = _mm_mul_pd(tmp, z);
        tmp = _mm_add_pd(tmp, _mm_cst_pd_asincof_p4);
        tmp = _mm_mul_pd(tmp, z);
        tmp = _mm_add_pd(tmp, _mm_cst_pd_asincof_p5);
        tmp = _mm_mul_pd(tmp, z);

        __m128d den = _mm_add_pd(z, _mm_cst_pd_asincof_q0);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_asincof_q1);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_asincof_q2);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_asincof_q3);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_asincof_q4);

        tmp = _mm_div_pd(tmp, den);

        tmp = _mm_mul_pd(tmp, x);
        tmp = _mm_add_pd(tmp, x);
        tmp = _mm_andnot_pd(mask, tmp);

        tmp = _mm_add_pd(tmp2, tmp);

        // Update the sign
        tmp = _mm_xor_pd(tmp, signbit);

        // if x > 1 return 0
        tmp = _mm_and_pd(tmp, nanmask);

        return tmp;
    }
#endif
    /*
 * Returns radian angle between -pi/2 and +pi/2 whose tangent is x.
 *
 * Range reduction is from four intervals into the interval from zero to
 * tan(pi/8). A polynomial approximates the function in this basic interval.
 */

#if !FST_PLATFORM_HAS_MM_ATAN_PS

    static const __m128 _mm_cst_ps_atancof_p0 = _mm_set1_ps(8.05374449538e-2f);
    static const __m128 _mm_cst_ps_atancof_p1 = _mm_set1_ps(-1.38776856032e-1f);
    static const __m128 _mm_cst_ps_atancof_p2 = _mm_set1_ps(1.99777106478e-1f);
    static const __m128 _mm_cst_ps_atancof_p3 = _mm_set1_ps(-3.33329491539e-1f);
    inline __m128 _mm_atan_ps(__m128 x)
    {
        // Extract the sign bit and work with absolute values
        __m128 signbit = _mm_and_ps(x, *(__m128*) _mm_cst_sign_mask_ps.data());
        __m128 y = _mm_setzero_ps();

        x = _mm_and_ps(x, *(__m128*) _mm_cst_inv_sign_mask_ps.data());

        // Range reduction
        __m128 x2 = _mm_div_ps(_mm_cst_ps_mone, x);
        __m128 x3 = _mm_div_ps(_mm_sub_ps(x, _mm_cst_ps_one), _mm_add_ps(x, _mm_cst_ps_one));

        // if x > tan(3 pi/8) (= 2.4142...)
        __m128 mask = _mm_cmpgt_ps(x, _mm_cst_ps_tan3pio8);
        __m128 y2 = _mm_and_ps(mask, _mm_cst_ps_pio2);
        x2 = _mm_and_ps(mask, x2);
        y = _mm_andnot_ps(mask, y);
        x = _mm_andnot_ps(mask, x);
        y = _mm_add_ps(y, y2);
        x = _mm_add_ps(x, x2);

        // if x > tan(pi/8) (= 0.4142...)
        mask = _mm_cmpgt_ps(x, _mm_cst_ps_tanpio8);
        y2 = _mm_and_ps(mask, _mm_cst_ps_pio4);
        x3 = _mm_and_ps(mask, x3);
        y = _mm_andnot_ps(mask, y);
        x = _mm_andnot_ps(mask, x);
        y = _mm_add_ps(y, y2);
        x = _mm_add_ps(x, x3);

        // Polynom computation
        __m128 z = _mm_mul_ps(x, x);
        __m128 tmp = _mm_mul_ps(z, _mm_cst_ps_atancof_p0);

        tmp = _mm_add_ps(tmp, _mm_cst_ps_atancof_p1);
        tmp = _mm_mul_ps(tmp, z);
        tmp = _mm_add_ps(tmp, _mm_cst_ps_atancof_p2);
        tmp = _mm_mul_ps(tmp, z);
        tmp = _mm_add_ps(tmp, _mm_cst_ps_atancof_p3);
        tmp = _mm_mul_ps(tmp, z);
        tmp = _mm_mul_ps(tmp, x);
        tmp = _mm_add_ps(tmp, x);

        y = _mm_add_ps(y, tmp);

        // Update the sign
        y = _mm_xor_ps(y, signbit);

        return y;
    }
#endif

#if !FST_PLATFORM_HAS_MM_ATAN_PD
    // FST_INLINE_VAR constexpr short atancof_q0[] = { to_short(0x603c), to_short(0x5b14), to_short(0xdbc4), to_short(0x4038) };
    // FST_INLINE_VAR constexpr short atancof_q1[] = { to_short(0xfa25), to_short(0x43b8), to_short(0xa0dd), to_short(0x4064) };
    // FST_INLINE_VAR constexpr short atancof_q2[] = { to_short(0xbe3b), to_short(0xd2e2), to_short(0x0e18), to_short(0x407b) };
    // FST_INLINE_VAR constexpr short atancof_q3[] = { to_short(0x49ea), to_short(0x13b0), to_short(0x563f), to_short(0x407e) };
    // FST_INLINE_VAR constexpr short atancof_q4[] = { to_short(0x62ec), to_short(0xfbbd), to_short(0x519e), to_short(0x4068) };

    static const __m128d _mm_cst_pd_atancof_q0 = _mm_set1_pd(atancof_q0);
    static const __m128d _mm_cst_pd_atancof_q1 = _mm_set1_pd(atancof_q1);
    static const __m128d _mm_cst_pd_atancof_q2 = _mm_set1_pd(atancof_q2);
    static const __m128d _mm_cst_pd_atancof_q3 = _mm_set1_pd(atancof_q3);
    static const __m128d _mm_cst_pd_atancof_q4 = _mm_set1_pd(atancof_q4);

    // FST_INLINE_VAR constexpr short atancof_p0[] = { to_short(0x2594), to_short(0xa1f7), to_short(0x007f), to_short(0xbfec) };
    // FST_INLINE_VAR constexpr short atancof_p1[] = { to_short(0x807a), to_short(0x5b6b), to_short(0x2854), to_short(0xc030) };
    // FST_INLINE_VAR constexpr short atancof_p2[] = { to_short(0x0273), to_short(0x3688), to_short(0xc08c), to_short(0xc052) };
    // FST_INLINE_VAR constexpr short atancof_p3[] = { to_short(0xba25), to_short(0x2d05), to_short(0xb8bf), to_short(0xc05e) };
    // FST_INLINE_VAR constexpr short atancof_p4[] = { to_short(0xec8e), to_short(0xfd28), to_short(0x3669), to_short(0xc050) };

    static const __m128d _mm_cst_pd_atancof_p0 = _mm_set1_pd(atancof_p0);
    static const __m128d _mm_cst_pd_atancof_p1 = _mm_set1_pd(atancof_p1);
    static const __m128d _mm_cst_pd_atancof_p2 = _mm_set1_pd(atancof_p2);
    static const __m128d _mm_cst_pd_atancof_p3 = _mm_set1_pd(atancof_p3);
    static const __m128d _mm_cst_pd_atancof_p4 = _mm_set1_pd(atancof_p4);

    inline __m128d _mm_atan_pd(__m128d x)
    {
        // Extract the sign bit and work with absolute values
        __m128d signbit = _mm_and_pd(x, *(__m128d*) _mm_cst_sign_mask_pd.data());
        __m128d y = _mm_setzero_pd();

        x = _mm_and_pd(x, *(__m128d*) _mm_cst_inv_sign_mask_pd.data());

        // Range reduction
        __m128d x2 = _mm_div_pd(_mm_cst_pd_mone, x);
        __m128d x3 = _mm_div_pd(_mm_sub_pd(x, _mm_cst_pd_one), _mm_add_pd(x, _mm_cst_pd_one));

        // if x > tan(3 pi/8) (= 2.4142...)
        __m128d mask = _mm_cmpgt_pd(x, _mm_cst_pd_tan3pio8);
        __m128d y2 = _mm_and_pd(mask, _mm_cst_pd_pio2);
        x2 = _mm_and_pd(mask, x2);
        y = _mm_andnot_pd(mask, y);
        x = _mm_andnot_pd(mask, x);
        y = _mm_add_pd(y, y2);
        x = _mm_add_pd(x, x2);

        // if x > tan(pi/8) (= 0.4142...)
        mask = _mm_cmpgt_pd(x, _mm_cst_pd_tanpio8);
        y2 = _mm_and_pd(mask, _mm_cst_pd_pio4);
        x3 = _mm_and_pd(mask, x3);
        y = _mm_andnot_pd(mask, y);
        x = _mm_andnot_pd(mask, x);
        y = _mm_add_pd(y, y2);
        x = _mm_add_pd(x, x3);

        // Polynom computation
        __m128d z = _mm_mul_pd(x, x);
        __m128d tmp = _mm_mul_pd(z, _mm_cst_pd_atancof_p0);

        tmp = _mm_add_pd(tmp, _mm_cst_pd_atancof_p1);
        tmp = _mm_mul_pd(tmp, z);
        tmp = _mm_add_pd(tmp, _mm_cst_pd_atancof_p2);
        tmp = _mm_mul_pd(tmp, z);
        tmp = _mm_add_pd(tmp, _mm_cst_pd_atancof_p3);
        tmp = _mm_mul_pd(tmp, z);
        tmp = _mm_add_pd(tmp, _mm_cst_pd_atancof_p4);
        tmp = _mm_mul_pd(tmp, z);

        __m128d den = _mm_add_pd(z, _mm_cst_pd_atancof_q0);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_atancof_q1);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_atancof_q2);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_atancof_q3);
        den = _mm_mul_pd(den, z);
        den = _mm_add_pd(den, _mm_cst_pd_atancof_q4);

        tmp = _mm_div_pd(tmp, den);

        tmp = _mm_mul_pd(tmp, x);
        tmp = _mm_add_pd(tmp, x);

        y = _mm_add_pd(y, tmp);

        // Update the sign
        y = _mm_xor_pd(y, signbit);

        return y;
    }
#endif
/*
 * Returns radian angle whose tangent is y/x between -PI and PI
 */
#if !FST_PLATFORM_HAS_MM_ATAN2_PS
    inline __m128 _mm_atan2_ps(__m128 y, __m128 x)
    {

        static __m128 zzero = _mm_setzero_ps();

        __m128 mask = _mm_cmplt_ps(x, zzero);
        __m128 w = _mm_and_ps(mask, _mm_cst_ps_pi);
        mask = _mm_cmplt_ps(y, zzero);
        __m128 mone = _mm_and_ps(mask, _mm_cst_ps_mtwo);
        // {-2, 0} + 1 -> {-1, 1} -> -pi, 0, pi
        w = _mm_mul_ps(_mm_add_ps(mone, _mm_cst_ps_one), w);

        __m128 q = _mm_div_ps(y, x);
        q = _mm_add_ps(w, _mm_atan_ps(q));

        __m128 mask2;

        // atan2(yneg, 0) -> -pi/2
        mask = _mm_cmpeq_ps(x, zzero);
        mask2 = _mm_and_ps(mask, _mm_cmplt_ps(y, zzero));
        q = _mm_andnot_ps(mask2, q);
        w = _mm_and_ps(mask2, _mm_cst_ps_mpio2);
        q = _mm_or_ps(w, q);

        // atan2(ypos, 0) -> pi/2
        mask2 = _mm_and_ps(mask, _mm_cmpgt_ps(y, zzero));
        q = _mm_andnot_ps(mask2, q);
        w = _mm_and_ps(mask2, _mm_cst_ps_pio2);
        q = _mm_or_ps(w, q);

        // atan2(0, 0) -> 0
        mask2 = _mm_and_ps(mask, _mm_cmpeq_ps(y, zzero));
        q = _mm_andnot_ps(mask2, q);
        w = _mm_and_ps(mask2, zzero);
        q = _mm_or_ps(w, q);

        // atan2(0, xneg) -> pi
        mask = _mm_cmplt_ps(x, zzero);
        mask2 = _mm_and_ps(mask, _mm_cmpeq_ps(y, zzero));
        q = _mm_andnot_ps(mask2, q);
        w = _mm_and_ps(mask2, _mm_cst_ps_pi);
        q = _mm_or_ps(w, q);

        return q;
    }
#endif

#if !FST_PLATFORM_HAS_MM_ATAN2_PD
    inline __m128d _mm_atan2_pd(__m128d y, __m128d x)
    {

        static __m128d zzero = _mm_setzero_pd();

        __m128d mask = _mm_cmplt_pd(x, zzero);
        __m128d w = _mm_and_pd(mask, _mm_cst_pd_pi);
        mask = _mm_cmplt_pd(y, zzero);
        __m128d mone = _mm_and_pd(mask, _mm_cst_pd_mtwo);
        // {-2, 0} + 1 -> {-1, 1} -> -pi, 0, pi
        w = _mm_mul_pd(_mm_add_pd(mone, _mm_cst_pd_one), w);

        __m128d q = _mm_div_pd(y, x);
        q = _mm_add_pd(w, _mm_atan_pd(q));

        __m128d mask2;

        // atan2(yneg, 0) -> -pi/2
        mask = _mm_cmpeq_pd(x, zzero);
        mask2 = _mm_and_pd(mask, _mm_cmplt_pd(y, zzero));
        q = _mm_andnot_pd(mask2, q);
        w = _mm_and_pd(mask2, _mm_cst_pd_mpio2);
        q = _mm_or_pd(w, q);

        // atan2(ypos, 0) -> pi/2
        mask2 = _mm_and_pd(mask, _mm_cmpgt_pd(y, zzero));
        q = _mm_andnot_pd(mask2, q);
        w = _mm_and_pd(mask2, _mm_cst_pd_pio2);
        q = _mm_or_pd(w, q);

        // atan2(0, 0) -> 0
        mask2 = _mm_and_pd(mask, _mm_cmpeq_pd(y, zzero));
        q = _mm_andnot_pd(mask2, q);
        w = _mm_and_pd(mask2, zzero);
        q = _mm_or_pd(w, q);

        // atan2(0, xneg) -> pi
        mask = _mm_cmplt_pd(x, zzero);
        mask2 = _mm_and_pd(mask, _mm_cmpeq_pd(y, zzero));
        q = _mm_andnot_pd(mask2, q);
        w = _mm_and_pd(mask2, _mm_cst_pd_pi);
        q = _mm_or_pd(w, q);

        return q;
    }
#endif

#if !FST_PLATFORM_HAS_MM_ADD_PS
    inline __m128 _mm_add_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2], v1[3] + v2[3]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_SUB_PS
    inline __m128 _mm_sub_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2], v1[3] - v2[3]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_MUL_PS
    inline __m128 _mm_mul_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2], v1[3] * v2[3]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_DIV_PS
    inline __m128 _mm_div_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2], v1[3] / v2[3]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_POW_PS
    inline __m128 _mm_pow_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(_FST::pow(v1[0], v2[0]), _FST::pow(v1[1], v2[1]), _FST::pow(v1[2], v2[2]), _FST::pow(v1[3], v2[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_FMOD_PS
    inline __m128 _mm_fmod_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(_FST::fmod(v1[0], v2[0]), _FST::fmod(v1[1], v2[1]), _FST::fmod(v1[2], v2[2]), _FST::fmod(v1[3], v2[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_MIN_PS
    inline __m128 _mm_min_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(_FST::fmin(v1[0], v2[0]), _FST::fmin(v1[1], v2[1]), _FST::fmin(v1[2], v2[2]), _FST::fmin(v1[3], v2[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_MAX_PS
    inline __m128 _mm_max_ps(__m128 x, __m128 y) noexcept
    {
        m128 v1(x);
        m128 v2(y);
        return _mm_set_ps(_FST::fmax(v1[0], v2[0]), _FST::fmax(v1[1], v2[1]), _FST::fmax(v1[2], v2[2]), _FST::fmax(v1[3], v2[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_HADD_PS
#endif

#if !FST_PLATFORM_HAS_MM_SQRT_PS

    /*inline __m128 _mm_sqrt_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(_FST::sqrt(v1[0]), _FST::sqrt(v1[1]), _FST::sqrt(v1[2]), _FST::sqrt(v1[3]));
    }*/
    static inline __m128 _mm_sqrt_ps(__m128 x)
    {

        __m128 op5 = _mm_set1_ps(0.5f);
        __m128 onep5 = _mm_set1_ps(1.5f);

        __m128 half = _mm_mul_ps(x, op5);
        __m128 iszero = _mm_cmpeq_ps(_mm_setzero_ps(), x);
        __m128 x2 = _mm_rsqrt_ps(x);
        x2 = _mm_mul_ps(x2, _mm_sub_ps(onep5, _mm_mul_ps(half, _mm_mul_ps(x2, x2))));
        return _mm_andnot_ps(iszero, _mm_mul_ps(x2, x));
    }

#endif

#if !FST_PLATFORM_HAS_MM_TAN_PS
    inline __m128 _mm_tan_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(_FST::tan(v1[0]), _FST::tan(v1[1]), _FST::tan(v1[2]), _FST::tan(v1[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_LOG_PS
    inline __m128 _mm_log_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(_FST::log(v1[0]), _FST::log(v1[1]), _FST::log(v1[2]), _FST::log(v1[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_LOG10_PS
    inline __m128 _mm_log10_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(_FST::log10(v1[0]), _FST::log10(v1[1]), _FST::log10(v1[2]), _FST::log10(v1[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_LOG2_PS
    inline __m128 _mm_log2_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(_FST::log2(v1[0]), _FST::log2(v1[1]), _FST::log2(v1[2]), _FST::log2(v1[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_EXP10_PS
    inline __m128 _mm_exp10_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(_FST::exp10(v1[0]), _FST::log2(v1[1]), _FST::log2(v1[2]), _FST::log2(v1[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_EXP2_PS
    inline __m128 _mm_exp2_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(_FST::exp2(v1[0]), _FST::exp2(v1[1]), _FST::exp2(v1[2]), _FST::exp2(v1[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_INVSQRT_PS
    inline __m128 _mm_invsqrt_ps(__m128 x) noexcept
    {
        m128 v1(x);
        return _mm_set_ps(1.0f / _FST::sqrt(v1[0]), 1.0f / _FST::sqrt(v1[1]), 1.0f / _FST::sqrt(v1[2]), 1.0f / _FST::sqrt(v1[3]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_ADD_PD
    inline __m128d _mm_add_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(v1[0] + v2[0], v1[1] + v2[1]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_SUB_PD
    inline __m128d _mm_sub_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(v1[0] - v2[0], v1[1] - v2[1]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_MUL_PD
    inline __m128d _mm_mul_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(v1[0] * v2[0], v1[1] * v2[1]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_DIV_PD
    inline __m128d _mm_div_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(v1[0] / v2[0], v1[1] / v2[1]);
    }
#endif

#if !FST_PLATFORM_HAS_MM_POW_PD
    inline __m128d _mm_pow_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(_FST::pow(v1[0], v2[0]), _FST::pow(v1[1], v2[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_FMOD_PD
    inline __m128d _mm_fmod_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(_FST::fmod(v1[0], v2[0]), _FST::fmod(v1[1], v2[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_MIN_PD
    inline __m128d _mm_min_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(_FST::fmin(v1[0], v2[0]), _FST::fmin(v1[1], v2[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_MAX_PD
    inline __m128d _mm_max_pd(__m128d x, __m128d y) noexcept
    {
        m128d v1(x);
        m128d v2(y);
        return _mm_set_pd(_FST::fmax(v1[0], v2[0]), _FST::fmax(v1[1], v2[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_HADD_PD
#endif

#if !FST_PLATFORM_HAS_MM_SQRT_PD
    inline __m128d _mm_sqrt_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::sqrt(v1[0]), _FST::sqrt(v1[1]));
    }

#endif

#if !FST_PLATFORM_HAS_MM_SIN_PD
    inline __m128d _mm_sin_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::sin(v1[0]), _FST::sin(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_COS_PD
    inline __m128d _mm_cos_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::cos(v1[0]), _FST::cos(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_TAN_PD
    inline __m128d _mm_tan_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::tan(v1[0]), _FST::tan(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_LOG_PD
    inline __m128d _mm_log_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::log(v1[0]), _FST::log(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_LOG10_PD
    inline __m128d _mm_log10_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::log10(v1[0]), _FST::log10(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_LOG2_PD
    inline __m128d _mm_log2_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::log2(v1[0]), _FST::log2(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_EXP10_PD
    inline __m128d _mm_exp10_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::exp10(v1[0]), _FST::log2(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_EXP2_PD
    inline __m128d _mm_exp2_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(_FST::exp2(v1[0]), _FST::exp2(v1[1]));
    }
#endif

#if !FST_PLATFORM_HAS_MM_INVSQRT_PD
    inline __m128d _mm_invsqrt_pd(__m128d x) noexcept
    {
        m128d v1(x);
        return _mm_set_pd(1.0f / _FST::sqrt(v1[0]), 1.0f / _FST::sqrt(v1[1]));
    }
#endif
FST_END_SUB_NAMESPACE

#else
#define FST_HAS_SIMD 0
#endif
