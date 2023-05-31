#include "xpp_generator.h"

namespace xpp
{
    inline fst::string remove_all_double_a(fst::string_view data)
    {
        fst::string tmp;

        size_t lindex = 0;
        for (size_t i = 0; i < data.size() - 1; i++)
        {
            if (data[i] == '@' && data[i + 1] == '@')
            {
                tmp.append(data.substr(lindex, i - lindex));
                //
                lindex = i + 1;
            }

            //if (!(data[i] == '@' && data[i + 1] == '@')) { tmp.push_back(data[i]); }
        }

        tmp.append(data.substr(lindex, data.size() - lindex));
        //tmp.push_back(data.back());
        return tmp;
    }
    inline fst::string get_imported_file(fst::string_view fname, fst::memory_view<fst::string_view> include_directories, fst::string_view ext)
    {
        for (fst::string_view dir : include_directories)
        {
            //fst::string path = fst::filesystem::join<fst::string>(dir, fname) + ".template";
            fst::string path = dir;
            path.push_back('/');
            path += fname;
            //path += ".template";
            path.append(ext);
            if (fst::filesystem::is_file(path)) { return path; }
        }

        return fst::string();
    }

    fst::error_result generator::process_import(const xpp::command& cmd, size_t& findex, fst::memory_view<fst::string_view> include_directories)
    {
        fst::string pp = get_imported_file(cmd.content, include_directories, ".template");

        if (pp.empty())
        {
            fst::print("Error: could not find import file", cmd.content, "at line", index_to_line(_parser->_data, cmd.range.left));
            return fst::status_code::invalid_argument;
        }

        fst::file_view ifile;
        if (fst::error_result err = ifile.open(pp.c_str()))
        {
            fst::print("Error: could not open import file", pp);
            return err;
        }

        _output_data.append(ifile.str());
        ifile.close();

        findex = cmd.range.right;

        return fst::status_code::success;
    }

    fst::error_result generator::process_trace(const xpp::command& cmd, size_t& findex)
    {
        _output_data.append("fst_trace_l1(\"");
        _output_data.append(cmd.content);
        _output_data.append("\");");

        findex = cmd.range.right;

        return fst::status_code::success;
    }

    fst::error_result generator::process_trait(const xpp::command& cmd, size_t& findex)
    {
        //fst::print("TRAIT :", cmd.function, "--", cmd.params);
        findex = cmd.range.right + 1;
        return fst::status_code::success;
    }

    fst::error_result generator::process_script(const xpp::command& cmd, size_t& findex, fst::memory_view<fst::string_view> script_directories)
    {
        fst::string pp = get_imported_file(cmd.content, script_directories, ".lua");

        if (pp.empty())
        {
            fst::print("Error: could not find script file", cmd.content, "at line", index_to_line(_parser->_data, cmd.range.left));
            return fst::status_code::invalid_argument;
        }

        fst::file_view file;
        if (fst::error_result err = file.open(pp.c_str()))
        {
            fst::print("Error: could not open import file", pp);
            return err;
        }

        fst::string content;
        content.resize(file.size());
        fst::memcpy(content.data(), file.data(), file.size());

        //_output_data.append(ifile.str());
        file.close();

        if (fst::error_result err = _sengine.state().run_string(content))
        {
            fst::print("Error: could not run code");
            return err;
        }

        lua_getglobal(_sengine.state(), "output_string");
        fst::string_view sss = lua_tostring(_sengine.state(), -1);
        if (!sss.empty()) { _output_data.append(sss); }

        lua_pushstring(_sengine.state(), "");
        lua_setglobal(_sengine.state(), "output_string");
        lua_pop(_sengine.state(), 1);

        findex = cmd.range.right + 1;

        return fst::status_code::success;
    }

    fst::error_result generator::process_code(const xpp::command& cmd, size_t& findex, fst::memory_view<fst::string_view> include_directories)
    {
        //fst::script::engine engine;

        //lua_pushstring(engine.state(), "");
        //lua_setglobal(engine.state(), "output_string");

        //if (fst::error_result err = engine.run_string("output_string = '';\nfunction add_code(a)\n print(a);\noutput_string = output_string .. a;\n end"))

        if (fst::error_result err = _sengine.state().run_string(cmd.content))
        {
            fst::print("Error: could not run code");
            return err;
        }

        lua_getglobal(_sengine.state(), "output_string");
        fst::string_view sss = lua_tostring(_sengine.state(), -1);
        if (!sss.empty()) { _output_data.append(sss); }

        lua_pushstring(_sengine.state(), "");
        lua_setglobal(_sengine.state(), "output_string");
        lua_pop(_sengine.state(), 1);

        /*if (fst::error_result err = _sengine.run_string("output_string = '';"))
            {
                fst::print("Error: could not run code");
                return err;
            }*/

        //

        findex = cmd.range.right + 1;

        return fst::status_code::success;
    }

    fst::error_result generator::init_script_engine(const xpp::parser& xparser, fst::string_view cache_dir, fst::memory_view<xpp::cmake_cache_value> cache)
    {
        _shared_cache_path = fst::filesystem::join<fst::string>(cache_dir, "xpp_shared.lua");
        lua_pushstring(_sengine.state(), "");
        lua_setglobal(_sengine.state(), "output_string");

        lua_pushstring(_sengine.state(), _shared_cache_path.c_str());
        lua_setglobal(_sengine.state(), "shared_cache_path");

        if (fst::error_result err = _sengine.state().run_file(RESOURCES_DIRECTORY "/xpp.lua"))
        {
            fst::print("Error: could not run code");
            return err;
        }

        lua_getglobal(_sengine.state(), "context");

        lua_pushstring(_sengine.state(), xparser._filepath.c_str());
        lua_setfield(_sengine.state(), -2, "path");

        lua_pushstring(_sengine.state(), xparser._data.c_str());
        lua_setfield(_sengine.state(), -2, "content");

        lua_pop(_sengine.state(), 1);

        lua_getglobal(_sengine.state(), "cache");

        for (const auto& c : cache)
        {

            lua_pushstring(_sengine.state(), c.value.c_str());
            lua_setfield(_sengine.state(), -2, c.name.c_str());
        }

        lua_pop(_sengine.state(), 1);

        return fst::status_code::success;
    }

    fst::error_result generator::reset_script_engine(const xpp::parser& xparser, int depth)
    {
        lua_pushstring(_sengine.state(), "");
        lua_setglobal(_sengine.state(), "output_string");

        lua_getglobal(_sengine.state(), "context");

        lua_pushstring(_sengine.state(), xparser._data.c_str());
        lua_setfield(_sengine.state(), -2, "content");

        lua_pushinteger(_sengine.state(), depth);
        lua_setfield(_sengine.state(), -2, "depth");

        lua_pop(_sengine.state(), 1);

        return fst::status_code::success;
    }

    fst::vector<xpp::cmake_cache_value> dump_cache(const char* filepath)
    {
        fst::file_view file;
        if (file.open(filepath))
        {
            fst::vector<xpp::cmake_cache_value> cache = xpp::parse_cache(file.str());

            for (const auto& c : cache)
            {
                //fst::print<fst::stream_detail::equal_t>(c.name, c.value);
            }

            return cache;
        }
        return {};
    }

    fst::error_result generator::preprocess(const xpp::parser& xparser, fst::string_view cache_dir)
    {
        fst::string cache_file_path = fst::filesystem::join<fst::string>(cache_dir, "CMakeCache.txt");
        fst::vector<xpp::cmake_cache_value> cache = dump_cache(cache_file_path.c_str());
        fst::error_result err = init_script_engine(xparser, cache_dir, cache);
        return err;
    }

    fst::error_result generator::process(
        const xpp::parser& xparser, fst::memory_view<fst::string_view> include_directories, fst::memory_view<fst::string_view> script_directories, int depth)
    {
        fst::string_view data(xparser._data);
        const fst::vector<xpp::command>& commands = xparser._commands;
        _output_data.reserve(data.size());
        _parser = &xparser;

        size_t findex = 0;
        for (size_t i = 0; i < commands.size(); i++)
        {
            const xpp::command& cmd = commands[i];
            xpp::index_range range{ findex, cmd.range.left };
            _output_data.append(range.substr(data));

            fst::error_result err;

            switch (cmd.tok)
            {
            case xpp::token::import: err = process_import(cmd, findex, include_directories); break;
            case xpp::token::code: err = process_code(cmd, findex, include_directories); break;
            case xpp::token::trace: err = process_trace(cmd, findex); break;
            case xpp::token::trait: err = process_trait(cmd, findex); break;
            case xpp::token::script: err = process_script(cmd, findex, script_directories); break;
            default: findex = cmd.range.right + 1;
            }

            if (err) { return err; }
        }

        _output_data.append(data.substr(findex));

        /*fst::string tmp;
        for (size_t i = 0; i < _output_data.size() -1; i++)
        {
            if (!(_output_data[i] == '@' && _output_data[i + 1] == '@')) { tmp.push_back(_output_data[i]); }
        }
        tmp.push_back(_output_data.back());*/

        fst::string tmp = remove_all_double_a(_output_data);
        xpp::parser nparser;
        nparser.parse_buffer(tmp, xparser._filepath);
        if (!nparser._commands.empty())
        {
            //nparser._data.

            /*fst::string tmp;
            tmp.reserve(nparser._data.size());

            fst::string_view data(nparser._data);
            size_t sz = nparser._data.size();
            for (size_t i = 0; i < sz - 1; i++)
            {
                if (!(data[i] == '@' && data[i + 1] == '@'))
                {
                    tmp.push_back(data[i]);
                }
            }*/

            //tmp.push_back(data.back());
            //nparser._data = tmp;

            //fst::print("DSDSDS", tmp);
            //size_t double_q = nparser._data.find("@@");
            //while (double_q != fst::string_view::npos)
            //{
            //    /*tmp.append(fst::string_view(nparser._data).substr());
            //    nparser._data.erase(double_q, 1);
            //    double_q = fst::string_view(nparser._data).find("@@", double_q);*/
            //}

            _output_data.clear();
            reset_script_engine(nparser, depth + 1);
            return process(nparser, include_directories, script_directories, depth + 1);
        }

        return fst::status_code::success;
    }

    fst::error_result generator::generate(const char* filepath)
    {
        if (fst::error_result err = _sengine.state().run_string("save_shared_cache()"))
        {
            fst::print("Error: could not run code");
            return err;
        }

        fst::file output_file;

        if (fst::error_result err = output_file.open(filepath, fst::open_mode::create_always | fst::open_mode::write))
        {
            fst::print("Error: could not open file", err.message());
            return err;
        }

        output_file.write(_output_data.data(), _output_data.size());
        output_file.close();

        return fst::status_code::success;
    }

} // namespace xpp
