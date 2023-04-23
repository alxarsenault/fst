#include "utest.h"
#include "fst/simd.h"

namespace
{

    TEST_CASE("fst::simd::array", "[simd]")
    {
        using array_type = fst::simd::array<float, 64>;
        array_type buffer1;
        array_type buffer2;

        TEST_BLOCK("fill")
        {
            buffer1.fill(32.0f);
            fst::test::require_compare_buffer(buffer1, 32.0f);

            buffer1.fill_increment(1);
            for (size_t i = 0; i < buffer1.size(); i++)
            {
                REQUIRE(buffer1[i] == (float) i);
            }

            buffer1.fill_increment(2);
            for (size_t i = 0; i < buffer1.size(); i++)
            {
                REQUIRE(buffer1[i] == (float) (i * 2));
            }

            buffer1.fill_increment(2, 1);
            for (size_t i = 0; i < buffer1.size(); i++)
            {
                REQUIRE(buffer1[i] == (float) ((i + 1) * 2));
            }
        }

        TEST_BLOCK("clear")
        {
            buffer1.clear();
            fst::test::require_compare_buffer(buffer1, 0.0f);
        }

        TEST_BLOCK("scalar += add")
        {
            buffer1.fill(1);
            buffer1 += 3.0f;
            fst::test::require_compare_buffer(buffer1, 4.0f);

            buffer1.add(3.0f);
            fst::test::require_compare_buffer(buffer1, 7.0f);
        }

        TEST_BLOCK("scalar -= sub")
        {
            buffer1.fill(5);
            buffer1 -= 3.0f;
            fst::test::require_compare_buffer(buffer1, 2.0f);

            buffer1.sub(1.0f);
            fst::test::require_compare_buffer(buffer1, 1.0f);
        }

        TEST_BLOCK("scalar *= mul")
        {
            buffer1.fill(2);
            buffer1 *= 4.0f;
            fst::test::require_compare_buffer(buffer1, 8.0f);

            buffer1.mul(2.0f);
            fst::test::require_compare_buffer(buffer1, 16.0f);
        }

        TEST_BLOCK("scalar /= div")
        {
            buffer1.fill(4);
            buffer1 /= 2.0f;
            fst::test::require_compare_buffer(buffer1, 2.0f);

            buffer1.div(2.0f);
            fst::test::require_compare_buffer(buffer1, 1.0f);
        }

        TEST_BLOCK("+= add")
        {
            buffer1.fill(1);
            buffer2.fill(2);
            buffer1 += buffer2;
            fst::test::require_compare_buffer(buffer1, 3.0f);

            buffer1.add(buffer2);
            fst::test::require_compare_buffer(buffer1, 5.0f);
        }

        TEST_BLOCK("-= sub")
        {
            buffer1.fill(3);
            buffer2.fill(2);
            buffer1 -= buffer2;
            fst::test::require_compare_buffer(buffer1, 1.0f);

            buffer1.sub(buffer2);
            fst::test::require_compare_buffer(buffer1, -1.0f);
        }

        TEST_BLOCK("*= mul")
        {
            buffer1.fill(2);
            buffer2.fill(4);
            buffer1 *= buffer2;
            fst::test::require_compare_buffer(buffer1, 8.0f);

            buffer1.mul(buffer2);
            fst::test::require_compare_buffer(buffer1, 32.0f);
        }

        TEST_BLOCK("/= div")
        {
            buffer1.fill(4);
            buffer2.fill(2);
            buffer1 /= buffer2;
            fst::test::require_compare_buffer(buffer1, 2.0f);

            buffer1.div(buffer2);
            fst::test::require_compare_buffer(buffer1, 1.0f);
        }

        TEST_BLOCK("scalar +")
        {
            buffer1.fill(4);
            array_type a = buffer1 + 3.0f;
            fst::test::require_compare_buffer(a, 7.0f);
            fst::test::require_compare_buffer(buffer1, 4.0f);
        }

        TEST_BLOCK("scalar -")
        {
            buffer1.fill(4);
            array_type a = buffer1 - 3.0f;
            fst::test::require_compare_buffer(a, 1.0f);
            fst::test::require_compare_buffer(buffer1, 4.0f);
        }

        TEST_BLOCK("scalar *")
        {
            buffer1.fill(4);
            array_type a = buffer1 * 3.0f;
            fst::test::require_compare_buffer(a, 12.0f);
            fst::test::require_compare_buffer(buffer1, 4.0f);
        }

        TEST_BLOCK("scalar /")
        {
            buffer1.fill(6);
            array_type a = buffer1 / 3.0f;
            fst::test::require_compare_buffer(a, 2.0f);
            fst::test::require_compare_buffer(buffer1, 6.0f);
        }

        TEST_BLOCK("+")
        {
            buffer1.fill(4);
            buffer2.fill(2);
            array_type a = buffer1 + buffer2;
            fst::test::require_compare_buffer(a, 6.0f);
            fst::test::require_compare_buffer(buffer1, 4.0f);
        }

        TEST_BLOCK("-")
        {
            buffer1.fill(4);
            buffer2.fill(2);
            array_type a = buffer1 - buffer2;
            fst::test::require_compare_buffer(a, 2.0f);
            fst::test::require_compare_buffer(buffer1, 4.0f);
        }

        TEST_BLOCK("*")
        {
            buffer1.fill(4);
            buffer2.fill(2);
            array_type a = buffer1 * buffer2;
            fst::test::require_compare_buffer(a, 8.0f);
            fst::test::require_compare_buffer(buffer1, 4.0f);
        }

        TEST_BLOCK("/")
        {
            buffer1.fill(6);
            buffer2.fill(3);
            array_type a = buffer1 / buffer2;
            fst::test::require_compare_buffer(a, 2.0f);
            fst::test::require_compare_buffer(buffer1, 6.0f);
        }
    }
} // namespace
