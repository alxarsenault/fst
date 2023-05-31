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
#include "fst/status_code.h"
#include "fst/string.h"
#include "fst/stream.h"

#include "fst/lua/common.h"
#include "fst/lua/traits.h"

// https://gist.github.com/roxlu/00974d35f64c5c5be8d6
FST_BEGIN_SUB_NAMESPACE(lua)

    class state
    {
      public:
        constexpr state() noexcept = default;
        constexpr state(const state&) noexcept = default;
        constexpr state(state&&) noexcept = default;

        constexpr state(__fst::lua::state_type s) noexcept
            : _state(s)
        {}

        constexpr state(__fst::nullptr_t) noexcept
            : _state(nullptr)
        {}

        constexpr state& operator=(const state&) noexcept = default;
        constexpr state& operator=(state&&) noexcept = default;

        inline explicit constexpr operator bool() const noexcept { return (bool) _state; }

        inline constexpr operator __fst::lua::state_type() const noexcept { return _state; }
        inline constexpr __fst::lua::state_type get_state() const noexcept { return _state; }

        //
        // stack
        //

        inline int top() const noexcept { return lua_gettop(_state); }

        inline void pop() const noexcept { lua_settop(_state, 1); }
        inline void pop(int n) const noexcept { lua_settop(_state, -(n - 1)); }

        //
        //
        //

        inline __fst::lua::type get_global(const char* name) const noexcept { return (__fst::lua::type) lua_getglobal(_state, name); }

        inline __fst::lua::type get_global(__fst::string_view name) const noexcept { return (__fst::lua::type) lua_getglobal(_state, name); }

        template <class T>
        inline void set_global(const char* name, const T& value) const noexcept
        {
            type_traits<T>::push(_state, value);
            lua_setglobal(_state, name);
        }

        template <class T>
        inline void set_field(const char* name, const T& value) const noexcept
        {
            type_traits<T>::push(_state, value);
            lua_setfield(_state, -2, name);
        }

        /*inline bool is_garbage_collect_paused() const noexcept
        {
            auto value = lua_gc(shared_L.get(), LUA_GCSETPAUSE, 200);
            lua_gc(shared_L.get(), LUA_GCSETPAUSE, value);
            return value;
        }*/

        /*void Lua::LuaState::SetGarbageCollectMultiplier(int value) { lua_gc(shared_L.get(), LUA_GCSETSTEPMUL, std::max(100, value)); }

        int Lua::LuaState::GetGarbageCollectMultiplier()
        {
            auto value = lua_gc(shared_L.get(), LUA_GCSETSTEPMUL, 200);
            lua_gc(shared_L.get(), LUA_GCSETSTEPMUL, value);
            return value;
        }*/

        inline __fst::lua::type get_type(int n) const noexcept { return (__fst::lua::type) lua_type(_state, n); }

        inline bool is_type(__fst::lua::type t, int n) const noexcept { return t == get_type(n); }

        inline bool is_number(int idx) const noexcept { return lua_isnumber(_state, idx) != 0; }
        inline bool is_integer(int idx) const noexcept { return lua_isinteger(_state, idx) != 0; }
        inline bool is_string(int idx) const noexcept { return lua_isstring(_state, idx) != 0; }
        inline bool is_c_function(int idx) const noexcept { return lua_iscfunction(_state, idx) != 0; }
        inline bool is_user_data(int idx) const noexcept { return lua_isuserdata(_state, idx) != 0; }
        inline bool is_function(int idx) const noexcept { return lua_isfunction(_state, idx); }
        inline bool is_table(int idx) const noexcept { return lua_istable(_state, idx); }
        inline bool is_light_user_data(int idx) const noexcept { return lua_islightuserdata(_state, idx); }
        inline bool is_nil(int idx) const noexcept { return lua_isnil(_state, idx); }
        inline bool is_bool(int idx) const noexcept { return lua_isboolean(_state, idx); }
        inline bool is_thread(int idx) const noexcept { return lua_isthread(_state, idx); }
        inline bool is_none(int idx) const noexcept { return lua_isnone(_state, idx); }
        inline bool is_none_or_nil(int idx) const noexcept { return lua_isnoneornil(_state, idx); }

        //
        //
        //
        template <class T>
        inline T get(int index) const noexcept
        {
            return __fst::lua::type_traits<T>::get(_state, index);
        }

        template <class T>
        inline void push(const T& value) const noexcept
        {
            __fst::lua::type_traits<T>::push(_state, value);
        }

        ///
        __fst::error_result run_string(__fst::string_view code) const noexcept;

        ///
        __fst::error_result run_file(const char* filepath) const noexcept;

        ///
        __fst::error_result load_string(__fst::string_view code) const noexcept;

        //
        //
        //
        inline void garbage_collect() const noexcept { lua_gc(_state, LUA_GCCOLLECT, 0); }

        //
        inline __fst::lua::context_base* get_context_base() const noexcept { return (__fst::lua::context_base*) fst_lua_get_user_data(_state); }
        
        inline void* get_context_data() const noexcept { return get_context_base()->get_context_data(); }

        void* get_context_user_data() const noexcept { return get_context_base()->get_context_user_data(); }

        //
        //
        //

        template <auto Fct>
        inline void add_global_function(const char* name) const noexcept
        {
            set_global(name, (lua_CFunction) &state::state_function<Fct>);
        }

        template <auto Fct>
        inline void push_function() noexcept
        {
            push(&state::state_function<Fct>);
        }

        template <auto Fct>
        static inline lua_CFunction create_function() noexcept
        {
            return &state::state_function<Fct>;
        }

      private:
        __fst::lua::state_type _state = nullptr;

        template <class _StateParams, class _Fct, size_t... I>
        static inline auto call_state_function_impl(const _StateParams& p, _Fct fct, __fst::index_sequence<I...>) noexcept
        {
            return fct((p.template get<I>())...);
        }

        template <class _StateParams, class _Fct>
        static inline auto call_state_function(const _StateParams& p, _Fct fct) noexcept
        {
            return call_state_function_impl(p, fct, __fst::make_index_sequence<_StateParams::param_list::size()>{});
        }

        template <auto Fct>
        static inline int state_function(lua_State* L) noexcept;
    };

    template <class... _Args>
    class state_parameters : public state_parameters<__fst::type_list<_Args...>>
    {};

    template <class... _Args>
    class state_parameters<__fst::type_list<_Args...>>
    {
      public:
        using param_list = __fst::type_list<_Args...>;

        inline state_parameters(__fst::lua::state s, int index) noexcept
            : _state(s)
            , _start_index(index)
        {}

        template <size_t Index>
        inline param_list::template type_at<Index> get() const noexcept
        {
            return _state.get<param_list::template type_at<Index>>(_start_index + Index);
        }

        FST_NODISCARD FST_ALWAYS_INLINE __fst::lua::state state() const noexcept { return _state; }

      private:
        __fst::lua::state _state;
        int _start_index;
    };

    template <auto Fct>
    inline int state::state_function(lua_State * L) noexcept
    {
        __fst::lua::state s = L;

        using ftraits = fst::lua::call_traits<Fct>;
        if (s.top() != ftraits::args_size) { return luaL_error(s, "expecting exactly %d arguments", (int) ftraits::args_size); }

        if constexpr (__fst::is_same_v<typename ftraits::return_t, void>)
        {
            call_state_function(__fst::lua::state_parameters<typename ftraits::args_list>(s, 1), Fct);
            return 0;
        }
        else
        {
            typename ftraits::return_t ret = call_state_function(__fst::lua::state_parameters<typename ftraits::args_list>(s, 1), Fct);
            s.push(ret);
            return 1;
        }
    }

FST_END_SUB_NAMESPACE
