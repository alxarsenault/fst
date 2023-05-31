#pragma once

#include "fst/stream.h"
#include "fst/file.h"
#include "fst/array.h"
#include "fst/profiler.h"
#include "fst/trace.h"
#include "fst/file_view.h"
#include "fst/lua.h"
#include "fst/string.h"
#include "fst/vector.h"
#include "fst/path.h"
#include "xpp_common.h"
#include "xpp_parser.h"

namespace xpp
{

    //
    class generator
    {
      public:
        fst::error_result process_import(const xpp::command& cmd, size_t& findex, fst::memory_view<fst::string_view> include_directories);
        fst::error_result process_trace(const xpp::command& cmd, size_t& findex);
        fst::error_result process_trait(const xpp::command& cmd, size_t& findex);

        fst::error_result process_script(const xpp::command& cmd, size_t& findex, fst::memory_view<fst::string_view> script_directories);

        fst::error_result process_code(const xpp::command& cmd, size_t& findex, fst::memory_view<fst::string_view> include_directories);

        fst::error_result init_script_engine(const xpp::parser& xparser, fst::string_view cache_dir, fst::memory_view<xpp::cmake_cache_value> cache);

        fst::error_result reset_script_engine(const xpp::parser& xparser, int depth);

        fst::error_result preprocess(const xpp::parser& xparser, fst::string_view cache_dir);

        fst::error_result process(
            const xpp::parser& xparser, fst::memory_view<fst::string_view> include_directories, fst::memory_view<fst::string_view> script_directories, int depth = 0);

        fst::error_result generate(const char* filepath);

        //fst::string _data;
        fst::string _output_data;
        //fst::vector<xpp::command> _commands;
        fst::lua::context _sengine;
        const xpp::parser* _parser = nullptr;
        fst::string _shared_cache_path;
        //fst::string _filepath;
    };
} // namespace xpp
