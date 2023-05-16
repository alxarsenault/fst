#include "utest.h"
#include "fst/binary_file.h"

namespace
{
    TEST_CASE("fst::binary_file::ds()", "[array]")
    {
        fst::string value_1 = "alex";
        fst::string value_2 = "alexandre";

        using writer_type = fst::data_file::writer<fst::default_memory_category, fst::default_memory_zone>;

        writer_type subbin;

        REQUIRE(!subbin.add_chunk("value_01", fst::byte_view((fst::byte*) value_1.data(), value_1.size()), false));
        REQUIRE(!subbin.add_chunk("value_02", fst::byte_view((fst::byte*) value_2.data(), value_2.size()), true));

        writer_type bin;

        REQUIRE(!bin.add_chunk("value_1", fst::byte_view((fst::byte*) value_1.data(), value_1.size()), false));
        REQUIRE(!bin.add_chunk("value_2", fst::byte_view((fst::byte*) value_2.data(), value_2.size()), true));
        REQUIRE(!bin.add_chunk("value_3", subbin, true));

        fst::vector<fst::byte> buffer;
        buffer.resize(bin.write_size());
        REQUIRE(!bin.write_to_buffer(buffer));

        {
            fst::data_file::reader<fst::default_memory_category, fst::default_memory_zone> loader;
            fst::error_result er = loader.load(buffer);
            REQUIRE(!er);

            REQUIRE(loader.contains("value_1"));
            REQUIRE(loader.get_data_string("value_1") == value_1);

            REQUIRE(loader.contains("value_2"));
            REQUIRE(loader.get_data_string("value_2") == value_2);

            REQUIRE(loader.contains("value_3"));
            fst::data_file::reader<fst::default_memory_category, fst::default_memory_zone> subloader;
            subloader.load(loader.get_data("value_3"));

            REQUIRE(subloader.contains("value_01"));
            REQUIRE(subloader.get_data_string("value_01") == value_1);

            REQUIRE(subloader.contains("value_02"));
            REQUIRE(subloader.get_data_string("value_02") == value_2);
        }
    }

    TEST_CASE("fst::binary_file::push_back()", "[array]")
    {
        const char* fpath = FST_TEST_OUTPUT_RESOURCES_DIRECTORY "/hh.bin";
        const char* fpath2 = FST_TEST_OUTPUT_RESOURCES_DIRECTORY "/hh2.bin";
        const char* fpath3 = FST_TEST_OUTPUT_RESOURCES_DIRECTORY "/hh3.bin";

        fst::string value_1 = "alex";
        fst::string value_2 = "alexandre";
        fst::string value_3 = "alexandre";
        fst::string value_4 = "k";

        int g = 89;
        {
            fst::data_file::writer<fst::default_memory_category, fst::default_memory_zone> bin;

            REQUIRE(!bin.add_chunk("value_1", fst::byte_view((fst::byte*) value_1.data(), value_1.size()), false));
            REQUIRE(!bin.add_chunk("value_2", fst::byte_view((fst::byte*) value_2.data(), value_2.size()), true));
            REQUIRE(!bin.add_chunk("value_3", fst::byte_view((fst::byte*) value_3.data(), value_3.size()), true));
            REQUIRE(!bin.add_chunk("value_4", fst::byte_view((fst::byte*) value_4.data(), value_4.size()), true));
            REQUIRE(!bin.add_chunk("abcdefgh", fst::byte_view((fst::byte*) &g, sizeof(g)), true));

            //
            REQUIRE(!bin.write_to_file(fpath));

            //
            fst::vector<fst::byte> buffer;
            buffer.resize(bin.write_size());
            REQUIRE(!bin.write_to_buffer(buffer));
            REQUIRE(fst::write_to_file(fpath2, fst::open_mode::write | fst::open_mode::create_always, buffer.data(), buffer.size()));

            //
            fst::vector<fst::byte> s;
            auto bstream = fst::byte_stream(s);
            REQUIRE(!bin.write_to_stream(bstream));
            REQUIRE(fst::write_to_file(fpath3, fst::open_mode::write | fst::open_mode::create_always, s.data(), s.size()));

            {
                fst::data_file::reader<fst::default_memory_category, fst::default_memory_zone> loader;
                fst::error_result er = loader.load(s);
                REQUIRE(!er);

                fst::memory_view<fst::string_view> names = loader.get_names();
                REQUIRE_EQ(names.size(), 5);

                REQUIRE(loader.contains("value_1"));
                REQUIRE(loader.get_data_string("value_1") == value_1);

                REQUIRE(loader.contains("value_2"));
                REQUIRE(loader.get_data_string("value_2") == value_2);

                REQUIRE(loader.contains("value_3"));
                REQUIRE(loader.get_data_string("value_3") == value_3);

                REQUIRE(loader.contains("value_4"));
                REQUIRE(loader.get_data_string("value_4") == value_4);

                REQUIRE(loader.get_data("abcdefgh").as<int>() == 89);
                REQUIRE(loader.get_data("abcdefgh").size() == 4);
            }
        }

        {
            auto load_file = [&](const char* filepath)
            {
                fst::data_file::file_reader<fst::default_memory_category, fst::default_memory_zone> loader;
                fst::error_result er = loader.load(filepath);
                REQUIRE(!er);

                fst::memory_view<fst::string_view> names = loader.get_names();
                REQUIRE_EQ(names.size(), 5);

                REQUIRE(loader.contains("value_1"));
                REQUIRE(loader.get_data_string("value_1") == value_1);

                REQUIRE(loader.contains("value_2"));
                REQUIRE(loader.get_data_string("value_2") == value_2);

                REQUIRE(loader.contains("value_3"));
                REQUIRE(loader.get_data_string("value_3") == value_3);

                REQUIRE(loader.contains("value_4"));
                REQUIRE(loader.get_data_string("value_4") == value_4);

                REQUIRE(loader.get_data("abcdefgh").as<int>() == 89);
            };

            load_file(fpath);
            load_file(fpath2);
            load_file(fpath3);
        }
    }
} // namespace
