#include "utest.h"
#include "fst/memory.h"
#include "fst/vector.h"
#include "fst/memory_pool.h"

namespace
{
    TEST_CASE("fst::memory", "[core]")
    {
        {
            REQUIRE(fst::is_power_of_two(fst::platform_alignment));
            using vector_type = fst::vector<int>;
            vector_type vec;
            vec.resize(89);
        }

        {
            fst::array<int, 1024> buffer;
            fst::object_memory_pool<4, 4, fst::default_memory_category, fst::void_memory_zone> pool(
                1024, fst::byte_range((uint8_t*) buffer.data(), buffer.size() * sizeof(int)));

            void* ptr = pool.aligned_allocate(4, 4);
            fst::unused(ptr);
        }
        {

            fst::pool_memory_zone pool(5, 1024 * 1024);
            using vector_type = fst::vector<int, alignof(int), fst::default_memory_category, fst::pool_memory_zone>;
            vector_type vec(pool);

            //fst::debug()(sizeof(int) * 78);
            vec.push_back(78);
            vec.resize(80);

            vector_type vec2 = vec;
            vec2.push_back(66);
            //pool.aligned_allocate(32, 32, fst::default_memory_category::id());
        }
        /*{

            fst::sm_memory_zone::init(5, (48 * 1024 * 1024));
            void* ptr = fst::sm_memory_zone::aligned_allocate(32, 32, fst::default_memory_category::id());
            REQUIRE(ptr);
            REQUIRE(fst::is_aligned(ptr, 32));
            fst::sm_memory_zone::aligned_deallocate(ptr, fst::default_memory_category::id());

            fst::sm_memory_zone::release();
        }*/
    }
} // namespace
