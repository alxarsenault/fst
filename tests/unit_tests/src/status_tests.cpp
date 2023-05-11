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

    inline void my_func(int a, fst::status_ref st = fst::status_ref::empty())
    {
        if (a == 0) { st = fst::status_code::success; }
        else { st = fst::status_code::not_supported; }
    }

    TEST_CASE("fst::status_ref", "[core]")
    {
        {
            my_func(0);
        }

        {
            fst::status st;
            my_func(0, st);
            REQUIRE(st);
        }

        {
            fst::status st;
            my_func(1, st);
            REQUIRE_FALSE(st);
            REQUIRE(st.code == fst::status_code::not_supported);
        }

        {
            fst::error_result er;
            my_func(0, er);
            REQUIRE_FALSE(er);
        }

        {
            fst::error_result er;
            my_func(1, er);
            REQUIRE(er);
            REQUIRE(er.code == fst::status_code::not_supported);
        }

        {
            fst::status st;
            fst::status_ref st_ref = st;
            my_func(0, st_ref);
            REQUIRE(st_ref);

            //fst::status ss = st_ref;
        }
    }
} // namespace
