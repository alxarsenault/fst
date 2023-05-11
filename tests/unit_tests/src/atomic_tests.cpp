#include "utest.h"
#include "fst/atomic.h"


namespace
{
    TEST_CASE("fst::atomic", "[core]")
    {
        //atomic::msvc::
        fst::atomic<uint32_t> a =89;
        a = 78;

    }
} // namespace
