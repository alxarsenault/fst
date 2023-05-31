#include "utest.h"
#include "fst/small_vector.h"

namespace
{
    TEST_CASE_T("fst::small_vector", "[vector]", //
        (fst::small_vector<int, 4>), //
        (fst::relocatable_small_vector<int, 4>) )
    {
        using vector_type = TestType;

        vector_type vec;
        REQUIRE(vec.has_stack_data());
        REQUIRE_FALSE(vec.has_allocated_data());
        REQUIRE_EQ(vec.capacity(), 4);

        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        vec.push_back(4);

        REQUIRE(vec.has_stack_data());
        REQUIRE_FALSE(vec.has_allocated_data());
        REQUIRE_EQ(vec.capacity(), 4);

        vec.push_back(5);
        REQUIRE_FALSE(vec.has_stack_data());
        REQUIRE(vec.has_allocated_data());
        REQUIRE_EQ(vec.capacity(), 8);

        vec.reset();
        REQUIRE(vec.has_stack_data());
        REQUIRE_FALSE(vec.has_allocated_data());
        REQUIRE_EQ(vec.capacity(), 4);

        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        vec.push_back(4);
        REQUIRE_EQ(vec.size(), 4);

        vector_type vec2 = vec;
        REQUIRE(vec2.has_stack_data());
        REQUIRE_FALSE(vec2.has_allocated_data());
        REQUIRE_EQ(vec2.capacity(), 4);
        REQUIRE_EQ(vec2.size(), 4);

        vec.push_back(5);
        vec2 = vec;

        REQUIRE_FALSE(vec2.has_stack_data());
        REQUIRE(vec2.has_allocated_data());
        REQUIRE_EQ(vec2.capacity(), 5);

        vec.push_back(6);

        vec = vec2;

        REQUIRE_FALSE(vec.has_stack_data());
        REQUIRE(vec.has_allocated_data());
        REQUIRE_EQ(vec.capacity(), 8);
    }
} // namespace
