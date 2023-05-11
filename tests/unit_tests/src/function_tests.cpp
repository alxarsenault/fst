#include "utest.h"
#include "fst/function.h"
#include "fst/stream.h"

namespace
{
    static void call_fct(int a)
    {
        fst::print("AA", a);
    }

    struct abc
    {

        int bingo(int a)
        {
            fst::debug<fst::info_flags::function>()("AA", a);
            return a * 2;
        }
    };

    TEST_CASE("fst::function")
    {
        {
            abc a;
            fst::function<int(int)> f(&abc::bingo, &a);
            int g = f(78);
            REQUIRE(g == 2 * 78);
        }

        {
            fst::function<void(int)> f([](int a) { fst::print("A", a); });
            f(33);
        }

        {
            fst::function<int(int)> f([](int a) -> int { return a; });
            int h = f(33);
            REQUIRE(h == 33);
        }

        {
            fst::function<void(int)> f = &call_fct;
            f(34);
        }

        {
            fst::function<void(int)> f2;
            {
                int b = 35;
                fst::array<char, 1024> data;
                fst::function<void(int)> f([=](int a) { fst::print("A", a, b, data[0]); });
                f2 = f;
            }
            f2(33);
        }
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
