#include "utest.h"
#include "fst/file_view.h"

namespace
{
    TEST_CASE("fst::file_view")
    {
        fst::file_view file;
        REQUIRE(file.open(FST_TEST_RESOURCES_DIRECTORY "/test.txt"));
        REQUIRE(file.str() == "Test");

        //fst::print(fst::status(fst::status_code::address_in_use));
    }
} // namespace
