#include "utest.h"
#include "fst/pointer.h"
#include "fst/array.h"


namespace
{
    TEST_CASE("fst::unique_ptr", "[pointer]")
    {
        fst::unique_ptr<int> ptr = fst::unique_ptr<int>::make(32);
        REQUIRE(ptr);
        REQUIRE(*ptr == 32);

        *ptr = 34;
        REQUIRE(*ptr == 34);

        ptr.reset();

        REQUIRE(!ptr);
    }
} // namespace
