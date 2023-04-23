#include "utest.h"
#include "fst/array.h"
#include "fst/allocator.h"

namespace
{
    TEST_CASE("fst::basic_array", "[array]")
    {
        fst::memory_zone_proxy proxy = fst::default_memory_zone::proxy();
        std::vector<float, fst::allocator<float, fst::default_memory_category, fst::memory_zone_proxy>> vec(proxy);

        vec.resize(32);
    }
} // namespace
