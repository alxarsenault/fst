#include "utest.h"
#include "fst/array.h"
#include "fst/memory.h"
#include "fst/vector.h"
#include "fst/memory_pool.h"

namespace
{
    TEST_CASE("fst::memory_pool buffer2dsds4", "[core]")
    {
        using memory_pool_type = fst::forward_memory_pool<>;
                    memory_pool_type pool(memory_pool_type::default_chunk_capacity);

        }
    TEST_CASE("fst::memory_pool buffer24", "[core]")
    {
        using memory_pool_type = fst::forward_memory_pool<fst::default_memory_category, fst::default_memory_zone>;

        {
            memory_pool_type pool(memory_pool_type::default_chunk_capacity);
            REQUIRE_EQ(pool.capacity(), 0);
        }

        {
            memory_pool_type pool(memory_pool_type::default_chunk_capacity, true);
            REQUIRE_EQ(pool.capacity(), memory_pool_type::default_chunk_capacity);

            REQUIRE(pool.aligned_allocate(32));
            REQUIRE_EQ(pool.capacity(), memory_pool_type::default_chunk_capacity);
        }
    }

    TEST_CASE("fst::memory_pool buffer2", "[core]")
    {
        using memory_pool_type = fst::forward_memory_pool<fst::default_memory_category, fst::default_memory_zone>;

        fst::array<uint8_t, 1024, fst::default_alignment> buffer;

        memory_pool_type pool((void*) buffer.data(), buffer.size());
        REQUIRE_EQ(pool.capacity(), buffer.size() - memory_pool_type::total_reserved_size);

        REQUIRE_EQ(pool.size(), 0);

        void* ptr = pool.aligned_allocate(32);
        REQUIRE(ptr);
        //REQUIRE(fst::is_aligned(ptr, 64));

        REQUIRE_EQ(pool.size(), 32);
        //REQUIRE(pool.capacity() == 1024 - memory_pool_type::total_reserved_size);
    }

    TEST_CASE("fst::memory_pool alignment", "[core]")
    {
        using memory_pool_type = fst::forward_memory_pool<fst::default_memory_category, fst::default_memory_zone>;

        fst::array<uint8_t, 1024, fst::default_alignment> buffer;

        memory_pool_type pool((void*) buffer.data(), buffer.size());
        REQUIRE_EQ(pool.capacity(), buffer.size() - memory_pool_type::total_reserved_size);

        void* ptr = pool.aligned_allocate(32, 64);
        REQUIRE(ptr);
        REQUIRE(fst::is_aligned(ptr, 64));

        //fst::debug()(pool.size());
        //REQUIRE(pool.capacity() == 1024 - memory_pool_type::total_reserved_size);
    }
    TEST_CASE("fst::memory_pool", "[core]")
    {
        using memory_pool_type = fst::forward_memory_pool<fst::default_memory_category, fst::default_memory_zone>;
        memory_pool_type pool;
        REQUIRE_FALSE(pool.is_shared());

        {
            memory_pool_type pool2 = pool;
            REQUIRE(pool.is_shared());
        }

        REQUIRE_FALSE(pool.is_shared());

        void* ptr = pool.aligned_allocate(32);
        REQUIRE(ptr);
        REQUIRE_EQ(pool.capacity(), memory_pool_type::default_chunk_capacity);

        void* ptr2 = pool.aligned_allocate(32);
        REQUIRE(ptr2);
        REQUIRE_EQ(pool.capacity(), memory_pool_type::default_chunk_capacity);

        void* ptr3 = pool.aligned_allocate(1024 * 64);
        REQUIRE(ptr3);

        REQUIRE_EQ(pool.capacity(), 2 * memory_pool_type::default_chunk_capacity);
    }

    TEST_CASE("fst::memory_pool buffer", "[core]")
    {
        using memory_pool_type = fst::forward_memory_pool<fst::default_memory_category, fst::default_memory_zone>;

        fst::array<uint8_t, 1024, fst::default_alignment> buffer;

        memory_pool_type pool((void*) buffer.data(), buffer.size());
        REQUIRE_EQ(pool.capacity(), buffer.size() - memory_pool_type::total_reserved_size);

        void* ptr = pool.aligned_allocate(32);
        REQUIRE(ptr);
        REQUIRE_EQ(pool.capacity(), buffer.size() - memory_pool_type::total_reserved_size);

        REQUIRE(pool.aligned_allocate(32));
        REQUIRE_EQ(pool.capacity(), buffer.size() - memory_pool_type::total_reserved_size);

        REQUIRE(pool.aligned_allocate(1024 * 64));
        REQUIRE_EQ(pool.capacity(), memory_pool_type::default_chunk_capacity + buffer.size() - memory_pool_type::total_reserved_size);
    }

} // namespace
