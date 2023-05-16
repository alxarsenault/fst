#include "utest.h"
#include "fst/number.h"
#include "fst/profiler.h"

namespace
{
    TEST_CASE("fst::number::push_back()", "[array]")
    {
        fst::number<int> a = 20;
        a += 2;

        REQUIRE(a == 22);

        int& b = a;
        b = 55;
        REQUIRE_EQ(a, 55);

        a = 22;
        // *
        a *= 2;
        REQUIRE_EQ(a, 44);

        REQUIRE_EQ(a * 2, 88);

        // <
        REQUIRE(a < 50);
        REQUIRE(a <= 50);
        REQUIRE(a <= 44);

        // >
        REQUIRE(a > 20);
        REQUIRE(a >= 25);
        REQUIRE(a >= 44);
    }


    TEST_CASE("non_zero_number")
    {
        fst::dsp_float_number<float> d = 0;

        fst::float_number<float> f = 3232.0f;
        REQUIRE_EQ(f, 3232.0f);

        //fst::number<float> c;
        fst::non_zero_number<int> a = 32;
        REQUIRE_EQ(a, 32);

        fst::bounded_number<int, 0, 5> b = 3;
        REQUIRE_EQ(b, 3);
    }
} // namespace
