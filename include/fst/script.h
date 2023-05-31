////
//// MIT License
////
//// Copyright (c) 2023 Alexandre Arsenault
////
//// Permission is hereby granted, free of charge, to any person obtaining a copy
//// of this software and associated documentation files (the "Software"), to deal
//// in the Software without restriction, including without limitation the rights
//// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//// copies of the Software, and to permit persons to whom the Software is
//// furnished to do so, subject to the following conditions:
////
//// The above copyright notice and this permission notice shall be included in all
//// copies or substantial portions of the Software.
////
//// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//// SOFTWARE.
////
//
//#pragma once
//
//#include "fst/common.h"
//#include "fst/traits.h"
//#include "fst/string_view.h"
//#include "fst/stream.h"
//#include "fst/memory.h"
//#include "fst/status_code.h"
//
//#include "fst/lua/lua.h"
//#include "fst/lua/lualib.h"
//#include "fst/lua/lauxlib.h"
//
////
/////*
////** Type for C functions registered with Lua
////*/
////typedef int (*lua_CFunction)(lua_State* L);
////
/////*
////** Type for continuation functions
////*/
////typedef int (*lua_KFunction)(lua_State* L, int status, lua_KContext ctx);
////
/////*
////** Type for functions that read/write blocks when loading/dumping Lua chunks
////*/
////typedef const char* (*lua_Reader)(lua_State* L, void* ud, size_t* sz);
////
////typedef int (*lua_Writer)(lua_State* L, const void* p, size_t sz, void* ud);
////
/////*
////** Type for memory-allocation functions
////*/
////typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);
////
/////*
////** Type for warning functions
////*/
////typedef void (*lua_WarnFunction)(void* ud, const char* msg, int tocont);
////
/////*
////** Type used by the debug API to collect debug information
////*/
////typedef struct lua_Debug lua_Debug;
////
/////*
////** Functions to be called by the debugger in specific events
////*/
////typedef void (*lua_Hook)(lua_State* L, lua_Debug* ar);
////
//
//FST_BEGIN_NAMESPACE
//    FST_DECLARE_MEMORY_CATEGORY(script_memory_category, "script");
//FST_END_NAMESPACE
//
//FST_BEGIN_SUB_NAMESPACE(script)
//
//    //#define LUA_TNONE (-1)
//    //
//    //#define LUA_TNIL 0
//    //#define LUA_TBOOLEAN 1
//    //#define LUA_TLIGHTUSERDATA 2
//    //#define LUA_TNUMBER 3
//    //#define LUA_TSTRING 4
//    //#define LUA_TTABLE 5
//    //#define LUA_TFUNCTION 6
//    //#define LUA_TUSERDATA 7
//    //#define LUA_TTHREAD 8
//    //
//    //#define LUA_NUMTYPES 9
//    enum class types {
//        none = LUA_TNONE,
//        nil = LUA_TNIL,
//        boolean = LUA_TBOOLEAN,
//        light_user_data = LUA_TLIGHTUSERDATA,
//        number = LUA_TNUMBER,
//        integer = LUA_TNUMBER,
//        string = LUA_TSTRING,
//        table = LUA_TTABLE,
//        function = LUA_TFUNCTION,
//        user_data = LUA_TUSERDATA,
//        thread = LUA_TTHREAD,
//        //,
//
//        //#define LUA_VNUMINT makevariant(LUA_TNUMBER, 0) /* integer numbers */
//        //#define LUA_VNUMFLT makevariant(LUA_TNUMBER, 1) /* float numbers */
//    };
//
//    struct table_tag
//    {};
//
//    struct light_user_data_tag
//    {};
//
//    namespace lua
//    {
//        //
//        // lua_gettable(L, "vector"); - Push vector onto the stack
//        // lua_pushstring(L, "y"); - Push y onto the stack
//        // lua_pushnumber(L, 7); - Push 7 onto the stack
//        //
//        // The stack has three new variables on it
//        // The index of 7 is -1
//        // The index of "y" is -2
//        // The index of "vector" is -3
//        //
//        // Call lua_settable on the "vector" table at index -3
//        // lua_settable(L, -3);
//        //
//        // lua_settable will pop the key ("y") and value (7) off the stack
//        // Only one item is left on the stack, the "vector" table
//        // The item left on the stack should be cleaned up at some point
//        //
//        // Lua also offers the lua_setfield(lua_State*, int, const char*) function
//        // which avoids the need to push the key onto the stack.
//        // The first two arguments are the same as lua_settable.
//        // The third argument is the key of what is being set.
//        //
//        // The value of what is being set is expected to be at the top of the stack.
//        // The lua_setfield function will pop the value off the stack,
//        // much like lua_settable does.
//        template <class _T, class = void>
//        struct variable_traits
//        {};
//
//        template <class _T>
//        struct variable_traits<_T, __fst::enable_if_t<__fst::is_same_v<__fst::remove_cv_t<_T>, lua_CFunction>>>
//        {
//            static constexpr types type_id = types::function;
//            using type = lua_CFunction;
//
//            static inline void push(lua_State* L, type fct) noexcept { lua_pushcclosure(L, fct, 0); }
//        };
//
//        template <class _T>
//        struct variable_traits<_T, __fst::enable_if_t<__fst::is_same_v<__fst::remove_cv_t<_T>, bool>>>
//        {
//            static constexpr types type_id = types::boolean;
//            using type = int;
//
//            static inline void push(lua_State* L, _T value) noexcept { lua_pushboolean(L, (type) value); }
//            static inline bool get(lua_State* L, int index) noexcept { return lua_toboolean(L, index) != 0; }
//            static inline bool opt(lua_State* L, int index, _T default_value) noexcept { return lua_isnone(L, index) ? ((type)(default_value)) : lua_toboolean(L, index) != 0; }
//        };
//
//        template <class _T>
//        struct variable_traits<_T, __fst::enable_if_t<__fst::is_same_v<__fst::remove_cv_t<_T>, table_tag>>>
//        {
//            static constexpr types type_id = types::table;
//            using type = table_tag;
//
//            static inline void push(lua_State* L, const _T&) noexcept { lua_createtable(L, 0, 0); }
//            //static inline type get(lua_State* L, int index) noexcept { return luaL_checktable(L, index); }
//            //static inline type opt(lua_State* L, int index, _T default_value) noexcept { return luaL_opttable(L, index, (type) default_value); }
//        };
//
//        template <class _T>
//        struct variable_traits<_T, __fst::enable_if_t<__fst::is_string_view_convertible_v<_T>>>
//        {
//            static constexpr types type_id = types::string;
//            using type = __fst::string_view;
//
//            static inline void push(lua_State* L, const _T& value) noexcept
//            {
//                __fst::string_view str(value);
//                lua_pushlstring(L, str.data(), str.size());
//            }
//
//            static inline type get(lua_State* L, int index) noexcept { return luaL_checkstring(L, index); }
//            static inline type opt(lua_State* L, int index, const char* default_value) noexcept { return luaL_optstring(L, index, default_value); }
//        };
//
//        template <class _T>
//        struct variable_traits<_T, __fst::enable_if_t<__fst::is_floating_point_v<_T>>>
//        {
//            static constexpr types type_id = types::number;
//            using type = lua_Number;
//            static inline void push(lua_State* L, _T value) noexcept { lua_pushnumber(L, (type) value); }
//            static inline type get(lua_State* L, int index) noexcept { return luaL_checknumber(L, index); }
//            static inline type opt(lua_State* L, int index, _T default_value) noexcept { return luaL_optnumber(L, index, (type) default_value); }
//        };
//
//        template <class _T>
//        struct variable_traits<_T, __fst::enable_if_t<__fst::is_integral_v<_T>>>
//        {
//            static constexpr types type_id = types::number;
//            using type = lua_Integer;
//
//            static inline void push(lua_State* L, _T value) noexcept { lua_pushinteger(L, (type) value); }
//            static inline type get(lua_State* L, int index) noexcept { return luaL_checkinteger(L, index); }
//            static inline type opt(lua_State* L, int index, _T default_value) noexcept { return luaL_optinteger(L, index, (type) default_value); }
//        };
//
//        template <>
//        struct variable_traits<void*>
//        {
//            static constexpr types type_id = types::light_user_data;
//            using type = void*;
//            static inline void push(lua_State* L, void* value) noexcept { lua_pushlightuserdata(L, value); }
//        };
//
//        inline int top(lua_State* L) noexcept
//        {
//            return lua_gettop(L);
//        }
//
//        inline void pop(lua_State* L, int n) noexcept
//        {
//            lua_settop(L, -(n - 1));
//        }
//
//        inline int get_global(lua_State* L, const char* name) noexcept
//        {
//            return lua_getglobal(L, name);
//        }
//
//        template <class T>
//        inline void set_global(lua_State* L, const char* name, const T& value) noexcept
//        {
//            variable_traits<T>::push(L, value);
//            lua_setglobal(L, name);
//        }
//
//        template <class T>
//        inline void set_field(lua_State* L, const char* name, const T& value) noexcept
//        {
//            variable_traits<T>::push(L, value);
//            lua_setfield(L, -2, name);
//        }
//
//        inline types get_type(lua_State* L, int n) noexcept
//        {
//            return (types) lua_type(L, n);
//        }
//
//        inline bool is_type(lua_State* L, types t, int n) noexcept
//        {
//            return t == get_type(L, n);
//        }
//
//        /*const char* to_string(lua_State* L, int n) noexcept
//        {
//            return lua_tolstring(L, n, nullptr);
//        }*/
//
//        __fst::error_result run_string(lua_State* L, __fst::string_view code) noexcept;
//        __fst::error_result run_file(lua_State* L, const char* filepath) noexcept;
//        __fst::error_result load_string(lua_State* L, __fst::string_view code) noexcept;
//
//    } // namespace lua
//
//    class var
//    {
//      public:
//        using state_type = lua_State;
//
//        inline var(state_type* s, int index) noexcept
//            : _state(s)
//        {
//            fst_assert(_state);
//            lua_pushvalue(_state, index);
//            _ref = luaL_ref(_state, LUA_REGISTRYINDEX);
//        }
//
//        inline var(state_type* s, const char* name) noexcept
//            : _state(s)
//        {
//            fst_assert(_state);
//            lua::get_global(_state, name);
//            _ref = luaL_ref(_state, LUA_REGISTRYINDEX);
//        }
//
//        inline ~var() noexcept
//        {
//            if (_state) { luaL_unref(_state, LUA_REGISTRYINDEX, _ref); }
//        }
//
//        inline state_type* state() const noexcept { return _state; }
//
//        inline bool valid() const noexcept { return _ref != LUA_NOREF; }
//
//        inline types type() const noexcept { return lua::get_type(_state, _ref); }
//
//        template<class T>
//        inline T to_value() const noexcept
//        {
//            push_to_stack();
//            T v = lua::variable_traits<T>::get(_state, -1);
//            lua_pop(_state, 1);
//            return v;
//        }
//
//        inline void push_to_stack() const noexcept
//        {
//            fst_assert(_state);
//            lua_rawgeti(_state, LUA_REGISTRYINDEX, _ref);
//        }
//
//      private:
//        state_type* _state = nullptr;
//        int _ref = LUA_NOREF;
//    };
//
//    class engine
//    {
//      public:
//        using _MemoryCategory = __fst::default_memory_category;
//        using _MemoryZone = __fst::default_memory_zone;
//
//        engine() noexcept;
//
//        ~engine() noexcept;
//
//        inline lua_State* state() const noexcept { return _state; }
//
//        template <class T>
//        inline void set_global(const char* name, const T& value) const noexcept
//        {
//            lua::set_global(state(), name, value);
//        }
//
//        inline int get_global(const char* name) const noexcept { return lua::get_global(state(), name); }
//
//        template <class T>
//        inline void set_field(const char* name, const T& value) const noexcept
//        {
//            lua::set_field(state(), name, value);
//        }
//
//        inline int top() const noexcept { return lua::top(state()); }
//
//        inline void pop() const noexcept { lua::pop(state(), 1); }
//
//        inline void pop(int n) const noexcept { lua::pop(state(), n); }
//
//        inline types get_type(int n) noexcept { return lua::get_type(state(), n); }
//
//        inline bool is_type(types t, int n) noexcept { return lua::is_type(state(), t, n); }
//
//        __fst::error_result run_string(__fst::string_view code) const noexcept { return lua::run_string(state(), code); }
//        __fst::error_result run_file(const char* filepath) const noexcept { return lua::run_file(state(), filepath); }
//        __fst::error_result load_string(__fst::string_view code) const noexcept { return lua::load_string(state(), code); }
//
//      private:
//        using state_type = lua_State;
//        lua_State* _state = nullptr;
//
//        void* internal_alloc(void* ptr, size_t osize, size_t nsize)
//        {
//            if (nsize == 0)
//            {
//                if (ptr) { _MemoryZone::deallocate(ptr, _MemoryCategory::id()); }
//                return nullptr;
//            }
//            else if (osize >= nsize) { return ptr; }
//
//            void* data = _MemoryZone::allocate(nsize, _MemoryCategory::id());
//            if (!data) { return nullptr; }
//
//            if (ptr) { __fst::memcpy(data, ptr, osize); }
//            _MemoryZone::deallocate(ptr, _MemoryCategory::id());
//            return data;
//        }
//
//        void internal_warn(const char* msg, FST_ATTRIBUTE_UNUSED int tocont) { fst::print("DSLJDSKLDJLKSD", msg); }
//
//        static int lua_pannic_callback(state_type* s)
//        {
//            const char* msg = lua_tostring(s, -1);
//            if (msg == NULL) msg = "error object is not a string";
//            lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n", msg);
//            return 0; /* return to Lua to abort */
//        }
//    };
//
//FST_END_SUB_NAMESPACE
