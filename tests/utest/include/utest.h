#pragma once

///

//#define FST_RUN_BENCHMARKS 1

#include "fst/common.h"

FST_DISABLE_ALL_WARNINGS_BEGIN

#ifdef FST_BENCHMARKS
#include "fst_bench.h"
#endif

#include <iostream>
#include <string>

#if FST_HAS_ASSERT
#if FST_HAS_EXCEPT
#define FST_USE_ASSERT_MSG 0
#define FST_ASSERT_HOOK(Expr, File, Line)
#define FST_ASSERT_BREAK() throw std::exception("assert")
#endif
#endif // FST_HAS_ASSERT

#include "catch2.h"

#define GIVEN(...) CATCH_GIVEN(__VA_ARGS__)
#define THEN(...) CATCH_THEN(__VA_ARGS__)
#define WHEN(...) CATCH_WHEN(__VA_ARGS__)

#define TEST_CASE(...) CATCH_TEST_CASE(__VA_ARGS__)
#define TEST_CASE_T(...) CATCH_TEMPLATE_TEST_CASE(__VA_ARGS__)

#define TEST_SECTION(...) CATCH_SECTION(__VA_ARGS__)
#define TEST_BLOCK(...)

#define REQUIRE(...) CATCH_REQUIRE(__VA_ARGS__)
#define REQUIRE_TRUE(...) REQUIRE(__VA_ARGS__)
#define REQUIRE_FALSE(...) CATCH_REQUIRE_FALSE(__VA_ARGS__)
#define REQUIRE_EQ(A, B) REQUIRE((A) == (B))
#define REQUIRE_NE(A, B) REQUIRE((A) != (B))

#define EXPECT(...) REQUIRE(__VA_ARGS__)
#define EXPECT_TRUE(...) REQUIRE(__VA_ARGS__)
#define EXPECT_FALSE(...) REQUIRE(!(__VA_ARGS__))
#define EXPECT_EQ(A, B) REQUIRE_EQ(A, B)
#define EXPECT_NE(A, B) REQUIRE_NE(A, B)

#define CHECK(...) CATCH_CHECK(__VA_ARGS__)
#define CHECK_EQ(A, B) CHECK((A) == (B))

#define CHECK_FLOAT_APPROX(A, B, EPSILON) CHECK(std::abs((A) - (B)) <= EPSILON)
#define REQUIRE_FLOAT_APPROX(A, B, EPSILON) REQUIRE(std::abs((A) - (B)) <= EPSILON)

#define __UNUSED_TEST_CASE(TestName, ...) static void TestName()
#define UNUSED_TEST_CASE(...) __UNUSED_TEST_CASE(INTERNAL_CATCH_UNIQUE_NAME(CATCH2_INTERNAL_TEST_), __VA_ARGS__)

#if FST_TEST_RUN_BENCHMARKS
#define BENCH_CASE(...) CATCH_TEST_CASE(__VA_ARGS__)
#else
#define BENCH_CASE(...) UNUSED_TEST_CASE(__VA_ARGS__)
#endif // FST_TEST_RUN_BENCHMARKS

#define PRINT(VAR) std::cout << #VAR << " " << (VAR) << std::endl

FST_BEGIN_NAMESPACE
    namespace test
    {
        template <class _Buffer, class T>
        inline void check_compare_buffer(const _Buffer& buffer, T value)
        {
            for (size_t i = 0; i < buffer.size(); i++)
            {
                CHECK(buffer[i] == value);
            }
        }

        template <class _Buffer, class T>
        inline void check_compare_buffer(const _Buffer& buffer, size_t size, T value)
        {
            for (size_t i = 0; i < size; i++)
            {
                CHECK(buffer[i] == value);
            }
        }

        template <class _Buffer, class T>
        inline void require_compare_buffer(const _Buffer& buffer, T value)
        {
            for (size_t i = 0; i < buffer.size(); i++)
            {
                REQUIRE(buffer[i] == value);
            }
        }

        template <class _Buffer, class T>
        inline void require_compare_buffer(const _Buffer& buffer, size_t size, T value)
        {
            for (size_t i = 0; i < size; i++)
            {
                REQUIRE(buffer[i] == value);
            }
        }

        template <class _Buffer1, class _Buffer2>
        inline void check_compare_buffers(const _Buffer1& buffer1, const _Buffer2& buffer2)
        {
            for (size_t i = 0; i < buffer1.size(); i++)
            {
                CHECK(buffer1[i] == buffer2[i]);
            }
        }

        template <class _Buffer1, class _Buffer2, class T>
        inline void check_compare_buffers(const _Buffer1& buffer1, const _Buffer2& buffer2, T epsilon)
        {
            for (size_t i = 0; i < buffer1.size(); i++)
            {
                CHECK_FLOAT_APPROX(buffer1[i], buffer2[i], epsilon);
            }
        }

        template <class _Buffer1, class _Buffer2>
        inline void require_compare_buffers(const _Buffer1& buffer1, const _Buffer2& buffer2)
        {
            for (size_t i = 0; i < buffer1.size(); i++)
            {
                REQUIRE(buffer1[i] == buffer2[i]);
            }
        }

        template <class _Buffer1, class _Buffer2, class T>
        inline void require_compare_buffers(const _Buffer1& buffer1, const _Buffer2& buffer2, T epsilon)
        {
            for (size_t i = 0; i < buffer1.size(); i++)
            {
                REQUIRE_FLOAT_APPROX(buffer1[i], buffer2[i], epsilon);
            }
        }

        template <class _Buffer>
        inline void print_buffer(const _Buffer& buffer)
        {
            std::cout << "{ ";
            for (size_t i = 0; i < buffer.size() - 1; i++)
            {
                std::cout << buffer[i] << ", ";
            }

            std::cout << buffer[buffer.size() - 1] << " }" << std::endl;
        }
    } // namespace test.
FST_END_NAMESPACE

FST_DISABLE_ALL_WARNINGS_END
