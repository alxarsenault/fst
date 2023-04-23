//#include "utest.h"
//#include "fst/color.h"
//
//namespace
//{
//    TEST_CASE("fst::color")
//    {
//        {
//            fst::color c = 0xF1A0B1EE;
//            EXPECT_EQ(c.red(), c[0]);
//            EXPECT_EQ(c.green(), c[1]);
//            EXPECT_EQ(c.blue(), c[2]);
//            EXPECT_EQ(c.alpha(), c[3]);
//        }
//
//        fst::color c = 0xFF00FFFF;
//
//        EXPECT_EQ(c.red(), 0xFF);
//        EXPECT_EQ(c.green(), 0);
//        EXPECT_EQ(c.blue(), 0xFF);
//        EXPECT_EQ(c.alpha(), 0xFF);
//
//        EXPECT_EQ(c.f_red(), 1.0f);
//        EXPECT_EQ(c.f_green(), 0.0f);
//        EXPECT_EQ(c.f_blue(), 1.0f);
//        EXPECT_EQ(c.f_alpha(), 1.0f);
//
//        EXPECT_EQ(c.red<float>(), 1.0f);
//        EXPECT_EQ(c.green<float>(), 0.0f);
//        EXPECT_EQ(c.blue<float>(), 1.0f);
//        EXPECT_EQ(c.alpha<float>(), 1.0f);
//    }
//} // namespace
