#include "utest.h"
#include "fst/int24_t.h"

namespace
{
    TEST_CASE("fst::int24_t", "[int24_t]")
    {
        fst::int24_t j = 89;
        REQUIRE((j == 89));
    }
} // namespace
