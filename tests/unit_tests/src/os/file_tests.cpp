#include "utest.h"
#include "fst/os/file.h"

namespace
{
    TEST_CASE("fst::os::file")
    {
        fst::os::path fpath = FST_TEST_RESOURCES_DIRECTORY "/bingo.txt";
        fst::os::file file;
        file.open(fpath, fst::os::file::flags::read | fst::os::file::flags::write | fst::os::file::flags::create_always );
    }
} // namespace
