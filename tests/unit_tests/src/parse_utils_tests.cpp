#include "utest.h"
#include "fst/parse_utils.h"
#include "fst/array.h"
#include "fst/file_view.h"

namespace
{
    constexpr fst::array<fst::string_view, 9> builtin_types = {
        //
        "bool", "char", "var", "int", "uint", "float", "void", "byte", "string" //
    };

    constexpr fst::array<fst::string_view, 20> keywords = { //
        "typeof", "do", "const", "true", "false", //
        "null", "default", "switch", "enum", "class", //
        "import", "for", "in", "return", "while", //
        "if", "else", "continue", "break", "case"
    };

    TEST_CASE("fst::parse_utils::script", "[core]") {
    //
        fst::file_view file;
        REQUIRE(file.open(FST_TEST_RESOURCES_DIRECTORY "/script/script.sp"));
        //fst::print(file.str());
    }

    TEST_CASE("fst::parse_utils", "[core]")
    {
        TEST_BLOCK("ltrim")
        {
            {
                fst::string_view s = "\n alex";
                REQUIRE(fst::ltrim(s) == "alex");
            }
        }

        TEST_BLOCK("rtrim")
        {
            {
                fst::string_view s = "alex  \t\n";
                REQUIRE(fst::rtrim(s) == "alex");
            }
        }

        TEST_BLOCK("trim")
        {
            {
                fst::string_view s = "  \talex  \t\n";
                REQUIRE(fst::trim(s) == "alex");
            }
        }
    }
} // namespace
