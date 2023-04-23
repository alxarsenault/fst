#include "utest.h"
#include "fst/geometry.h"

namespace
{
    TEST_CASE_T("fst::geometry rect conversion", "geometry", int, unsigned int, float, double)
    {
        using value_type = TestType;

#define DECLARE_RECT(NAME, X, Y, W, H) \
    struct NAME                        \
    {                                  \
        value_type X, Y, W, H;         \
    }

        // clang-format off
        struct valid_os { struct Point { value_type x, y; } origin; struct Size { value_type width, height; } size; };
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, valid_os>);

        struct wrong_os { struct Point { value_type y, x; } origin; struct Size { value_type width, height; } size; };
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, wrong_os>);
        // clang-format on

        DECLARE_RECT(valid_xywh, x, y, width, height);
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, valid_xywh>);

        DECLARE_RECT(wrong_xywh, y, x, width, height);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, wrong_xywh>);

        DECLARE_RECT(valid_XYWH, X, Y, Width, Height);
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, valid_XYWH>);

        DECLARE_RECT(wrong_XYWH, Y, X, Width, Height);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, wrong_XYWH>);

        DECLARE_RECT(valid_ltrb, left, top, right, bottom);
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, valid_ltrb>);

        DECLARE_RECT(wrong_ltrb_1, left, top, bottom, right);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, wrong_ltrb_1>);

        DECLARE_RECT(wrong_ltrb_2, left, bottom, top, right);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, wrong_ltrb_2>);

        // Can construct rect from other rect type.
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, fst::rect<float>>);
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, fst::rect<double>>);
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, fst::rect<int>>);
        REQUIRE(fst::is_constructible_v<fst::rect<value_type>, fst::rect<unsigned int>>);

        // Can't construct rect from point.
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::point<float>>);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::point<double>>);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::point<int>>);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::point<unsigned int>>);

        // Can't construct rect from size.
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::size<float>>);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::size<double>>);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::size<int>>);
        REQUIRE_FALSE(fst::is_constructible_v<fst::rect<value_type>, fst::size<unsigned int>>);
    }

    TEST_CASE_T("fst::geometry point conversion", "geometry", int, unsigned int, float, double)
    {
        using value_type = TestType;

        REQUIRE(fst::is_constructible_v<fst::point<value_type>, fst::point<float>>);
        REQUIRE(fst::is_constructible_v<fst::point<value_type>, fst::point<double>>);
        REQUIRE(fst::is_constructible_v<fst::point<value_type>, fst::point<int>>);
        REQUIRE(fst::is_constructible_v<fst::point<value_type>, fst::point<unsigned int>>);

        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::rect<float>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::rect<double>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::rect<int>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::rect<unsigned int>>);

        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::size<float>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::size<double>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::size<int>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, fst::size<unsigned int>>);

#define DECLARE_POINT(NAME, X, Y) \
    struct NAME                   \
    {                             \
        value_type X, Y;          \
    }

        DECLARE_POINT(valid_xy, x, y);
        REQUIRE(fst::is_constructible_v<fst::point<value_type>, valid_xy>);

        DECLARE_POINT(wrong_xy, y, x);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, wrong_xy>);

        DECLARE_POINT(valid_XY, X, Y);
        REQUIRE(fst::is_constructible_v<fst::point<value_type>, valid_XY>);

        DECLARE_POINT(wrong_XY, Y, X);
        EXPECT_FALSE(fst::is_constructible_v<fst::point<value_type>, wrong_XY>);
    }

    TEST_CASE_T("fst::geometry size conversion", "geometry", int, unsigned int, float, double)
    {
        using value_type = TestType;

        REQUIRE(fst::is_constructible_v<fst::size<value_type>, fst::size<float>>);
        REQUIRE(fst::is_constructible_v<fst::size<value_type>, fst::size<double>>);
        REQUIRE(fst::is_constructible_v<fst::size<value_type>, fst::size<int>>);
        REQUIRE(fst::is_constructible_v<fst::size<value_type>, fst::size<unsigned int>>);

        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::rect<float>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::rect<double>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::rect<int>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::rect<unsigned int>>);

        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::point<float>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::point<double>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::point<int>>);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, fst::point<unsigned int>>);

#define DECLARE_SIZE(NAME, W, H) \
    struct NAME                  \
    {                            \
        value_type W, H;         \
    }

        DECLARE_SIZE(valid_wh, width, height);
        REQUIRE(fst::is_constructible_v<fst::size<value_type>, valid_wh>);

        DECLARE_SIZE(wrong_wh, height, width);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, wrong_wh>);

        DECLARE_SIZE(valid_WH, Width, Height);
        REQUIRE(fst::is_constructible_v<fst::size<value_type>, valid_WH>);

        DECLARE_SIZE(wrong_WH, Height, Width);
        EXPECT_FALSE(fst::is_constructible_v<fst::size<value_type>, wrong_WH>);
    }

    TEST_CASE("fst::geometry transform", "geometry")
    {
        {
            fst::rect<float> r = { 0, 0, 20, 20 };
            fst::transform<float> f = fst::transform<float>::translation({ 10, 10 });

            EXPECT_EQ(r.position * f, fst::point<float>(10, 10));

            f += { 5.0f, 6.0f };
            EXPECT_EQ(r.position * f, fst::point<float>(15, 16));
        }

        {
            fst::rect<float> r = { 5, 6, 20, 20 };
            fst::transform<float> f = fst::transform<float>::translation({ 10, 20 });
            fst::point<float> pt = r.position * f;
            EXPECT_EQ(pt, fst::point<float>(15, 26));
        }

        {
            fst::rect<float> r = { 5, 6, 10, 20 };
            fst::transform<float> f = fst::transform<float>::scale({ 2, 3 });
            EXPECT_EQ(r.position * f, fst::point<float>(10, 18));
        }

        {
            fst::rect<float> r = { 5, 6, 10, 20 };
            fst::transform<float> f = fst::transform<float>::translation({ 10, 20 }) * fst::transform<float>::scale({ 2, 3 });
            EXPECT_EQ(r.position * f, fst::point<float>(5 * 2 + 10, 6 * 3 + 20));
        }

        {
            fst::rect<float> r = { 5, 6, 10, 20 };
            fst::transform<float> f = fst::transform<float>::scale({ 2, 3 }) * fst::transform<float>::translation({ 10, 20 });
            EXPECT_EQ(r.position * f, fst::point<float>((5 + 10) * 2, (6 + 20) * 3));

            f.translated({ 5, 7 });
            EXPECT_EQ(r.position * f, fst::point<float>((5 + 10 + 5) * 2, (6 + 20 + 7) * 3));
        }

        {
            fst::rect<float> r = { 5, 6, 10, 20 };
            fst::transform<float> f = fst::transform<float>::scale({ 2, 3 }) * fst::transform<float>::translation({ 10, 20 });
            EXPECT_EQ(r.position * f, fst::point<float>((5 + 10) * 2, (6 + 20) * 3));

            f += { 5.0f, 7.0f };
            EXPECT_EQ(r.position * f, fst::point<float>((5 + 10 + 5) * 2, (6 + 20 + 7) * 3));
        }

        {
            fst::rect<double> r = { 0, 0, 10, 10 };
            fst::transform<double> f = fst::transform<double>::rotation(fst::pi<double>, r.bottom_right());
            fst::point<double> tl = r.position * f;
            fst::point<double> tr = r.top_right() * f;
            fst::point<double> bl = r.bottom_left() * f;
            fst::point<double> br = r.bottom_right() * f;
            EXPECT_EQ(tl, fst::point<double>(20, 20));
            EXPECT_EQ(tr, fst::point<double>(10, 20));
            EXPECT_EQ(bl, fst::point<double>(20, 10));
            EXPECT_EQ(br, r.bottom_right());
        }

        {
            fst::rect<double> r = { 0, 0, 10, 10 };
            fst::transform<double> f = fst::transform<double>::rotation(fst::pi<double>, r.bottom_right());
            fst::quad<double> q = r * f;
            EXPECT_EQ(q.top_left, fst::point<double>(20, 20));
            EXPECT_EQ(q.top_right, fst::point<double>(10, 20));
            EXPECT_EQ(q.bottom_left, fst::point<double>(20, 10));
            EXPECT_EQ(q.bottom_right, r.bottom_right());
        }

        {
            fst::rect<double> r = { 0, 0, 10, 10 };
            fst::quad<double> q = r;
            fst::transform<double> f = fst::transform<double>::rotation(fst::pi<double>, r.bottom_right());
            fst::quad<double> tq = q * f;
            EXPECT_EQ(tq.top_left, fst::point<double>(20, 20));
            EXPECT_EQ(tq.top_right, fst::point<double>(10, 20));
            EXPECT_EQ(tq.bottom_left, fst::point<double>(20, 10));
            EXPECT_EQ(tq.bottom_right, r.bottom_right());
        }
    }
} // namespace.
