#include "xpp_parser.h"

namespace xpp
{
    inline xpp::index_range find_inside_param(fst::string_view str)
    {
        size_t left = xpp::invalid_index;
        for (size_t i = 0; i < str.size(); i++)
        {
            if (str[i] == '(')
            {
                left = i;
                break;
            }
        }

        if (left == xpp::invalid_index) { return xpp::index_range::create_invalid(); }

        size_t right = left;
        for (size_t i = str.size() - 1; i > left; i--)
        {
            if (str[i] == ')')
            {
                right = i;
                break;
            }
        }

        if (right == left) { return xpp::index_range::create_invalid(); }

        return xpp::index_range{ left, right };
    }

    inline bool has_text_on_line_before(fst::string_view data, size_t start)
    {
        for (size_t i = start + 1; i > 0; i--)
        {
            if (data[i - 1] == '\n') { return false; }

            if (!fst::is_one_of(data[i - 1], ' ', '\t', '\v', '\r')) { return true; }
        }
        return false;
    }

    inline xpp::token_result find_next_token(fst::string_view data, size_t start = 0)
    {
        size_t token_index = 0;
        size_t str_index = xpp::invalid_index;

        for (size_t i = 0; i < xpp::tokens.size(); i++)
        {
            size_t k = data.find(xpp::tokens[i], start);
            if (k != xpp::invalid_index && k < str_index)
            {
                // Skip double @@
                if (k != 0 && data[k - 1] == '@') { continue; }

                str_index = k;
                token_index = i;
            }
        }

        // No more token found.
        if (str_index == xpp::invalid_index) { return xpp::token_result{ xpp::token::end_of_file, data.size(), xpp::index_range::create_invalid() }; }

        //
        if (str_index && has_text_on_line_before(data, str_index - 1))
        {
            fst::print("Error: line should start with @command", index_to_line(data, str_index));
            return xpp::token_result::create_invalid();
        }

        size_t after_token_index = str_index + xpp::tokens[token_index].size();
        size_t line_end_index = fst::minimum(data.find('\n', after_token_index), data.size());

        return xpp::token_result{ (xpp::token) token_index, str_index, xpp::index_range{ after_token_index, line_end_index } };
    }

    inline xpp::command parse_macro(fst::string_view data, xpp::token_result tok_res)
    {
        fst::string_view cmd_line = ltrim(tok_res.cmd_range.substr(data));

        xpp::index_range ir = find_inside_param(cmd_line);

        if (ir.valid())
        {
            fst::string_view params = ir.substr2(cmd_line);

            fst::string_view function = cmd_line.substr(0, ir.left);
            return xpp::command{ tok_res.tok, function, params, {}, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
        }

        return xpp::command::create_invalid(fst::status_code::address_not_available);
    }

    inline xpp::command parse_trait(fst::string_view data, xpp::token_result tok_res)
    {
        fst::string_view cmd_line = ltrim(tok_res.cmd_range.substr(data));

        xpp::index_range ir = find_inside_param(cmd_line);

        if (ir.valid())
        {
            fst::string_view params = ir.substr2(cmd_line);

            fst::string_view function = cmd_line.substr(0, ir.left);
            return xpp::command{ tok_res.tok, function, params, {}, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
        }

        return xpp::command::create_invalid(fst::status_code::address_not_available);
    }

    inline xpp::command parse_doc(fst::string_view data, xpp::token_result tok_res)
    {
        fst::string_view cmd_line = ltrim(tok_res.cmd_range.substr(data));
        return xpp::command{ tok_res.tok, {}, {}, cmd_line, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
    }

    inline xpp::command parse_import(fst::string_view data, xpp::token_result tok_res)
    {
        fst::string_view cmd_line = ltrim(tok_res.cmd_range.substr(data));
        return xpp::command{ tok_res.tok, {}, {}, cmd_line, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
    }

    inline xpp::command parse_script(fst::string_view data, xpp::token_result tok_res)
    {
        fst::string_view cmd_line = ltrim(tok_res.cmd_range.substr(data));
        return xpp::command{ tok_res.tok, {}, {}, cmd_line, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
    }

    inline xpp::command parse_trace(fst::string_view data, xpp::token_result tok_res)
    {
        fst::string_view cmd_line = ltrim(tok_res.cmd_range.substr(data));
        return xpp::command{ tok_res.tok, {}, {}, cmd_line, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
    }

    inline xpp::command find_next(fst::string_view data, size_t start = 0);

    inline xpp::command parse_code_begin(fst::string_view data, xpp::token_result tok_res)
    {
        fst::string_view cmd_line = ltrim(tok_res.cmd_range.substr(data));

        xpp::index_range ir = find_inside_param(cmd_line);

        if (ir.valid())
        {
            fst::string_view params = ir.substr2(cmd_line);

            fst::string_view function = cmd_line.substr(0, ir.left);
            return xpp::command{ tok_res.tok, function, params, {}, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
        }

        return xpp::command{ tok_res.tok, {}, {}, cmd_line, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
    }

    inline xpp::command parse_code(fst::string_view data, xpp::token_result tok_res)
    {
        xpp::command begin_cmd = parse_code_begin(data, tok_res);
        xpp::command end_cmd = find_next(data, begin_cmd.range.right);

        if (!end_cmd || end_cmd.tok != xpp::token::endcode)
        {
            fst::print("ERROR: No @endcode after @code at line", index_to_line(data, begin_cmd.range.right));
            return xpp::command::create_invalid(fst::status_code::address_not_available);
        }

        fst::string_view content = data.substr(begin_cmd.range.right + 1, end_cmd.range.left - begin_cmd.range.right - 2);
        return xpp::command{ tok_res.tok, {}, begin_cmd.params, content, xpp::index_range{ begin_cmd.range.left, end_cmd.range.right } };
    }

    inline xpp::command parse_endcode(fst::string_view data, xpp::token_result tok_res)
    {
        return xpp::command{ tok_res.tok, {}, {}, {}, xpp::index_range{ tok_res.start_index, tok_res.cmd_range.right } };
    }

    inline xpp::command find_next(fst::string_view data, size_t start)
    {
        xpp::token_result tok_result = find_next_token(data, start);

        if (!tok_result.valid()) { return xpp::command{}; }

        switch (tok_result.tok)
        {
        case xpp::token::end_of_file: return xpp::command{};
        case xpp::token::import: return parse_import(data, tok_result);
        case xpp::token::macro: return parse_macro(data, tok_result);
        case xpp::token::doc: return parse_doc(data, tok_result);
        case xpp::token::code: return parse_code(data, tok_result);
        case xpp::token::endcode: return parse_endcode(data, tok_result);
        case xpp::token::trace: return parse_trace(data, tok_result);
        case xpp::token::trait: return parse_trait(data, tok_result);
        case xpp::token::script: return parse_script(data, tok_result);
        }

        return xpp::command{};
    }

    fst::error_result parser::parse_buffer(fst::string_view buffer, fst::string_view filepath)
    {
        _filepath = filepath;
        _data.resize(buffer.size());
        fst::memcpy(_data.data(), buffer.data(), buffer.size());

        fst::string_view data = _data;

        //_output_data = _data;
        xpp::command cmd;
        cmd.range.right = 0;
        while (cmd = find_next(data, cmd.range.right))
        {
            if (cmd.status != fst::status_code::success) { return cmd.status; }

            _commands.push_back(cmd);
        }

        return fst::status_code::success;
    }

    fst::vector<cmake_cache_value> parse_cache(fst::string_view data)
    {
        fst::vector<cmake_cache_value> vec;

        for (size_t i = 0; i < data.size(); i++)
        {
            size_t index = data.find('\n', i);
            if (index == fst::string_view::npos) { return vec; }

            fst::string_view s = data.substr(i, index - i - 1);

            if (!(s.find_first_of('/') == 0 || s.find_first_of('#') == 0 || s.empty()))
            {
                const size_t split_index = s.find(':');
                fst::string_view var_name = s.substr(0, split_index);

                if (var_name.find("-ADVANCED") == fst::string_view::npos)
                {
                    size_t eq_index = s.find('=', split_index + 1);
                    fst::string_view var_type = s.substr(split_index + 1, eq_index - split_index - 1);
                    fst::string_view var_value = s.substr(eq_index + 1);

                    if (!var_value.empty()) { vec.push_back(cmake_cache_value{ var_name, var_type, var_value }); }
                }
            }

            i = index;
        }

        return vec;
    }
} // namespace xpp
