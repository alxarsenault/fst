macro(fst_platform_info)
    include(CheckSourceCompiles)
    include(CheckIncludeFile)

    # Windows header
    check_include_file(Windows.h FST_PLATFORM_HAS_WINDOWS_H)

    # Unix
    check_include_file(unistd.h FST_PLATFORM_HAS_UNISTD_H)
    if(FST_PLATFORM_HAS_UNISTD_H)
        check_include_file(fcntl.h FST_PLATFORM_HAS_FCNTL_H)
        check_include_file(sys/mman.h FST_PLATFORM_HAS_SYS_MMAN_H)
        check_include_file(sys/types.h FST_PLATFORM_HAS_SYS_TYPES_H)
    endif()

    if(FST_USE_SIMD_128 OR FST_USE_SIMD_256 OR FST_USE_SIMD_512)
        # SSE
        check_include_file(immintrin.h FST_PLATFORM_HAS_IMMINTRIN_H)
        if(FST_PLATFORM_HAS_IMMINTRIN_H)
            check_include_file(emmintrin.h FST_PLATFORM_HAS_EMMINTRIN_H)
            check_include_file(xmmintrin.h FST_PLATFORM_HAS_XMMINTRIN_H)
            check_include_file(tmmintrin.h FST_PLATFORM_HAS_TMMINTRIN_H)

            set(__FST_INCLUDE_IMMINTRIN_H "#include <emmintrin.h>\n#include <immintrin.h>\n")
            set(__FST_SIMD_MAIN_BLOCK "${__FST_INCLUDE_IMMINTRIN_H} int main(void)\n")

            # cmake-format: off
            # SSE
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); }" FST_PLATFORM_HAS_SSE)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); __m128d b = _mm_setzero_pd(); a = _mm_add_sd(a, b); }" FST_PLATFORM_HAS_SSE2)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128i a = _mm_setzero_si128(); a = _mm_abs_epi16(a); }" FST_PLATFORM_HAS_SSE3)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_ceil_pd(a); }" FST_PLATFORM_HAS_SSE4_1)

            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_sincos_ps(&a, a); }" FST_PLATFORM_HAS_MM_SINCOS_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_asin_ps(a); }" FST_PLATFORM_HAS_MM_ASIN_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_atan_ps(a); }" FST_PLATFORM_HAS_MM_ATAN_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_atan2_ps(a, a); }" FST_PLATFORM_HAS_MM_ATAN2_PS)

            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_add_ps(a, a); }" FST_PLATFORM_HAS_MM_ADD_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_sub_ps(a, a); }" FST_PLATFORM_HAS_MM_SUB_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_mul_ps(a, a); }" FST_PLATFORM_HAS_MM_MUL_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_div_ps(a, a); }" FST_PLATFORM_HAS_MM_DIV_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_pow_ps(a, a); }" FST_PLATFORM_HAS_MM_POW_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_fmod_ps(a, a); }" FST_PLATFORM_HAS_MM_FMOD_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_min_ps(a, a); }" FST_PLATFORM_HAS_MM_MIN_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_max_ps(a, a); }" FST_PLATFORM_HAS_MM_MAX_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_hadd_ps(a, a); }" FST_PLATFORM_HAS_MM_HADD_PS)

            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_sqrt_ps(a); }" FST_PLATFORM_HAS_MM_SQRT_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_invsqrt_ps(a); }" FST_PLATFORM_HAS_MM_INVSQRT_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_sin_ps(a); }" FST_PLATFORM_HAS_MM_SIN_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_cos_ps(a); }" FST_PLATFORM_HAS_MM_COS_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_tan_ps(a); }" FST_PLATFORM_HAS_MM_TAN_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_log_ps(a); }" FST_PLATFORM_HAS_MM_LOG_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_log10_ps(a); }" FST_PLATFORM_HAS_MM_LOG10_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_log2_ps(a); }" FST_PLATFORM_HAS_MM_LOG2_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_exp10_ps(a); }" FST_PLATFORM_HAS_MM_EXP10_PS)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128 a = _mm_setzero_ps(); a = _mm_exp2_ps(a); }" FST_PLATFORM_HAS_MM_EXP2_PS)

            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_sincos_pd(&a, a); }" FST_PLATFORM_HAS_MM_SINCOS_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_asin_pd(a); }" FST_PLATFORM_HAS_MM_ASIN_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_atan_pd(a); }" FST_PLATFORM_HAS_MM_ATAN_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_atan2_pd(a, a); }" FST_PLATFORM_HAS_MM_ATAN2_PD)

            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_add_pd(a, a); }" FST_PLATFORM_HAS_MM_ADD_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_sub_pd(a, a); }" FST_PLATFORM_HAS_MM_SUB_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_mul_pd(a, a); }" FST_PLATFORM_HAS_MM_MUL_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_div_pd(a, a); }" FST_PLATFORM_HAS_MM_DIV_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_pow_pd(a, a); }" FST_PLATFORM_HAS_MM_POW_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_fmod_pd(a, a); }" FST_PLATFORM_HAS_MM_FMOD_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_min_pd(a, a); }" FST_PLATFORM_HAS_MM_MIN_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_max_pd(a, a); }" FST_PLATFORM_HAS_MM_MAX_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_hadd_pd(a, a); }" FST_PLATFORM_HAS_MM_HADD_PD)

            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_sqrt_pd(a); }" FST_PLATFORM_HAS_MM_SQRT_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_invsqrt_pd(a); }" FST_PLATFORM_HAS_MM_INVSQRT_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_sin_pd(a); }" FST_PLATFORM_HAS_MM_SIN_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_cos_pd(a); }" FST_PLATFORM_HAS_MM_COS_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_tan_pd(a); }" FST_PLATFORM_HAS_MM_TAN_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_log_pd(a); }" FST_PLATFORM_HAS_MM_LOG_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_log10_pd(a); }" FST_PLATFORM_HAS_MM_LOG10_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_log2_pd(a); }" FST_PLATFORM_HAS_MM_LOG2_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_exp10_pd(a); }" FST_PLATFORM_HAS_MM_EXP10_PD)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m128d a = _mm_setzero_pd(); a = _mm_exp2_pd(a); }" FST_PLATFORM_HAS_MM_EXP2_PD)

            # AVX
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m256 a = _mm256_setzero_ps(); }" FST_PLATFORM_HAS_AVX)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m256i i;  __m256i a = _mm256_abs_epi16(i); }" FST_PLATFORM_HAS_AVX2)
            check_source_compiles(CXX "${__FST_SIMD_MAIN_BLOCK} { __m512 a = _mm512_setzero_ps(); }" FST_PLATFORM_HAS_AVX_512)
            # cmake-format: on
        endif()
    endif()

    # Neon
    if(FST_USE_NEON)
        check_include_file(arm_neon.h FST_PLATFORM_HAS_NEON_H)
        if(FST_PLATFORM_HAS_NEON_H)
            # set(CMAKE_REQUIRED_FLAGS ${HOST_FLAGS} -ftree-vectorize -mfpu=neon)

            check_source_compiles(
                CXX "#include <arm_neon.h>\n int main(void) { float32x4_t v1 = { 1.0, 2.0, 3.0, 4.0 }; return 0; }"
                FST_PLATFORM_HAS_NEON)
            # list(APPEND HOST_FLAGS -mfpu=neon)
        endif()
    endif()

endmacro()
