#include "utest.h"
#include "fst/traits.h"

namespace
{
    TEST_CASE("fst::traits", "[array]")
    {
        using list = fst::type_list<int, float, void*>;
        REQUIRE(list::size() == 3);
                                REQUIRE(fst::is_same_v<list::template type_at<0>, int>);
                        REQUIRE(fst::is_same_v<list::template type_at<1>, float>);
                        REQUIRE(fst::is_same_v<list::template type_at<2>, void*>);
                        //REQUIRE(fst::is_same_v<list::template type_at<3>, void*>);

                using sublist1 = list::template drop_first_n<1>;
                using sublist2 = list::template drop_first_n<2>;
                        REQUIRE(sublist1::size() == 2);
                        REQUIRE(sublist2::size() == 1);

                        //using firstn = fst::variadic_n_first_arg<2, int, float, void*>;

//using firstn = typename fst::variadic_n_first_arg<2, int, float, void*>::type;
                                        //using fsublist1 = list::template keep_first_n<1>;
                                                                //REQUIRE(fsublist1::size() == 1);


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
