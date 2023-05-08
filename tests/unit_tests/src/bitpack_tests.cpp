#include "utest.h"
#include "fst/bitpack.h"
#include "fst/bitpack.h"

namespace
{
    TEST_CASE("fst::bitpack", "[bitpack]")
    {
        {
            fst::bitpack<uint8_t, fst::bitpack_t<uint8_t, 4>, fst::bitpack_t<uint8_t, 2>, fst::bitpack_t<bool, 1>, fst::bitpack_t<bool, 1>> bits;
            REQUIRE(sizeof(bits) == sizeof(uint8_t));
            bits.set<0>(6);
            bits.set<1>(1);
            bits.set<2>(true);
            bits.set<3>(false);
            REQUIRE(bits.get<0>() == 6);
            REQUIRE(bits.get<1>() == 1);
            REQUIRE(bits.get<2>() == true);
            REQUIRE(bits.get<3>() == false);
        }

        {
            using istruct = fst::bitpack<uint64_t, fst::bitpack_t<int8_t, 8>, fst::bitpack_t<uint64_t, 49>, fst::bitpack_t<uint8_t, 6>, fst::bitpack_t<bool, 1>>;
            REQUIRE(sizeof(istruct) == sizeof(uint64_t));

            {
                istruct s = { int8_t(3), uint64_t(2), uint8_t(18), false };
                REQUIRE_EQ(s.get<0>(), 3);
                REQUIRE_EQ(s.get<1>(), 2);
                REQUIRE_EQ(s.get<2>(), 18);
                REQUIRE_EQ(s.get<3>(), false);
            }

            {
                istruct s;
                s.set<0>(-3);
                s.set<1>(34);
                s.set<2>(3);
                s.set<3>(true);

                REQUIRE_EQ(s.get<0>(), -3);
                REQUIRE_EQ(s.get<1>(), 34);
                REQUIRE_EQ(s.get<2>(), 3);
                REQUIRE_EQ(s.get<3>(), true);

                istruct s2 = s;
                REQUIRE_EQ(s2.get<0>(), -3);
                REQUIRE_EQ(s2.get<1>(), 34);
                REQUIRE_EQ(s2.get<2>(), 3);
                REQUIRE_EQ(s2.get<3>(), true);
            }
        }
    }
} // namespace
