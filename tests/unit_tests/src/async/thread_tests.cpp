#include "utest.h"
#include "fst/async/thread.h"
#include "fst/string.h"
#include "fst/unicode.h"
namespace
{

    void callback() {}
    TEST_CASE("fst::thread", "[async]")
    {
        {
            fst::string a = "alex:perter";
            auto c = fst::strsplit(a, ':');

            //fst::print(c.front(), c.back());
        }
        {
            fst::string s = "B\nJ\nbanana\nalex";
            char noendline[] = { 'B', '\n', 'J', '\n', 'b', 'a', 'n', 'a', 'n', 'a', '\n', 'a', 'l', 'e', 'x', 'X' };

            fst::string_view sv(noendline, sizeof(noendline) - 1);
            fst::array<fst::string_view, 4> arr = { "B", "J", "banana", "alex" };

            {
                auto it = arr.begin();
                for (fst::string_view line : fst::line_range(s))
                {
                    REQUIRE(line == *it++);
                }
            }

            {
                auto it = arr.begin();
                for (fst::string_view line : fst::line_range(sv))
                {
                    REQUIRE(line == *it++);
                }
            }
        }

        {
            fst::wstring s = L"B\nJ\nbanana\nalex";
            fst::array<fst::wstring_view, 4> arr = { L"B", L"J", L"banana", L"alex" };
            wchar_t noendline[] = { 'B', '\n', 'J', '\n', 'b', 'a', 'n', 'a', 'n', 'a', '\n', 'a', 'l', 'e', 'x', 'X' };
            fst::wstring_view sv(noendline, sizeof(noendline) / sizeof(wchar_t) - 1);

            {
                auto it = arr.begin();
                for (fst::wstring_view line : fst::line_range(s))
                {
                    REQUIRE(line == *it++);
                }
            }

            {
                auto it = arr.begin();
                for (fst::wstring_view line : fst::line_range(sv))
                {
                    REQUIRE(line == *it++);
                }
            }
        }

        fst::async::thread t;

        struct banana
        {

            ~banana() { /*fst::print("vababa");*/ }
        };

        //banana bbb;
        fst::string s = "Aex";
        //fst::print(s);
        fst::async::thread t2([=] { /*fst::print("BINFO", s);*/ });

        fst::async::thread t3(&callback);
    }
} // namespace
