#include "utest.h"
#include "fst/array.h"
#include "fst/profiler.h"
#include "fst/trace.h"

namespace
{
    struct dddefault_trace_section
    {
        static constexpr const char* name = "bn";
    };
    struct bingo
    {
        bingo()
        {
            a = 3;
            b = 32;
            //fst::trace(dddefault_trace_section{}, "L", a, b);
            //fst::trace("L", a, b);
        }
        int a, b;
    };

   TEST_CASE("fst::array::push_back()", "[array]")
   {
       fst::tracer<1, fst::simd_memory_category>()("FFFF");

       fst_trace(1, fst::default_trace_section, "BIngo");
        fst::debug()("KLLLLLLLLLLLL");
       fst::fst_dbg("JOHN");
        fst_trace_l1("AAA");
        fst_trace_l1("A", "KL");
        fst_trace_l1( );
        //fst::trace();
        //fst::trace(fst::default_trace_section{}, "A");
        bingo bb;

        fst_trace_l2("AAA");
        fst_trace_l3("AAA");
        fst_trace_l4("AAA");

        //fst::heap_array<float> hppp;
        ////hppp.get_memory_zone();

        //fst::fixed_heap_array<float, 2> hppp2;
        ////hppp2.get_memory_zone();
        ////fst::array<float, 2> a = {1.0f, 2.0f};
        ////REQUIRE(a[0] == 1.0f);
        ////fst::array<int, 2> a = { 1, 2 };
        //fst::array<float, 8> array;
        //fst::fixed_heap_array<float, 32> fharray;

        //fst::heap_array<float> harray;
        //REQUIRE(harray.data() == nullptr);
        //REQUIRE(harray.size() == 0);

        //fst::fixed_array_range<float, 8> rarray(array);
        //REQUIRE(rarray.size() == 8);

        //fst::array_range<float> rrarray(array);
        //REQUIRE(rrarray.size() == 8);
    }
} // namespace
