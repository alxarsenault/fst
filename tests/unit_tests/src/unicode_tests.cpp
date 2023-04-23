#include "utest.h"
#include "fst/unicode.h"

#if FST_HAS_CHAR8_T
#define CPP20_EXPECT_TRUE(X) EXPECT_TRUE(X)
#define CPP20_EXPECT_EQ(A, B) EXPECT_EQ(A, B)
#define IF_CPP20(X) X
#else
#define CPP20_EXPECT_TRUE(X)
#define CPP20_EXPECT_EQ(A, B)
#define IF_CPP20(X)
#endif // FST_HAS_CHAR8_T

namespace common
{
    inline constexpr const char* c_string();
    inline constexpr const char16_t* u16_string();
    inline constexpr const char32_t* u32_string();
    inline constexpr const wchar_t* w_string();

#if FST_HAS_CHAR8_T
    inline constexpr const char8_t* u8_string();
#endif
} // namespace common.

TEST_CASE("CHINA")
{
    fst::string s = "的";
    REQUIRE_EQ(s.size(), 3);
    REQUIRE_EQ(fst::utf8_length(s.c_str(), s.size()), 1);

    s += s;
    REQUIRE_EQ(s.size(), 6);
    REQUIRE_EQ(fst::utf8_length(s.c_str(), s.size()), 2);

    s = "香港增补字符集";
    REQUIRE_EQ(s.size(), 21);
    REQUIRE_EQ(fst::utf8_length(s.c_str(), s.size()), 7);
}

TEST_CASE("utf", "traits")
{
    // is_utf_char_type.
    EXPECT_TRUE(fst::is_utf_char_type<char>::value);
    EXPECT_TRUE(fst::is_utf_char_type<const char>::value);
    CPP20_EXPECT_TRUE(fst::is_utf_char_type<char8_t>::value);
    CPP20_EXPECT_TRUE(fst::is_utf_char_type<const char8_t>::value);
    EXPECT_TRUE(fst::is_utf_char_type<char16_t>::value);
    EXPECT_TRUE(fst::is_utf_char_type<const char16_t>::value);
    EXPECT_TRUE(fst::is_utf_char_type<char32_t>::value);
    EXPECT_TRUE(fst::is_utf_char_type<const char32_t>::value);
    EXPECT_TRUE(fst::is_utf_char_type<wchar_t>::value);
    EXPECT_TRUE(fst::is_utf_char_type<const wchar_t>::value);

    EXPECT_FALSE(fst::is_utf_char_type<char&>::value);
    EXPECT_FALSE(fst::is_utf_char_type<const char&>::value);
    EXPECT_FALSE(fst::is_utf_char_type<char*>::value);
    EXPECT_FALSE(fst::is_utf_char_type<unsigned char>::value);
    EXPECT_FALSE(fst::is_utf_char_type<double>::value);

    // is_utf_char_size.
    EXPECT_TRUE(fst::is_utf_char_size<sizeof(char)>::value);
    EXPECT_TRUE(fst::is_utf_char_size<sizeof(char16_t)>::value);
    EXPECT_TRUE(fst::is_utf_char_size<sizeof(char32_t)>::value);
    EXPECT_TRUE(fst::is_utf_char_size<sizeof(wchar_t)>::value);
    EXPECT_FALSE(fst::is_utf_char_size<3>::value);
    EXPECT_FALSE(fst::is_utf_char_size<8>::value);

    // utf_encoding_of.
    EXPECT_TRUE(fst::utf_encoding_of<char>::value == fst::char_encoding::utf8);
    CPP20_EXPECT_TRUE(fst::utf_encoding_of<char8_t>::value == fst::char_encoding::utf8);
    EXPECT_TRUE(fst::utf_encoding_of<char16_t>::value == fst::char_encoding::utf16);
    EXPECT_TRUE(fst::utf_encoding_of<char32_t>::value == fst::char_encoding::utf32);
    EXPECT_TRUE(fst::utf_encoding_of<wchar_t>::value == (sizeof(wchar_t) == sizeof(char16_t) ? fst::char_encoding::utf16 : fst::char_encoding::utf32));

    // utf_encoding_size.
    EXPECT_TRUE(fst::utf_encoding_size<fst::char_encoding::utf8>::value == sizeof(char));
    EXPECT_TRUE(fst::utf_encoding_size<fst::char_encoding::utf16>::value == sizeof(char16_t));
    EXPECT_TRUE(fst::utf_encoding_size<fst::char_encoding::utf32>::value == sizeof(char32_t));

    // string_char_type.
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::string>, char>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::string_view>, char>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char*>, char>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<char*>, char>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char(&)[]>, char>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const fst::string&>, char>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::string&&>, char>) );
    EXPECT_FALSE((fst::is_same_v<fst::string_char_type_t<fst::string*>, char>) );
    EXPECT_FALSE((fst::is_same_v<fst::string_char_type_t<char>, char>) );
    EXPECT_FALSE((fst::is_same_v<fst::string_char_type_t<char&>, char>) );

    CPP20_EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::u8string>, char8_t>) );
    CPP20_EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::u8string_view>, char8_t>) );
    CPP20_EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char8_t*>, char8_t>) );
    CPP20_EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char8_t(&)[]>, char8_t>) );

    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::u16string>, char16_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::u16string_view>, char16_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char16_t*>, char16_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char16_t(&)[]>, char16_t>) );

    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::u32string>, char32_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::u32string_view>, char32_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char32_t*>, char32_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const char32_t(&)[]>, char32_t>) );

    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::wstring>, wchar_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<fst::wstring_view>, wchar_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const wchar_t*>, wchar_t>) );
    EXPECT_TRUE((fst::is_same_v<fst::string_char_type_t<const wchar_t(&)[]>, wchar_t>) );
    //
}

TEST_CASE("utf utf_cvt_size")
{
    fst::string s = common::c_string();
    IF_CPP20(fst::u8string s8 = common::u8_string());
    fst::u16string s16 = common::u16_string();
    fst::u32string s32 = common::u32_string();
    fst::wstring sw = common::w_string();

    EXPECT_EQ(fst::utf_cvt_size<char>(s), s.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<char8_t>(s), s8.size());
    EXPECT_EQ(fst::utf_cvt_size<char16_t>(s), s16.size());
    EXPECT_EQ(fst::utf_cvt_size<char32_t>(s), s32.size());
    EXPECT_EQ(fst::utf_cvt_size<wchar_t>(s), sw.size());

    CPP20_EXPECT_EQ(fst::utf_cvt_size<char>(s8), s.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<char8_t>(s8), s8.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<char16_t>(s8), s16.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<char32_t>(s8), s32.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<wchar_t>(s8), sw.size());

    EXPECT_EQ(fst::utf_cvt_size<char>(s16), s.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<char8_t>(s16), s8.size());
    EXPECT_EQ(fst::utf_cvt_size<char16_t>(s16), s16.size());
    EXPECT_EQ(fst::utf_cvt_size<char32_t>(s16), s32.size());
    EXPECT_EQ(fst::utf_cvt_size<wchar_t>(s16), sw.size());

    EXPECT_EQ(fst::utf_cvt_size<char>(s32), s.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<char8_t>(s32), s8.size());
    EXPECT_EQ(fst::utf_cvt_size<char16_t>(s32), s16.size());
    EXPECT_EQ(fst::utf_cvt_size<char32_t>(s32), s32.size());
    EXPECT_EQ(fst::utf_cvt_size<wchar_t>(s32), sw.size());

    EXPECT_EQ(fst::utf_cvt_size<char>(sw), s.size());
    CPP20_EXPECT_EQ(fst::utf_cvt_size<char8_t>(sw), s8.size());
    EXPECT_EQ(fst::utf_cvt_size<char16_t>(sw), s16.size());
    EXPECT_EQ(fst::utf_cvt_size<char32_t>(sw), s32.size());
    EXPECT_EQ(fst::utf_cvt_size<wchar_t>(sw), sw.size());
}
//
TEST_CASE("utf length")
{
    fst::string s = common::c_string();
    IF_CPP20(fst::u8string s8 = common::u8_string());
    fst::u16string s16 = common::u16_string();
    fst::u32string s32 = common::u32_string();

    //fst::print(fst::string_view(s16));

    EXPECT_EQ(fst::utf_length(s), s32.size());
    CPP20_EXPECT_EQ(fst::utf_length(s8), s32.size());
    EXPECT_EQ(fst::utf_length(s16), s32.size());
}

TEST_CASE("utf utf_cvt")
{
    {
        fst::string in = common::c_string();
        fst::string s = fst::utf_cvt(in);
        IF_CPP20(fst::u8string s8 = fst::utf_cvt(in));
        fst::u16string s16 = fst::utf_cvt(in);
        fst::u32string s32 = fst::utf_cvt(in);
        fst::wstring sw = fst::utf_cvt(in);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }

#if FST_HAS_CHAR8_T

    {
        fst::u8string in = common::u8_string();
        fst::string s = fst::utf_cvt(in);
        fst::u8string s8 = fst::utf_cvt(in);
        fst::u16string s16 = fst::utf_cvt(in);
        fst::u32string s32 = fst::utf_cvt(in);
        fst::wstring sw = fst::utf_cvt(in);

        EXPECT_TRUE(s == common::c_string());
        EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }
#endif

    {
        fst::u16string in = common::u16_string();
        fst::string s = fst::utf_cvt(in);
        IF_CPP20(fst::u8string s8 = fst::utf_cvt(in));
        fst::u16string s16 = fst::utf_cvt(in);
        fst::u32string s32 = fst::utf_cvt(in);
        fst::wstring sw = fst::utf_cvt(in);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }

    {
        fst::u32string in = common::u32_string();
        fst::string s = fst::utf_cvt(in);
        IF_CPP20(fst::u8string s8 = fst::utf_cvt(in));
        fst::u16string s16 = fst::utf_cvt(in);
        fst::u32string s32 = fst::utf_cvt(in);
        fst::wstring sw = fst::utf_cvt(in);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }

    {
        fst::wstring in = common::w_string();
        fst::string s = fst::utf_cvt(in);
        IF_CPP20(fst::u8string s8 = fst::utf_cvt(in));
        fst::u16string s16 = fst::utf_cvt(in);
        fst::u32string s32 = fst::utf_cvt(in);
        fst::wstring sw = fst::utf_cvt(in);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }
}

TEST_CASE("utf impl_copy")
{
    {
        fst::string in = common::c_string();
        fst::string s;
        IF_CPP20(fst::u8string s8);
        fst::u16string s16;
        fst::u32string s32;
        fst::wstring sw;

        fst::utf_append_to(in, s);

        //fst::copy(in, fst::back_inserter(s));

        IF_CPP20(fst::utf_append_to(in, s8));
        fst::utf_append_to(in, s16);
        fst::utf_append_to(in, s32);
        fst::utf_append_to(in, sw);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }

#if FST_HAS_CHAR8_T

    {
        fst::u8string in = common::u8_string();
        fst::string s;
        fst::u8string s8;
        fst::u16string s16;
        fst::u32string s32;
        fst::wstring sw;

        fst::utf_append_to(in, s);
        fst::utf_append_to(in, s8);
        fst::utf_append_to(in, s16);
        fst::utf_append_to(in, s32);
        fst::utf_append_to(in, sw);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }
#endif

    {
        fst::u16string in = common::u16_string();
        fst::string s;
        IF_CPP20(fst::u8string s8);
        fst::u16string s16;
        fst::u32string s32;
        fst::wstring sw;

        fst::utf_append_to(in, s);
        IF_CPP20(fst::utf_append_to(in, s8));
        fst::utf_append_to(in, s16);
        fst::utf_append_to(in, s32);
        fst::utf_append_to(in, sw);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }

    {
        fst::u32string in = common::u32_string();
        fst::string s;
        IF_CPP20(fst::u8string s8);
        fst::u16string s16;
        fst::u32string s32;
        fst::wstring sw;

        fst::utf_append_to(in, s);
        IF_CPP20(fst::utf_append_to(in, s8));
        fst::utf_append_to(in, s16);
        fst::utf_append_to(in, s32);
        fst::utf_append_to(in, sw);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }

    {
        fst::wstring in = common::w_string();
        fst::string s;
        IF_CPP20(fst::u8string s8);
        fst::u16string s16;
        fst::u32string s32;
        fst::wstring sw;

        fst::utf_append_to(in, s);
        IF_CPP20(fst::utf_append_to(in, s8));
        fst::utf_append_to(in, s16);
        fst::utf_append_to(in, s32);
        fst::utf_append_to(in, sw);

        EXPECT_TRUE(s == common::c_string());
        CPP20_EXPECT_TRUE(s8 == common::u8_string());
        EXPECT_TRUE(s16 == common::u16_string());
        EXPECT_TRUE(s32 == common::u32_string());
        EXPECT_TRUE(sw == common::w_string());
    }
}
//
TEST_CASE("utf u8_iterator")
{

    {
        fst::string s = common::c_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::string_view c : fst::utf::iterate(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

#if FST_HAS_CHAR8_T
    {
        fst::u8string s = common::u8_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::u8string_view c : fst::utf::iterate(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }
#endif

    {
        fst::u16string s = common::u16_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::u16string_view c : fst::utf::iterate(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::wstring s = common::w_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::wstring_view c : fst::utf::iterate(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::string s = common::c_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::string_view c : fst::utf::iterate_as<char>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::u16string s = common::u16_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::string_view c : fst::utf::iterate_as<char>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::u32string s = common::u32_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::string_view c : fst::utf::iterate_as<char>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::u32string s = common::u32_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::u16string_view c : fst::utf::iterate_as<char16_t>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::u32string s = common::u32_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::u32string_view c : fst::utf::iterate_as<char32_t>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::u32string s = common::u32_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::wstring_view c : fst::utf::iterate_as<wchar_t>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::u16string s = common::u16_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::u16string_view c : fst::utf::iterate_as<char16_t>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::string s = common::c_string();
        size_t length = fst::utf_length(s);

        size_t count = 0;
        for (fst::u16string_view c : fst::utf::iterate_as<char16_t>(s))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::string s = common::c_string();
        size_t count = 0;

        for (fst::string_view c : fst::utf::iterate_as<char>(fst::string_view(s).substr(0, 500)))
        {
            (void) c;
            count++;
        }
        EXPECT_EQ(count, fst::utf_length(fst::string_view(s).substr(0, 500)));
    }

    {
        fst::string s = common::c_string();
        size_t length = fst::utf_length(s);
        size_t count = 0;

        for (auto it = fst::utf::iterator(s.begin()); it != fst::utf::iterator(s.end()); ++it)
        {
            count++;
        }
        EXPECT_EQ(count, length);
    }

    {
        fst::u16string s = common::u16_string();
        size_t length = fst::utf_length(s);
        size_t count = 0;

        for (auto it = fst::utf::iterator(s.begin()); it != fst::utf::iterator(s.end()); ++it)
        {
            count++;
        }
        EXPECT_EQ(count, length);
    }
    //
}

TEST_CASE("utf-string-view string_view")
{
    {
        EXPECT_EQ(fst::utf_string_view(common::c_string()).count(), fst::utf_string_view(common::w_string()).count());
        EXPECT_EQ(fst::utf_string_view(common::c_string()).count(), fst::utf_string_view(common::w_string()).to_utf32().size());
    }

    {
        fst::wstring a = fst::utf::to_wide(fst::utf_string_view(common::c_string()));
        fst::wstring aa = fst::utf::to_wide(common::c_string());
        fst::wstring b = common::w_string();
        EXPECT_EQ(a.size(), b.size());
        EXPECT_EQ(aa.size(), b.size());
    }

    {
        fst::string a = fst::utf::to_utf8(common::c_string());
        fst::string b = fst::utf::to_utf8(common::w_string());
        EXPECT_EQ(a.size(), b.size());
    }

    {
        fst::utf_string_view a(common::c_string());

        EXPECT_NE(a.u8cstr(), nullptr);
        EXPECT_EQ(a.u16cstr(), nullptr);
        EXPECT_EQ(a.u32cstr(), nullptr);
        EXPECT_EQ(a.wcstr(), nullptr);

        EXPECT_FALSE(a.u8str().empty());
        EXPECT_FALSE(a.u16str().empty());
        EXPECT_FALSE(a.u32str().empty());
        EXPECT_FALSE(a.wstr().empty());

        EXPECT_FALSE(a.u8view().empty());
        EXPECT_TRUE(a.u16view().empty());
        EXPECT_TRUE(a.u32view().empty());
        EXPECT_TRUE(a.wview().empty());

        EXPECT_EQ(a.u8str().size(), a.size());
        EXPECT_EQ(a.u8view().size(), a.size());
    }

    {
        fst::utf_string_view a(common::w_string());

        EXPECT_EQ(a.u8cstr(), nullptr);
        EXPECT_NE(a.wcstr(), nullptr);

        EXPECT_FALSE(a.u8str().empty());
        EXPECT_FALSE(a.wstr().empty());

        EXPECT_TRUE(a.u8view().empty());
        EXPECT_FALSE(a.wview().empty());

        EXPECT_EQ(a.wstr().size(), a.size());
        EXPECT_EQ(a.wview().size(), a.size());
    }
}

#define UTF_CONCAT1(_X, _Y) _X##_Y
#define UTF_CONCAT(_X, _Y) UTF_CONCAT1(_X, _Y)

#define UTF_STRING_TYPE(prefix) UTF_CONCAT(prefix, RAW_STRING)

// clang-format off
#define RAW_STRING "Original by Markus Kuhn, adapted for HTML by Martin Dürst.\n"\
"\n"\
"UTF-8 encoded sample plain-text file\n"\
"‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\n"\
"\n"\
"Markus Kuhn [ˈmaʳkʊs kuːn] <mkuhn@acm.org> — 1999-08-20\n"\
"\n"\
"\n"\
"The ASCII compatible UTF-8 encoding of ISO 10646 and Unicode\n"\
"plain-text files is defined in RFC 2279 and in ISO 10646-1 Annex R.\n"\
"\n"\
"\n"\
"Using Unicode/UTF-8, you can write in emails and source code things such as\n"\
"\n"\
"Mathematics and Sciences:\n"\
"\n"\
"  ∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i), ∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β),\n"\
"\n"\
"  ℕ ⊆ ℕ₀ ⊂ ℤ ⊂ ℚ ⊂ ℝ ⊂ ℂ, ⊥ < a ≠ b ≡ c ≤ d ≪ ⊤ ⇒ (A ⇔ B),\n"\
"\n"\
"  2H₂ + O₂ ⇌ 2H₂O, R = 4.7 kΩ, ⌀ 200 mm\n"\
"\n"\
"Linguistics and dictionaries:\n"\
"\n"\
"  ði ıntəˈnæʃənəl fəˈnɛtık əsoʊsiˈeıʃn\n"\
"  Y [ˈʏpsilɔn], Yen [jɛn], Yoga [ˈjoːgɑ]\n"\
"\n"\
"APL:\n"\
"\n"\
"  ((V⍳V)=⍳⍴V)/V←,V    ⌷←⍳→⍴∆∇⊃‾⍎⍕⌈\n"\
"\n"\
"Nicer typography in plain text files:\n"\
"\n"\
"  ╔══════════════════════════════════════════╗\n"\
"  ║                                          ║\n"\
"  ║   • ‘single’ and “double” quotes         ║\n"\
"  ║                                          ║\n"\
"  ║   • Curly apostrophes: “We’ve been here” ║\n"\
"  ║                                          ║\n"\
"  ║   • Latin-1 apostrophe and accents: '´`  ║\n"\
"  ║                                          ║\n"\
"  ║   • ‚deutsche‘ „Anführungszeichen“       ║\n"\
"  ║                                          ║\n"\
"  ║   • †, ‡, ‰, •, 3–4, —, −5/+5, ™, …      ║\n"\
"  ║                                          ║\n"\
"  ║   • ASCII safety test: 1lI|, 0OD, 8B     ║\n"\
"  ║                      ╭─────────╮         ║\n"\
"  ║   • the euro symbol: │ 14.95 € │         ║\n"\
"  ║                      ╰─────────╯         ║\n"\
"  ╚══════════════════════════════════════════╝\n"\
"\n"\
"Greek (in Polytonic):\n"\
"\n"\
"  The Greek anthem:\n"\
"\n"\
"  Σὲ γνωρίζω ἀπὸ τὴν κόψη\n"\
"  τοῦ σπαθιοῦ τὴν τρομερή,\n"\
"  σὲ γνωρίζω ἀπὸ τὴν ὄψη\n"\
"  ποὺ μὲ βία μετράει τὴ γῆ.\n"\
"\n"\
"  ᾿Απ᾿ τὰ κόκκαλα βγαλμένη\n"\
"  τῶν ῾Ελλήνων τὰ ἱερά\n"\
"  καὶ σὰν πρῶτα ἀνδρειωμένη\n"\
"  χαῖρε, ὦ χαῖρε, ᾿Ελευθεριά!\n"\
"\n"\
"  From a speech of Demosthenes in the 4th century BC:\n"\
"\n"\
"  Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν, ὦ ἄνδρες ᾿Αθηναῖοι,\n"\
"  ὅταν τ᾿ εἰς τὰ πράγματα ἀποβλέψω καὶ ὅταν πρὸς τοὺς\n"\
"  λόγους οὓς ἀκούω· τοὺς μὲν γὰρ λόγους περὶ τοῦ\n"\
"  τιμωρήσασθαι Φίλιππον ὁρῶ γιγνομένους, τὰ δὲ πράγματ᾿\n"\
"  εἰς τοῦτο προήκοντα,  ὥσθ᾿ ὅπως μὴ πεισόμεθ᾿ αὐτοὶ\n"\
"  πρότερον κακῶς σκέψασθαι δέον. οὐδέν οὖν ἄλλο μοι δοκοῦσιν\n"\
"  οἱ τὰ τοιαῦτα λέγοντες ἢ τὴν ὑπόθεσιν, περὶ ἧς βουλεύεσθαι,\n"\
"  οὐχὶ τὴν οὖσαν παριστάντες ὑμῖν ἁμαρτάνειν. ἐγὼ δέ, ὅτι μέν\n"\
"  ποτ᾿ ἐξῆν τῇ πόλει καὶ τὰ αὑτῆς ἔχειν ἀσφαλῶς καὶ Φίλιππον\n"\
"  τιμωρήσασθαι, καὶ μάλ᾿ ἀκριβῶς οἶδα· ἐπ᾿ ἐμοῦ γάρ, οὐ πάλαι\n"\
"  γέγονεν ταῦτ᾿ ἀμφότερα· νῦν μέντοι πέπεισμαι τοῦθ᾿ ἱκανὸν\n"\
"  προλαβεῖν ἡμῖν εἶναι τὴν πρώτην, ὅπως τοὺς συμμάχους\n"\
"  σώσομεν. ἐὰν γὰρ τοῦτο βεβαίως ὑπάρξῃ, τότε καὶ περὶ τοῦ\n"\
"  τίνα τιμωρήσεταί τις καὶ ὃν τρόπον ἐξέσται σκοπεῖν· πρὶν δὲ\n"\
"  τὴν ἀρχὴν ὀρθῶς ὑποθέσθαι, μάταιον ἡγοῦμαι περὶ τῆς\n"\
"  τελευτῆς ὁντινοῦν ποιεῖσθαι λόγον.\n"\
"\n"\
"  Δημοσθένους, Γ´ ᾿Ολυνθιακὸς\n"\
"\n"\
"Georgian:\n"\
"\n"\
"  From a Unicode conference invitation:\n"\
"\n"\
"  გთხოვთ ახლავე გაიაროთ რეგისტრაცია Unicode-ის მეათე საერთაშორისო\n"\
"  კონფერენციაზე დასასწრებად, რომელიც გაიმართება 10-12 მარტს,\n"\
"  ქ. მაინცში, გერმანიაში. კონფერენცია შეჰკრებს ერთად მსოფლიოს\n"\
"  ექსპერტებს ისეთ დარგებში როგორიცაა ინტერნეტი და Unicode-ი,\n"\
"  ინტერნაციონალიზაცია და ლოკალიზაცია, Unicode-ის გამოყენება\n"\
"  ოპერაციულ სისტემებსა, და გამოყენებით პროგრამებში, შრიფტებში,\n"\
"  ტექსტების დამუშავებასა და მრავალენოვან კომპიუტერულ სისტემებში.\n"\
"\n"\
"Russian:\n"\
"\n"\
"  From a Unicode conference invitation:\n"\
"\n"\
"  Зарегистрируйтесь сейчас на Десятую Международную Конференцию по\n"\
"  Unicode, которая состоится 10-12 марта 1997 года в Майнце в Германии.\n"\
"  Конференция соберет широкий круг экспертов по  вопросам глобального\n"\
"  Интернета и Unicode, локализации и интернационализации, воплощению и\n"\
"  применению Unicode в различных операционных системах и программных\n"\
"  приложениях, шрифтах, верстке и многоязычных компьютерных системах.\n"\
"\n"\
"Thai (UCS Level 2):\n"\
"\n"\
"  Excerpt from a poetry on The Romance of The Three Kingdoms (a Chinese\n"\
"  classic 'San Gua'):\n"\
"\n"\
"  [----------------------------|------------------------]\n"\
"    ๏ แผ่นดินฮั่นเสื่อมโทรมแสนสังเวช  พระปกเกศกองบู๊กู้ขึ้นใหม่\n"\
"  สิบสองกษัตริย์ก่อนหน้าแลถัดไป       สององค์ไซร้โง่เขลาเบาปัญญา\n"\
"    ทรงนับถือขันทีเป็นที่พึ่ง           บ้านเมืองจึงวิปริตเป็นนักหนา\n"\
"  โฮจิ๋นเรียกทัพทั่วหัวเมืองมา         หมายจะฆ่ามดชั่วตัวสำคัญ\n"\
"    เหมือนขับไสไล่เสือจากเคหา      รับหมาป่าเข้ามาเลยอาสัญ\n"\
"  ฝ่ายอ้องอุ้นยุแยกให้แตกกัน          ใช้สาวนั้นเป็นชนวนชื่นชวนใจ\n"\
"    พลันลิฉุยกุยกีกลับก่อเหตุ          ช่างอาเพศจริงหนาฟ้าร้องไห้\n"\
"  ต้องรบราฆ่าฟันจนบรรลัย           ฤๅหาใครค้ำชูกู้บรรลังก์ ฯ\n"\
"\n"\
"  (The above is a two-column text. If combining characters are handled\n"\
"  correctly, the lines of the second column should be aligned with the\n"\
"  | character above.)\n"\
"\n"\
"Ethiopian:\n"\
"\n"\
"  Proverbs in the Amharic language:\n"\
"\n"\
"  ሰማይ አይታረስ ንጉሥ አይከሰስ።\n"\
"  ብላ ካለኝ እንደአባቴ በቆመጠኝ።\n"\
"  ጌጥ ያለቤቱ ቁምጥና ነው።\n"\
"  ደሀ በሕልሙ ቅቤ ባይጠጣ ንጣት በገደለው።\n"\
"  የአፍ ወለምታ በቅቤ አይታሽም።\n"\
"  አይጥ በበላ ዳዋ ተመታ።\n"\
"  ሲተረጉሙ ይደረግሙ።\n"\
"  ቀስ በቀስ፥ ዕንቁላል በእግሩ ይሄዳል።\n"\
"  ድር ቢያብር አንበሳ ያስር።\n"\
"  ሰው እንደቤቱ እንጅ እንደ ጉረቤቱ አይተዳደርም።\n"\
"  እግዜር የከፈተውን ጉሮሮ ሳይዘጋው አይድርም።\n"\
"  የጎረቤት ሌባ፥ ቢያዩት ይስቅ ባያዩት ያጠልቅ።\n"\
"  ሥራ ከመፍታት ልጄን ላፋታት።\n"\
"  ዓባይ ማደሪያ የለው፥ ግንድ ይዞ ይዞራል።\n"\
"  የእስላም አገሩ መካ የአሞራ አገሩ ዋርካ።\n"\
"  ተንጋሎ ቢተፉ ተመልሶ ባፉ።\n"\
"  ወዳጅህ ማር ቢሆን ጨርስህ አትላሰው።\n"\
"  እግርህን በፍራሽህ ልክ ዘርጋ።\n"\
"\n"\
"Runes:\n"\
"\n"\
"  ᚻᛖ ᚳᚹᚫᚦ ᚦᚫᛏ ᚻᛖ ᛒᚢᛞᛖ ᚩᚾ ᚦᚫᛗ ᛚᚪᚾᛞᛖ ᚾᚩᚱᚦᚹᛖᚪᚱᛞᚢᛗ ᚹᛁᚦ ᚦᚪ ᚹᛖᛥᚫ\n"\
"\n"\
"  (Old English, which transcribed into Latin reads 'He cwaeth that he\n"\
"  bude thaem lande northweardum with tha Westsae.' and means 'He said\n"\
"  that he lived in the northern land near the Western Sea.')\n"\
"\n"\
"Braille:\n"\
"\n"\
"  ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌\n"\
"\n"\
"  ⡍⠜⠇⠑⠹ ⠺⠁⠎ ⠙⠑⠁⠙⠒ ⠞⠕ ⠃⠑⠛⠔ ⠺⠊⠹⠲ ⡹⠻⠑ ⠊⠎ ⠝⠕ ⠙⠳⠃⠞\n"\
"  ⠱⠁⠞⠑⠧⠻ ⠁⠃⠳⠞ ⠹⠁⠞⠲ ⡹⠑ ⠗⠑⠛⠊⠌⠻ ⠕⠋ ⠙⠊⠎ ⠃⠥⠗⠊⠁⠇ ⠺⠁⠎\n"\
"  ⠎⠊⠛⠝⠫ ⠃⠹ ⠹⠑ ⠊⠇⠻⠛⠹⠍⠁⠝⠂ ⠹⠑ ⠊⠇⠻⠅⠂ ⠹⠑ ⠥⠝⠙⠻⠞⠁⠅⠻⠂\n"\
"  ⠁⠝⠙ ⠹⠑ ⠡⠊⠑⠋ ⠍⠳⠗⠝⠻⠲ ⡎⠊⠗⠕⠕⠛⠑ ⠎⠊⠛⠝⠫ ⠊⠞⠲ ⡁⠝⠙\n"\
"  ⡎⠊⠗⠕⠕⠛⠑⠰⠎ ⠝⠁⠍⠑ ⠺⠁⠎ ⠛⠕⠕⠙ ⠥⠏⠕⠝ ⠰⡡⠁⠝⠛⠑⠂ ⠋⠕⠗ ⠁⠝⠹⠹⠔⠛ ⠙⠑\n"\
"  ⠡⠕⠎⠑ ⠞⠕ ⠏⠥⠞ ⠙⠊⠎ ⠙⠁⠝⠙ ⠞⠕⠲\n"\
"\n"\
"  ⡕⠇⠙ ⡍⠜⠇⠑⠹ ⠺⠁⠎ ⠁⠎ ⠙⠑⠁⠙ ⠁⠎ ⠁ ⠙⠕⠕⠗⠤⠝⠁⠊⠇⠲\n"\
"\n"\
"  ⡍⠔⠙⠖ ⡊ ⠙⠕⠝⠰⠞ ⠍⠑⠁⠝ ⠞⠕ ⠎⠁⠹ ⠹⠁⠞ ⡊ ⠅⠝⠪⠂ ⠕⠋ ⠍⠹\n"\
"  ⠪⠝ ⠅⠝⠪⠇⠫⠛⠑⠂ ⠱⠁⠞ ⠹⠻⠑ ⠊⠎ ⠏⠜⠞⠊⠊⠥⠇⠜⠇⠹ ⠙⠑⠁⠙ ⠁⠃⠳⠞\n"\
"  ⠁ ⠙⠕⠕⠗⠤⠝⠁⠊⠇⠲ ⡊ ⠍⠊⠣⠞ ⠙⠁⠧⠑ ⠃⠑⠲ ⠔⠊⠇⠔⠫⠂ ⠍⠹⠎⠑⠇⠋⠂ ⠞⠕\n"\
"  ⠗⠑⠛⠜⠙ ⠁ ⠊⠕⠋⠋⠔⠤⠝⠁⠊⠇ ⠁⠎ ⠹⠑ ⠙⠑⠁⠙⠑⠌ ⠏⠊⠑⠊⠑ ⠕⠋ ⠊⠗⠕⠝⠍⠕⠝⠛⠻⠹\n"\
"  ⠔ ⠹⠑ ⠞⠗⠁⠙⠑⠲ ⡃⠥⠞ ⠹⠑ ⠺⠊⠎⠙⠕⠍ ⠕⠋ ⠳⠗ ⠁⠝⠊⠑⠌⠕⠗⠎\n"\
"  ⠊⠎ ⠔ ⠹⠑ ⠎⠊⠍⠊⠇⠑⠆ ⠁⠝⠙ ⠍⠹ ⠥⠝⠙⠁⠇⠇⠪⠫ ⠙⠁⠝⠙⠎\n"\
"  ⠩⠁⠇⠇ ⠝⠕⠞ ⠙⠊⠌⠥⠗⠃ ⠊⠞⠂ ⠕⠗ ⠹⠑ ⡊⠳⠝⠞⠗⠹⠰⠎ ⠙⠕⠝⠑ ⠋⠕⠗⠲ ⡹⠳\n"\
"  ⠺⠊⠇⠇ ⠹⠻⠑⠋⠕⠗⠑ ⠏⠻⠍⠊⠞ ⠍⠑ ⠞⠕ ⠗⠑⠏⠑⠁⠞⠂ ⠑⠍⠏⠙⠁⠞⠊⠊⠁⠇⠇⠹⠂ ⠹⠁⠞\n"\
"  ⡍⠜⠇⠑⠹ ⠺⠁⠎ ⠁⠎ ⠙⠑⠁⠙ ⠁⠎ ⠁ ⠙⠕⠕⠗⠤⠝⠁⠊⠇⠲\n"\
"\n"\
"  (The first couple of paragraphs of \"A Christmas Carol\" by Dickens)\n"\
"\n"\
"Compact font selection example text:\n"\
"\n"\
"  ABCDEFGHIJKLMNOPQRSTUVWXYZ /0123456789\n"\
"  abcdefghijklmnopqrstuvwxyz £©µÀÆÖÞßéöÿ\n"\
"  –—‘“”„†•…‰™œŠŸž€ ΑΒΓΔΩαβγδω АБВГДабвгд\n"\
"  ∀∂∈ℝ∧∪≡∞ ↑↗↨↻⇣ ┐┼╔╘░►☺♀ ﬁ�⑀₂ἠḂӥẄɐː⍎אԱა\n"\
"\n"\
"Greetings in various languages:\n"\
"\n"\
"  Hello world, Καλημέρα κόσμε, コンニチハ\n"\
"\n"\
"Box drawing alignment tests:                                          █\n"\
"                                                                      ▉\n"\
"  ╔══╦══╗  ┌──┬──┐  ╭──┬──╮  ╭──┬──╮  ┏━━┳━━┓  ┎┒┏┑   ╷  ╻ ┏┯┓ ┌┰┐    ▊ ╱╲╱╲╳╳╳\n"\
"  ║┌─╨─┐║  │╔═╧═╗│  │╒═╪═╕│  │╓─╁─╖│  ┃┌─╂─┐┃  ┗╃╄┙  ╶┼╴╺╋╸┠┼┨ ┝╋┥    ▋ ╲╱╲╱╳╳╳\n"\
"  ║│╲ ╱│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╿ │┃  ┍╅╆┓   ╵  ╹ ┗┷┛ └┸┘    ▌ ╱╲╱╲╳╳╳\n"\
"  ╠╡ ╳ ╞╣  ├╢   ╟┤  ├┼─┼─┼┤  ├╫─╂─╫┤  ┣┿╾┼╼┿┫  ┕┛┖┚     ┌┄┄┐ ╎ ┏┅┅┓ ┋ ▍ ╲╱╲╱╳╳╳\n"\
"  ║│╱ ╲│║  │║   ║│  ││ │ ││  │║ ┃ ║│  ┃│ ╽ │┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▎\n"\
"  ║└─╥─┘║  │╚═╤═╝│  │╘═╪═╛│  │╙─╀─╜│  ┃└─╂─┘┃  ░░▒▒▓▓██ ┊  ┆ ╎ ╏  ┇ ┋ ▏\n"\
"  ╚══╩══╝  └──┴──┘  ╰──┴──╯  ╰──┴──╯  ┗━━┻━━┛           └╌╌┘ ╎ ┗╍╍┛ ┋  ▁▂▃▄▅▆▇█\n"
// clang-format on

namespace common
{
    inline constexpr const char* c_string()
    {
        return RAW_STRING;
    }
    inline constexpr const char16_t* u16_string()
    {
        return UTF_STRING_TYPE(u);
    }
    inline constexpr const char32_t* u32_string()
    {
        return UTF_STRING_TYPE(U);
    }
    inline constexpr const wchar_t* w_string()
    {
        return UTF_STRING_TYPE(L);
    }

#if FST_HAS_CHAR8_T
    inline constexpr const char8_t* u8_string()
    {
        return UTF_STRING_TYPE(u8);
    }
#endif
} // namespace common.
