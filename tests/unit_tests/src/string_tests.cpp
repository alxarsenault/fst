#include "utest.h"
#include "fst/string.h"

namespace
{
    TEST_CASE("fst::string")
    {
        fst::string s = "abcdefghijklmnopqrstuvwxyz";
        REQUIRE_EQ(s.size(), 26);
        REQUIRE(s.is_small());
        fst::print(fst::string::small_capacity, fst::string::small_array_size);
    }

    TEST_CASE("fst::wstring")
    {
        fst::wstring s = L"abcdefghijklmnopqrstuvwxyz";
        REQUIRE_EQ(s.size(), 26);
        //REQUIRE(s.is_big());
        //fst::print("wstring", fst::wstring::small_capacity, fst::wstring::small_array_size);
    }

    TEST_CASE("fst::u32string")
    {
        fst::u32string s = U"abcdefghijklmnopqrstuvwxyz";
        REQUIRE_EQ(s.size(), 26);
        //REQUIRE(s.is_big());
        //fst::print(fst::u32string::small_capacity, fst::u32string::small_array_size);
    }
} // namespace
