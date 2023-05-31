#include "fst/lua/state.h"
#include "fst/lua.h"
#include "fst/stream.h"

FST_BEGIN_SUB_NAMESPACE(lua)

   

    __fst::error_result state::run_string(__fst::string_view code) const noexcept
    {
        if (int status = luaL_loadbufferx(_state, code.data(), code.size(), "fst_scrcipt", nullptr))
        {
            // If something went wrong, error message is at the top of the stack.
            __fst::debug()("Couldn't load file :", lua_tostring(_state, -1));

            using sc = __fst::status_code;
            switch (status)
            {
            case LUA_ERRRUN: return sc::runtime_error;
            case LUA_ERRSYNTAX: return sc::syntax_error;
            case LUA_ERRMEM: return sc::not_enough_memory;
            case LUA_ERRERR: return sc::script_error;
            }

            return sc::unknown;
        }

        if (int result = lua_pcallk(_state, 0, LUA_MULTRET, 0, 0, nullptr))
        {
            __fst::debug()("Failed to run script :", lua_tostring(_state, -1));

            using sc = __fst::status_code;
            switch (result)
            {
            case LUA_ERRRUN: return sc::runtime_error;
            case LUA_ERRSYNTAX: return sc::syntax_error;
            case LUA_ERRMEM: return sc::not_enough_memory;
            case LUA_ERRERR: return sc::script_error;
            }

            return sc::unknown;
        }

        return __fst::status_code::success;
    }

    __fst::error_result state::run_file(const char* filepath) const noexcept
    {
        // Load the file containing the script we are going to run.
        if (int status = luaL_loadfile(_state, filepath))
        {
            // If something went wrong, error message is at the top of the stack.
            __fst::print("Couldn't load file :", lua_tostring(_state, -1));

            using sc = __fst::status_code;
            switch (status)
            {
            case LUA_ERRRUN: return sc::runtime_error;
            case LUA_ERRSYNTAX: return sc::syntax_error;
            case LUA_ERRMEM: return sc::not_enough_memory;
            case LUA_ERRERR: return sc::script_error;
            }

            return sc::unknown;
        }

        if (int result = lua_pcall(_state, 0, LUA_MULTRET, 0))
        {
            __fst::print("Failed to run script :", lua_tostring(_state, -1));

            using sc = __fst::status_code;
            switch (result)
            {
            case LUA_ERRRUN: return sc::runtime_error;
            case LUA_ERRSYNTAX: return sc::syntax_error;
            case LUA_ERRMEM: return sc::not_enough_memory;
            case LUA_ERRERR: return sc::script_error;
            }

            return sc::unknown;
        }

        return __fst::status_code::success;
    }

    __fst::error_result state::load_string(__fst::string_view code) const noexcept
    {
        if (int status = luaL_loadbufferx(_state, code.data(), code.size(), "fst_scrcipt", nullptr))
        {
            // If something went wrong, error message is at the top of the stack.
            __fst::debug()("Couldn't load file :", lua_tostring(_state, -1));

            using sc = __fst::status_code;
            switch (status)
            {
            case LUA_ERRRUN: return sc::runtime_error;
            case LUA_ERRSYNTAX: return sc::syntax_error;
            case LUA_ERRMEM: return sc::not_enough_memory;
            case LUA_ERRERR: return sc::script_error;
            }

            return sc::unknown;
        }
        return __fst::status_code::success;
    }

FST_END_SUB_NAMESPACE
