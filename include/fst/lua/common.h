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
#include "fst/memory.h"

//
#include "fst/lua/lua.h"
#include "fst/lua/lualib.h"
#include "fst/lua/lauxlib.h"

FST_BEGIN_NAMESPACE
FST_DECLARE_MEMORY_CATEGORY(lua_memory_category, "lua");
FST_END_NAMESPACE

FST_BEGIN_SUB_NAMESPACE(lua)

    using state_type = lua_State*;

    class state;
    
    template <class... _Args>
    class state_parameters;

    template <class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    class basic_context;

    using context = basic_context<__fst::default_memory_category, __fst::default_memory_zone>;

    using inplace_memory_zone = __fst::void_memory_zone;

    struct table_tag
    {};

    

    enum class type : int8_t {
        none = LUA_TNONE,
        nil = LUA_TNIL,
        boolean = LUA_TBOOLEAN,
        light_user_data = LUA_TLIGHTUSERDATA,
        number = LUA_TNUMBER,
        integer = LUA_TNUMBER,
        string = LUA_TSTRING,
        table = LUA_TTABLE,
        function = LUA_TFUNCTION,
        user_data = LUA_TUSERDATA,
        thread = LUA_TTHREAD
    };

    class context_base
    {
      public:
        template <class _Context>
        FST_NODISCARD FST_ALWAYS_INLINE _Context* get_context_data() const noexcept
        {
            return (_Context*) _context_data;
        }

        FST_NODISCARD FST_ALWAYS_INLINE void* get_context_data() const noexcept { return _context_data; }

        template <class _T>
        FST_NODISCARD FST_ALWAYS_INLINE _T* get_context_user_data() const noexcept
        {
            return (_T*) _user_data;
        }

        FST_NODISCARD FST_ALWAYS_INLINE void* get_context_user_data() const noexcept { return _user_data; }

        inline void set_context_user_data(void* udata) noexcept
        {
            _user_data = udata;
        }

      protected:
        inline context_base(void* ctx, void* udata) noexcept
            : _context_data(ctx)
            , _user_data(udata)
        {}

        FST_NODISCARD FST_ALWAYS_INLINE context_base* base() noexcept { return this; }
        FST_NODISCARD FST_ALWAYS_INLINE const context_base* base() const noexcept { return this; }

      private:
        void* _context_data;
        void* _user_data;
    };

    enum class function_type {
        normal,
        member
    };

FST_END_SUB_NAMESPACE
