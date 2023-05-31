#pragma once
#include "fst/array.h"
#include "fst/string_view.h"

namespace xpp
{
    FST_INLINE_VAR constexpr size_t invalid_index = (fst::numeric_limits<size_t>::max)();

    enum class token {
        import,
        doc,
        macro,
        code,
        endcode,
        trace,
        trait,
        script,

        end_of_file,
        invalid
    };

    //enum class arg_requirement {
    //    never,
    //    optional,
    //    always,
    //    prefix
    //};

    inline const fst::array<fst::string_view, 8> tokens = { "@import", "@doc", "@macro", "@code", "@endcode", "@trace", "@trait", "@script" };
    
    //inline const fst::array<arg_requirement, 5> tokens_requirement
      //  = { arg_requirement::never, arg_requirement::optional, arg_requirement::always, arg_requirement::prefix, arg_requirement::never };

    struct index_range
    {
        size_t left, right;

        static index_range create_invalid() { return index_range{ invalid_index, invalid_index }; }

        inline bool valid() const noexcept { return left != invalid_index && right != invalid_index; }

        inline fst::string_view substr(fst::string_view str) const noexcept { return str.substr(left, right - left); }
        inline fst::string_view substr2(fst::string_view str) const noexcept { return str.substr(left + 1, right - left - 1); }
    };

    struct token_result
    {
        
        static token_result create_invalid() { return token_result{ token::invalid, invalid_index, index_range::create_invalid() }; }

        inline bool valid() const noexcept { return tok != token::invalid; }

        token tok;
        size_t start_index;
        index_range cmd_range;
    };

    struct command
    {
        token tok = token::invalid;
        fst::string_view function;
        fst::string_view params;
        fst::string_view content;
        index_range range;
        fst::status_code status = fst::status_code::success;

                static command create_invalid(fst::status_code status)
        { return command{ token::invalid, {}, {}, {}, index_range::create_invalid(), status };
        }


        inline explicit operator bool() const noexcept
        {
            return tok != token::invalid;
        }
    };

    struct cmake_cache_value
    {
        fst::string name, type, value;
    };

    inline size_t index_to_line(fst::string_view str, size_t index)
    {
        fst_assert(index < str.size());

        size_t end_line_count = 0;
        for (size_t i = 0; i <= index; i++)
        {
            if (str[i] == '\n') { end_line_count++; }
        }
        return end_line_count + 1;
    }

    inline fst::string_view ltrim(fst::string_view s)
    {
        for (size_t i = 0; i < s.size(); i++)
        {
            if (!fst::is_one_of(s[i], ' ', '\t', '\n', '\v', '\r')) { return s.substr(i); }
        }
        return fst::string_view();
    }
}