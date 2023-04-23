#include "utest.h"
#include "fst/vector.h"
#include "fst/array.h"
#include "fst/small_vector.h"
#include "fst/stack_vector.h"

// clang-format off
#define FST_VECTOR_TEST_TYPE_LIST(TYPE, SIZE)      \
            (fst::vector<TYPE>),                   \
            (fst::stack_vector<TYPE, SIZE>),       \
            (fst::small_vector<TYPE, SIZE>)

#define FST_VECTOR_PUSH_BACK_TEST_TYPE_LIST       \
            FST_VECTOR_TEST_TYPE_LIST(char, 20),  \
            FST_VECTOR_TEST_TYPE_LIST(int, 20),   \
            FST_VECTOR_TEST_TYPE_LIST(float, 20), \
            FST_VECTOR_TEST_TYPE_LIST(double, 20)
// clang-format on

namespace
{
    template <class T, size_t Size>
    static inline fst::array<T, Size> buffer;

    template <size_t Size, class Vector>
    inline void init_vector(Vector&)
    {
        //if constexpr(fst::is_vector_range<Vector>::value || fst::is_fixed_vector_range<Vector>::value)
        //{
        //    vec = Vector(buffer<typename Vector::value_type, Size>);
        //    //vec = buffer<typename Vector::value_type, Size>;
        //}
    }

    TEST_CASE_T("fst::vector::push_back()", "[vector]", FST_VECTOR_PUSH_BACK_TEST_TYPE_LIST)
    {
        using vector_type = TestType;
        using value_type = typename vector_type::value_type;
        //static constexpr bool is_range_type = fst::is_vector_range<vector_type>::value || fst::is_fixed_vector_range<vector_type>::value;
        static constexpr bool is_small_vector = fst::is_small_vector<vector_type>::value;
        static constexpr bool is_stack_vector = fst::is_stack_vector<vector_type>::value;

        TEST_BLOCK("one")
        {
            vector_type vec;

            if constexpr (is_stack_vector)
            {
                init_vector<20>(vec);
                REQUIRE(vec.capacity());
                REQUIRE(vec.capacity() == vec.max_size());
                REQUIRE(vec.data());
            }
            else if constexpr (is_small_vector)
            {
                REQUIRE(vec.capacity());
                REQUIRE(vec.capacity() != vec.max_size());
                REQUIRE(vec.data());
            }
            else { REQUIRE(vec.data() == nullptr); }

            REQUIRE(vec.empty());

            value_type value{};
            vec.push_back(value);

            REQUIRE_EQ(vec.size(), 1);
            REQUIRE(value == vec.front());
            REQUIRE(value == vec.back());
        }

        TEST_BLOCK("multiple")
        {
            vector_type vec;

            if constexpr (is_stack_vector)
            {
                init_vector<20>(vec);
                REQUIRE(vec.capacity());
                REQUIRE(vec.capacity() == vec.max_size());
                REQUIRE(vec.data());
            }
            else if constexpr (is_small_vector)
            {
                REQUIRE(vec.capacity());
                REQUIRE(vec.capacity() != vec.max_size());
                REQUIRE(vec.data());
            }
            else { REQUIRE(vec.data() == nullptr); }

            REQUIRE(vec.empty());

            value_type value{};
            // const value_type orig = value;
            // value_type last = value;
            const size_t total = 20;

            for (size_t i = 0; i < total; ++i)
            {
                // last = value;
                vec.push_back(value);
            }

            REQUIRE_EQ(vec.size(), total);
            // REQUIRE(orig == vec.front());
            // REQUIRE(last == vec.back());
        }
    }
} // namespace
