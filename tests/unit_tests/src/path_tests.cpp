#include "utest.h"
#include "fst/path.h"

namespace
{
    TEST_CASE("fst::path")
    {
        using path_type =
            fst::basic_path_view<char>;

        fst::string_view public_dir = "C:\\Users\\Public";
        REQUIRE(fst::filesystem::is_directory(public_dir));

        fst::string_view user_dir = fst::filesystem::get_parent_directory(public_dir);
        REQUIRE(user_dir == "C:\\Users");
        REQUIRE(fst::filesystem::is_directory(user_dir));

        fst::string_view root_dir = fst::filesystem::get_parent_directory(user_dir);
        REQUIRE(root_dir == "C:");
        REQUIRE(fst::filesystem::is_directory(root_dir));

         
        fst::string base_dir_path = fst::filesystem::join<fst::string>("C:", "Users", "aarsenault", "Desktop", "d0", "d1", "d2");
          fst::small_string<50> dir_path = fst::filesystem::join < fst::small_string<50> > (base_dir_path, "d3");

        REQUIRE(fst::filesystem::create_directories(dir_path));
        REQUIRE(fst::filesystem::is_directory(dir_path));

        REQUIRE(fst::filesystem::get_parent_directory(dir_path) == base_dir_path);
          REQUIRE(fst::filesystem::delete_directory(dir_path));

          for (auto n : fst::filesystem::path_range(dir_path))
          {
              fst::print(n);
          }
     }
} // namespace
