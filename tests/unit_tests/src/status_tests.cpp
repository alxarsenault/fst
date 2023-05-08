#include "utest.h"
#include "fst/status_code.h"

namespace
{
    TEST_CASE("fst::status", "[core]")
    {
        {
            fst::status s;
            REQUIRE(s);
        }

        {
            fst::status s = fst::status_code::success;
            REQUIRE(s);
        }

        {
            fst::status s = fst::status_code::unknown;
            REQUIRE_FALSE(s);
        }
    }

    TEST_CASE("fst::error_result", "[core]")
    {
        {
            fst::error_result e;
            REQUIRE_FALSE(e);
        }

        {
            fst::error_result e = fst::status_code::success;
            REQUIRE_FALSE(e);
        }

        {
            fst::error_result e = fst::status_code::unknown;
            REQUIRE(e);
        }
    }
} // namespace
