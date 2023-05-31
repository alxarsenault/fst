#pragma once

#include "fst/stream.h"
#include "fst/file.h"
#include "fst/array.h"
#include "fst/profiler.h"
#include "fst/trace.h"
#include "fst/file_view.h"
#include "fst/string.h"
#include "fst/vector.h"

#include "xpp_common.h"

namespace xpp
{
    class parser
    {
      public:
        fst::string _data;
        fst::vector<xpp::command> _commands;
        fst::string _filepath;

        fst::error_result parse_buffer(fst::string_view buffer, fst::string_view filepath);

        fst::error_result parse_file(const char* filepath)
        {
            fst::file_view file;
            if (fst::error_result err = file.open(filepath))
            {
                fst::print("Error: could not open file", err.message());
                return err;
            }
            return parse_buffer(file.str(), filepath);
        }
    };

    fst::vector<cmake_cache_value> parse_cache(fst::string_view data);
} // namespace xpp
