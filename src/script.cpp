//#include "fst/script.h"
//
//
//#include "fst/lua/lua.h"
//#include "fst/lua/lualib.h"
//#include "fst/lua/lauxlib.h"
//
//FST_BEGIN_SUB_NAMESPACE(script)
//
//engine::engine() noexcept
//    {
//        _state = lua_newstate([](void* ud, void* ptr, size_t osize, size_t nsize) noexcept -> void* { return ((engine*) ud)->internal_alloc(ptr, osize, nsize); }, this);
//
//        lua_atpanic(_state, &engine::lua_pannic_callback);
//        lua_setwarnf(
//            _state, [](void* ud, const char* msg, int tocont) noexcept { ((engine*) ud)->internal_warn(msg, tocont); }, this);
//        luaL_openlibs(_state);
//    }
//
//engine::~engine() noexcept
//    {
//        if (_state) { lua_close(_state); }
//    }
//    namespace lua
//{
//
//    __fst::error_result run_string(lua_State* L, __fst::string_view code) noexcept
//    {
//        // Load the file containing the script we are going to run.
//        //#define luaL_loadbuffer(L, s, sz, n) luaL_loadbufferx(L, s, sz, n, NULL)
//
//        //luaL_loadbuffer(L, s, code.size(), s);
//
//        //if (int status = luaL_loadstring(_state, code))
//        if (int status = luaL_loadbufferx(L, code.data(), code.size(), "fst_scrcipt", nullptr))
//        {
//
//            // If something went wrong, error message is at the top of the stack.
//            __fst::debug()("Couldn't load file :", lua_tostring(L, -1));
//
//            using sc = __fst::status_code;
//            switch (status)
//            {
//            case LUA_ERRRUN: return sc::runtime_error;
//            case LUA_ERRSYNTAX: return sc::syntax_error;
//            case LUA_ERRMEM: return sc::not_enough_memory;
//            case LUA_ERRERR: return sc::script_error;
//            }
//
//            return sc::unknown;
//        }
//        //lua_pcallk(L, (n), (r), (f), 0, NULL)
//        //if (int result = lua_pcall(_state, 0, LUA_MULTRET, 0))
//        if (int result = lua_pcallk(L, 0, LUA_MULTRET, 0, 0, nullptr))
//        {
//            __fst::debug()("Failed to run script :", lua_tostring(L, -1));
//
//            using sc = __fst::status_code;
//            switch (result)
//            {
//            case LUA_ERRRUN: return sc::runtime_error;
//            case LUA_ERRSYNTAX: return sc::syntax_error;
//            case LUA_ERRMEM: return sc::not_enough_memory;
//            case LUA_ERRERR: return sc::script_error;
//            }
//
//            return sc::unknown;
//        }
//
//        return __fst::status_code::success;
//    }
//
//    __fst::error_result run_file(lua_State* L, const char* filepath) noexcept
//    {
//        // Load the file containing the script we are going to run.
//        if (int status = luaL_loadfile(L, filepath))
//        {
//            // If something went wrong, error message is at the top of the stack.
//            __fst::print("Couldn't load file :", lua_tostring(L, -1));
//            using sc = __fst::status_code;
//            switch (status)
//            {
//            case LUA_ERRRUN: return sc::runtime_error;
//            case LUA_ERRSYNTAX: return sc::syntax_error;
//            case LUA_ERRMEM: return sc::not_enough_memory;
//            case LUA_ERRERR: return sc::script_error;
//            }
//
//            return sc::unknown;
//        }
//
//        if (int result = lua_pcall(L, 0, LUA_MULTRET, 0))
//        {
//            __fst::print("Failed to run script :", lua_tostring(L, -1));
//            using sc = __fst::status_code;
//            switch (result)
//            {
//            case LUA_ERRRUN: return sc::runtime_error;
//            case LUA_ERRSYNTAX: return sc::syntax_error;
//            case LUA_ERRMEM: return sc::not_enough_memory;
//            case LUA_ERRERR: return sc::script_error;
//            }
//
//            return sc::unknown;
//        }
//
//        return __fst::status_code::success;
//    }
//
//    __fst::error_result load_string(lua_State* L, __fst::string_view code) noexcept
//    {
//        if (int status = luaL_loadbufferx(L, code.data(), code.size(), "fst_scrcipt", nullptr))
//        {
//
//            // If something went wrong, error message is at the top of the stack.
//            __fst::debug()("Couldn't load file :", lua_tostring(L, -1));
//
//            using sc = __fst::status_code;
//            switch (status)
//            {
//            case LUA_ERRRUN: return sc::runtime_error;
//            case LUA_ERRSYNTAX: return sc::syntax_error;
//            case LUA_ERRMEM: return sc::not_enough_memory;
//            case LUA_ERRERR: return sc::script_error;
//            }
//
//            return sc::unknown;
//        }
//        return __fst::status_code::success;
//    }
//}
//
//FST_END_SUB_NAMESPACE
