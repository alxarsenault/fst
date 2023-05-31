//
// MIT License
//
// Copyright (c) 2023 Alexandre Arsenault
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "fst/common.h"
#include "fst/traits.h"
#include "fst/stack_string.h"
#include "fst/string_literal.h"
#include "fst/small_vector.h"

#include "fst/lua/common.h"
#include "fst/lua/state.h"

FST_BEGIN_SUB_NAMESPACE(lua)

    template <__fst::string_literal _Name>
    class module_register
    {
      public:
        static constexpr __fst::string_literal __module_name = _Name;
        static inline const char* module_name() noexcept { return __module_name.c_str(); }

        inline module_register(__fst::lua::state s) noexcept
            : _state(s)
        {}

        template <auto Fct>
        inline module_register& add_static_function(const char* name) noexcept
        {
            _register.emplace_back(name, _state.create_function<Fct>());
            return *this;
        }

        inline module_register& add_custom_function(const char* name, lua_CFunction fct) noexcept
        {
            _register.emplace_back(name, fct);
            return *this;
        }

         
        inline __fst::error_result import() noexcept
        {
            __fst::small_vector<luaL_Reg, 10> reg;

            for (const function_info& f : _register)
            {
                reg.push_back({ f.name.c_str(), f.fct });
            }

            // Last element.
            reg.push_back(luaL_Reg{ nullptr, nullptr });

            //#define luaL_newlibtable(L, l) lua_createtable(L, 0, sizeof(l) / sizeof((l)[0]) - 1)

            lua_createtable(_state, 0, (int)reg.size());
            luaL_setfuncs(_state, reg.data(), 0);
            lua_setglobal(_state, module_name());


            return __fst::status_code::success;
        }

      private:
        struct function_info
        {
            __fst::stack_string<64> name;
            lua_CFunction fct;
        };

        __fst::lua::state _state;
        __fst::small_vector<function_info, 10, alignof(function_info)> _register;
    };

    template <class _T, __fst::string_literal _Name, class _MemoryZone = __fst::lua::inplace_memory_zone>
    class class_register
    {
      public:
        using class_type = _T;

        using memory_category_type = __fst::lua_memory_category;
        using memory_zone_type = _MemoryZone;
        static constexpr bool inplace_allocation = __fst::is_same_v<_MemoryZone, inplace_memory_zone>;
        static_assert(__fst::is_static_memory_zone<_MemoryZone>::value, "Only static memory zones are currently supported");

        static constexpr __fst::string_literal __class_name = _Name;
        static constexpr __fst::string_literal __meta_class_name = __fst::string_literal_concat<"luaL_", _Name>();

        static inline const char* class_name() noexcept { return __class_name.c_str(); }
        static inline const char* meta_class_name() noexcept { return __meta_class_name.c_str(); }

        inline class_register(__fst::lua::state s) noexcept
            : _state(s)
        {}

        template <auto Fct>
        inline class_register& add_member_function(const char* name) noexcept
        {
            _register.emplace_back(name, &class_register::member_function<Fct>);
            return *this;
        }

        template <auto Fct>
        inline class_register& add_custom_member_function(const char* name) noexcept
        {
            _register.emplace_back(name, &class_register::custom_member_function<Fct>);
            return *this;
        }

        template <auto Fct>
        inline class_register& add_static_function(const char* name) noexcept
        {
            _register.emplace_back(name, _state.create_function<Fct>());
            return *this;
        }

        inline class_register& add_custom_function(const char* name, lua_CFunction fct) noexcept
        {
            _register.emplace_back(name, fct);
            return *this;
        }

        template <class... _Args>
        inline class_register& add_constructor(const char* name) noexcept
        {
            _register.emplace_back(name, &class_register::constructor<_Args...>);
            return *this;
        }

        template <class... _Args>
        inline class_register& add_global_constructor(const char* name) noexcept
        {
            _state.set_global(name, (lua_CFunction) &class_register::constructor<_Args...>);
            return *this;
        }

        static inline class_type* check_class(lua_State* L, int n) noexcept
        {
            // This checks that the argument is a userdata with the metatable 'meta_class_name()'.
            if constexpr (inplace_allocation) { return (class_type*) luaL_checkudata(L, n, meta_class_name()); }
            else { return *(class_type**) luaL_checkudata(L, n, meta_class_name()); }
        }

        // Import as a global table.
        inline __fst::error_result import() noexcept;

        // Import class into the 'module_name' global table.
        inline __fst::error_result import(const char* module_name) noexcept;

      private:
        struct function_info
        {
            __fst::stack_string<64> name;
            lua_CFunction fct;
        };

        __fst::lua::state _state;
        __fst::small_vector<function_info, 10, alignof(function_info)> _register;

        template <class _StateParams, size_t... I>
        static inline class_type* call_constructor_impl(const _StateParams& p, class_type* obj, __fst::index_sequence<I...>) noexcept
        {
            if constexpr (inplace_allocation)
            {
                //
                return fst_placement_new(obj) class_type((p.template get<I>())...);
            }
            else
            {
                class_type* c = (class_type*) _MemoryZone::aligned_allocate(sizeof(class_type), alignof(class_type), __fst::lua_memory_category::id());
                fst_placement_new(c) class_type((p.template get<I>())...);
                return c;
            }
        }

        template <class _StateParams>
        static inline class_type* call_constructor(const _StateParams& p, class_type* obj) noexcept
        {
            return call_constructor_impl(p, obj, __fst::make_index_sequence<_StateParams::param_list::size()>{});
        }

        template <class _StateParams, class _Fct, size_t... I>
        static inline auto call_member_function_impl(const _StateParams& p, _Fct fct, __fst::index_sequence<I...>) noexcept
        {
            class_type* c = check_class(p.state(), 1);
            return (c->*fct)((p.template get<I>())...);
        }

        template <class _StateParams, class _Fct>
        static inline auto call_member_function(const _StateParams& p, _Fct fct) noexcept
        {
            return call_member_function_impl(p, fct, __fst::make_index_sequence<_StateParams::param_list::size()>{});
        }

        template <class _StateParams, class _Fct, size_t... I>
        static inline auto call_custom_member_function_impl(const _StateParams& p, _Fct fct, __fst::index_sequence<I...>) noexcept
        {
            class_type* c = check_class(p.state(), 1);
            return fct(c, (p.template get<I + 1>())...);
        }

        template <class _StateParams, class _Fct>
        static inline auto call_custom_member_function(const _StateParams& p, _Fct fct) noexcept
        {
            return call_custom_member_function_impl(p, fct, __fst::make_index_sequence<_StateParams::param_list::size() - 1>{});
        }

        template <class... _Args>
        static int constructor(lua_State* L) noexcept
        {
            using state_params_type = __fst::lua::state_parameters<__fst::type_list<_Args...>>;
            constexpr int params_size = (int) sizeof...(_Args);

            __fst::lua::state s(L);

            if (s.top() != params_size)
            {
                // Error.
                return luaL_error(L, "expecting exactly %d arguments", params_size);
            }

            // We could actually allocate Foo itself as a user data but
            // since user data can be GC'ed and we gain unity by using CRT's heap
            // all along.

            if constexpr (inplace_allocation)
            {
                class_type* udata = (class_type*) lua_newuserdatauv(s, sizeof(class_type), 1);
                call_constructor(state_params_type(s, 1), udata);
            }
            else
            {
                class_type** udata = (class_type**) lua_newuserdatauv(s, sizeof(class_type*), 1);
                *udata = call_constructor(state_params_type(s, 1), nullptr);
            }

            // Usually, we'll just use "Foo" as the second parameter, but I
            // say luaL_Foo here to distinguish the difference:
            //
            // This 2nd parameter here is an _internal label_ for luaL, it is
            // _not_ exposed to Lua by default.
            //
            // Effectively, this metatable is not accessible by Lua by default.
            // luaL_getmetatable(L, meta_class_name());
            lua_getfield(L, LUA_REGISTRYINDEX, meta_class_name());

            // The Lua stack at this point looks like this:
            //
            //     3| metatable "luaL_foo"   |-1
            //     2| userdata               |-2
            //     1| string parameter       |-3
            //
            // So the following line sets the metatable for the user data to the luaL_Foo
            // metatable
            //
            // We must set the metatable here because Lua prohibits setting
            // the metatable of a userdata in Lua. The only way to set a metatable
            // of a userdata is to do it in C.
            lua_setmetatable(L, -2);

            // The Lua stack at this point looks like this:
            //
            //     2| userdata               |-1
            //     1| string parameter       |-2
            //
            // We return 1 so Lua callsite will get the user data and
            // Lua will clean the stack after that.

            return 1;
        }

        static int destructor(lua_State* L) noexcept
        {
            if (class_type* c = check_class(L, 1))
            {
                c->~class_type();

                // Only deallocate when not inplace_allocation.
                if constexpr (!inplace_allocation) { _MemoryZone::aligned_deallocate(c, __fst::lua_memory_category::id()); }
            }

            return 0;
        }

        template <auto Fct>
        static inline int member_function(lua_State* L) noexcept
        {
            using ftraits = __fst::lua::call_traits<Fct>;
            using state_params_type = __fst::lua::state_parameters<typename ftraits::args_list>;
            constexpr int params_size = (int) (ftraits::args_size + 1);

            __fst::lua::state s = L;

            if (s.top() != params_size)
            {
                // Error.
                return luaL_error(s, "expecting exactly %d arguments", params_size);
            }

            if constexpr (!__fst::is_same_v<typename ftraits::return_t, void>)
            {
                typename ftraits::return_t ret = call_member_function(state_params_type(s, 2), Fct);
                s.push(ret);
                return 1;
            }
            else
            {
                call_member_function(state_params_type(s, 2), Fct);
                return 0;
            }
        }

        template <auto Fct>
        static inline int custom_member_function(lua_State* L) noexcept
        {
            using ftraits = __fst::lua::call_traits<Fct>;
            using state_params_type = __fst::lua::state_parameters<typename ftraits::args_list>;
            constexpr int params_size = (int) (ftraits::args_size);

            __fst::lua::state s = L;

            if (s.top() != params_size)
            {
                // Error.
                return luaL_error(s, "expecting exactly %d arguments", params_size);
            }

            if constexpr (!__fst::is_same_v<typename ftraits::return_t, void>)
            {
                typename ftraits::return_t ret = call_custom_member_function(state_params_type(s, 1), Fct);
                s.push(ret);
                return 1;
            }
            else
            {
                call_custom_member_function(state_params_type(s, 1), Fct);
                return 0;
            }
        }
    };

    template <class _T, __fst::string_literal _Name, class _MemoryZone>
    inline __fst::error_result class_register<_T, _Name, _MemoryZone>::import() noexcept
    {
        __fst::small_vector<luaL_Reg, 10> reg;
        reg.push_back({ "__gc", &class_register::destructor });

        for (const function_info& f : _register)
        {
            reg.push_back({ f.name.c_str(), f.fct });
        }

        // Last element.
        reg.push_back(luaL_Reg{ nullptr, nullptr });

        // Create a luaL metatable.
        // This metatable is not exposed to Lua.
        // The "luaL_Foo" label is used by luaL internally to identity things.
        int ret = luaL_newmetatable(_state, meta_class_name());
        if (!ret)
        {
            fst_error("metatable already exists");
            return __fst::status_code::already_created;
        }

        // Register the C functions _into_ the metatable we just created.
        luaL_setfuncs(_state, reg.data(), 0);

        //_state.set_field<int>("bingo", 32);

        // The Lua stack at this point looks like this:
        //
        //     1| metatable "luaL_Foo"   |-1
        lua_pushvalue(_state, -1);

        // The Lua stack at this point looks like this:
        //
        //     2| metatable "luaL_Foo"   |-1
        //     1| metatable "luaL_Foo"   |-2

        // Set the "__index" field of the metatable to point to itself.
        // This pops the stack
        lua_setfield(_state, -1, "__index");

        // The Lua stack at this point looks like this:
        //
        //     1| metatable "luaL_Foo"   |-1

        // The luaL_Foo metatable now has the following fields
        //     - __gc
        //     - __index
        //     - add
        //     - new

        // Now we use setglobal to officially expose the luaL_Foo metatable
        // to Lua. And we use the name "Foo".
        //
        // This allows Lua scripts to _override_ the metatable of Foo.
        // For high security code this may not be called for but
        // we'll do this to get greater flexibility.
        lua_setglobal(_state, class_name());

        return __fst::status_code::success;
    }

    template <class _T, __fst::string_literal _Name, class _MemoryZone>
    inline __fst::error_result class_register<_T, _Name, _MemoryZone>::import(const char* module_name) noexcept
    {
        // Add all function to reg.
        __fst::small_vector<luaL_Reg, 10> reg;
        reg.push_back({ "__gc", &class_register::destructor });

        for (const function_info& f : _register)
        {
            reg.push_back({ f.name.c_str(), f.fct });
        }

        // Last element.
        reg.push_back(luaL_Reg{ nullptr, nullptr });

        // Push the 'module_name' table on stack.
        _state.get_global(module_name);

        // Create a luaL metatable.
        // This metatable is not exposed to Lua.
        // The "meta_class_name" label is used by luaL internally to identity things.
        int ret = luaL_newmetatable(_state, meta_class_name());
        if (!ret)
        {
            fst_error("metatable already exists");
            return __fst::status_code::already_created;
        }

        // Register the C functions into the metatable we just created.
        luaL_setfuncs(_state, reg.data(), 0);

        // The Lua stack at this point looks like this:
        //
        //     1| metatable "luaL_Foo"   |-1
        lua_pushvalue(_state, -1);

        // The Lua stack at this point looks like this:
        //
        //     2| metatable "luaL_Foo"   |-1
        //     1| metatable "luaL_Foo"   |-2

        // Set the "__index" field of the metatable to point to itself.
        // This pops the stack
        lua_setfield(_state, -1, "__index");

        // The Lua stack at this point looks like this:
        //
        //     1| metatable "luaL_Foo"   |-1

        // The luaL_Foo metatable now has the following fields
        //     - __gc
        //     - __index
        //     - add
        //     - new

        // Set the class table into the 'module_name' table.
        lua_setfield(_state, -2, class_name());

        _state.pop();

        return __fst::status_code::success;
    }

FST_END_SUB_NAMESPACE
