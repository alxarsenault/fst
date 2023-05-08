#include "utest.h"
#include "fst/file.h"

namespace
{
    TEST_CASE("fst::file")
    {
        const char* fpath = FST_TEST_OUTPUT_RESOURCES_DIRECTORY "/bingo2.txt";
        fst::file file;

        // write.
        REQUIRE(file.open(fpath, fst::open_mode::write | fst::open_mode::create_always));
        REQUIRE(file.write("john", 4) == 4);
        REQUIRE(file.close());

        // read.
        char buffer[32] = {0};
        REQUIRE(file.open(fpath, fst::open_mode::read | fst::open_mode::open_existing));
        REQUIRE(file.read(buffer, 4) == 4);
        REQUIRE(file.close());
        REQUIRE(fst::string_view((const char*)buffer) == "john");
    }
} // namespace
