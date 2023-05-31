#include "utest.h"
#include "fst/byte_vector.h"

namespace
{ 
   TEST_CASE("fst::byte_vector")
   {
       fst::byte_vector vec;
       vec.push_back<int16_t>(3);
       vec.push_back<char>(1);
       REQUIRE(vec.size() == 3);

       fst::small_byte_vector<32> svec;
       svec.push_back<int16_t>(3);
       svec.push_back<char>(1);
       REQUIRE(svec.size() == 3);

       svec.push_back(vec);
       REQUIRE(svec.size() == 6);

       REQUIRE(svec.as_offset<int16_t>(0) == 3);
       REQUIRE(svec.as_offset<char>(2) == 1);
       REQUIRE(svec.as_type_offset<int16_t, int16_t, char>() == 3);

       int16_t& k = svec.as_ref_offset<int16_t>(3);
       k = 23;
       REQUIRE(svec.as_offset<int16_t>(3) == 23);

    }
} // namespace
