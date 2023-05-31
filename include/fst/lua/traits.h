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
#include "fst/string_view.h"
#include "fst/lua/common.h"

FST_BEGIN_SUB_NAMESPACE(lua)

    //
    // lua_gettable(L, "vector"); - Push vector onto the stack
    // lua_pushstring(L, "y"); - Push y onto the stack
    // lua_pushnumber(L, 7); - Push 7 onto the stack
    //
    // The stack has three new variables on it
    // The index of 7 is -1
    // The index of "y" is -2
    // The index of "vector" is -3
    //
    // Call lua_settable on the "vector" table at index -3
    // lua_settable(L, -3);
    //
    // lua_settable will pop the key ("y") and value (7) off the stack
    // Only one item is left on the stack, the "vector" table
    // The item left on the stack should be cleaned up at some point
    //
    // Lua also offers the lua_setfield(lua_State*, int, const char*) function
    // which avoids the need to push the key onto the stack.
    // The first two arguments are the same as lua_settable.
    // The third argument is the key of what is being set.
    //
    // The value of what is being set is expected to be at the top of the stack.
    // The lua_setfield function will pop the value off the stack,
    // much like lua_settable does.
    template <class _T, class = void>
    struct type_traits
    {};

    template <class _T>
    struct type_traits<_T, __fst::enable_if_t<__fst::is_same_v<__fst::remove_cv_t<_T>, lua_CFunction>>>
    {
        static constexpr type type_id = type::function;
        using type = lua_CFunction;

        static inline void push(lua_State* L, type fct) noexcept { lua_pushcclosure(L, fct, 0); }
    };

    template <class _T>
    struct type_traits<_T, __fst::enable_if_t<__fst::is_same_v<__fst::remove_cv_t<_T>, bool>>>
    {
        static constexpr type type_id = type::boolean;
        using type = int;

        static inline void push(lua_State* L, _T value) noexcept { lua_pushboolean(L, (type) value); }
        static inline bool get(lua_State* L, int index) noexcept { return lua_toboolean(L, index) != 0; }
        static inline bool opt(lua_State* L, int index, _T default_value) noexcept
        {
            return lua_isnone(L, index) ? ((type) (default_value)) : lua_toboolean(L, index) != 0;
        }
    };

    template <class _T>
    struct type_traits<_T, __fst::enable_if_t<__fst::is_same_v<__fst::remove_cv_t<_T>, table_tag>>>
    {
        static constexpr type type_id = type::table;
        using type = table_tag;

        static inline void push(lua_State* L, const _T&) noexcept { lua_createtable(L, 0, 0); }
        //static inline type get(lua_State* L, int index) noexcept { return luaL_checktable(L, index); }
        //static inline type opt(lua_State* L, int index, _T default_value) noexcept { return luaL_opttable(L, index, (type) default_value); }
    };

    template <class _T>
    struct type_traits<_T, __fst::enable_if_t<__fst::is_string_view_convertible_v<_T>>>
    {
        static constexpr type type_id = type::string;
        using type = __fst::string_view;

        static inline void push(lua_State* L, const _T& value) noexcept
        {
            __fst::string_view str(value);
            lua_pushlstring(L, str.data(), str.size());
        }

        static inline type get(lua_State* L, int index) noexcept { return luaL_checkstring(L, index); }
        static inline type opt(lua_State* L, int index, const char* default_value) noexcept { return luaL_optstring(L, index, default_value); }
    };

    template <class _T>
    struct type_traits<_T, __fst::enable_if_t<__fst::is_floating_point_v<_T>>>
    {
        static constexpr type type_id = type::number;
        using type = lua_Number;
        static inline void push(lua_State* L, _T value) noexcept { lua_pushnumber(L, (type) value); }
        static inline type get(lua_State* L, int index) noexcept { return luaL_checknumber(L, index); }
        static inline type opt(lua_State* L, int index, _T default_value) noexcept { return luaL_optnumber(L, index, (type) default_value); }
    };

    template <class _T>
    struct type_traits<_T, __fst::enable_if_t<__fst::is_integral_v<_T>>>
    {
        static constexpr type type_id = type::number;
        using type = lua_Integer;

        static inline void push(lua_State* L, _T value) noexcept { lua_pushinteger(L, (type) value); }
        static inline type get(lua_State* L, int index) noexcept { return luaL_checkinteger(L, index); }
        static inline type opt(lua_State* L, int index, _T default_value) noexcept { return luaL_optinteger(L, index, (type) default_value); }
    };

    template <>
    struct type_traits<void*>
    {
        static constexpr type type_id = type::light_user_data;
        using type = void*;

        static inline void push(lua_State* L, void* value) noexcept { lua_pushlightuserdata(L, value); }
    };

     namespace detail
    {
        template <class F>
        struct call_traits_impl
        {
            static_assert(__fst::always_false<F>, "error");
        };

        template <class _R, class... _Args>
        struct call_traits_impl<_R (*)(_Args...)>
        {
            using return_t = _R;
            using args_list = __fst::type_list<_Args...>;
            static constexpr size_t args_size = sizeof...(_Args);
        };

        template <class _T, class _R, class... _Args>
        struct call_traits_impl<_R (_T::*)(_Args...)>
        {
            using class_type = _T;
            using return_t = _R;
            using args_list = __fst::type_list<_Args...>;
            static constexpr size_t args_size = sizeof...(_Args);
        };
    } // namespace detail

    template <auto Fct, class Enable = void>
    struct call_traits : detail::call_traits_impl<decltype(+Fct)>
    {
        static constexpr function_type ftype = function_type::normal;
    };

    template <auto Fct>
    struct call_traits<Fct, __fst::enable_if_t<__fst::is_function_pointer_v<decltype(Fct)>>> : detail::call_traits_impl<decltype(Fct)>
    {
        static constexpr function_type ftype = function_type::normal;
    };

    template <auto Fct>
    struct call_traits<Fct, __fst::enable_if_t<__fst::is_member_function_pointer_v<decltype(Fct)>>> : detail::call_traits_impl<decltype(Fct)>
    {
        static constexpr function_type ftype = function_type::member;
    };
FST_END_SUB_NAMESPACE
