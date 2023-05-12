#include "utest.h"
#include "fst/string.h"
#include "fst/sutils.h"
#include <iostream>
#include <iomanip>
namespace
{
    TEST_CASE("fst::format", "[core]")
    {
        {
            REQUIRE(fst::required_aligned_size(1, 10, 8, false) == 17);
            REQUIRE(fst::required_aligned_size(1, 10, 8, true) == 24);
            REQUIRE(fst::required_aligned_size(1, 10, 16, true) == 32);

                        REQUIRE(fst::required_aligned_size(8, 10, 8, true) == 16);
                        REQUIRE(fst::required_aligned_size(8, 10, 8, false) == 10);

        }
        char buffer[16];
        
        char* cptr = fst::align(&buffer[1], 8);
REQUIRE(fst::is_aligned(cptr, 8));        

         cptr = fst::align(&buffer[1], 16);
REQUIRE(fst::is_aligned(cptr, 16));        

         cptr = fst::align_range(buffer, buffer + 16, 8, 16);
         REQUIRE(cptr);
         REQUIRE(fst::is_aligned(cptr, 16));        

//size_t a = ;         cptr = fst::align(&buffer[1], 16);

        REQUIRE(fst::align(8, 16) == 16);
        REQUIRE(fst::align(16, 16) == 16);
        //char buffer[3] = {0, 0, 0};
         //*(uint16_t*)(buffer) = fst::digits_to_99[89];
         //fst::string s(buffer, 2);
        REQUIRE(fst::clip_alignment(8) == fst::default_alignment);
        fst::get_digit_pair(buffer, 78);
        fst::print("----------------------", buffer[0], buffer[1]);


        //for(int i =0; i < 100; i++)
        //    {
        //    uint16_t j = ((const uint16_t*)fst::digits_0_to_99)[i];
        //    //fst::print(j);
        //    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << j << ", ";
        //}
    }
} // namespace
