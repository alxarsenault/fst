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
#include "fst/pair.h"
#include "fst/string.h"
#include "fst/string_view.h"
#include "fst/stream.h"
#include "fst/memory.h"
#include "fst/status_code.h"

//
#include "fst/lua/common.h"
#include "fst/lua/state.h"
#include "fst/lua/register.h"

FST_BEGIN_SUB_NAMESPACE(lua)

    template <class _MemoryCategory, class _MemoryZone>
    class basic_context : context_base
    {
      public:
        using memory_zone_type = _MemoryZone;
        using storage_type = __fst::pair<__fst::lua::state, _MemoryZone>;
        using memory_zone_reference = typename storage_type::second_reference;
        using memory_zone_const_reference = typename storage_type::second_const_reference;

        using context_base::get_context_data;
        using context_base::get_context_user_data;
        using context_base::set_context_user_data;


        FST_NODISCARD FST_ALWAYS_INLINE memory_zone_const_reference get_memory_zone() const noexcept { return _data.second(); }
        FST_NODISCARD FST_ALWAYS_INLINE memory_zone_reference get_memory_zone() noexcept { return _data.second(); }

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_default_constructible_v<_Zone>, int> = 0>
        inline basic_context(bool open_libs = true) noexcept
            : context_base{ this, nullptr }
            , _data()
        {
            init(open_libs);
        }

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_copy_constructible_v<_Zone>, int> = 0>
        inline basic_context(const memory_zone_type& zone, bool open_libs = true) noexcept
            : context_base{ this, nullptr }
            , _data(__fst::default_construct_t{}, zone)
        {
            init(open_libs);
        }

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_move_constructible_v<_Zone>, int> = 0>
        inline basic_context(memory_zone_type&& zone, bool open_libs = true) noexcept
            : context_base{ this, nullptr }
            , _data(__fst::default_construct_t{}, __fst::move(zone))
        {
            init(open_libs);
        }

        template <class _Zone = memory_zone_type, __fst::enable_if_t<__fst::is_move_constructible_v<_Zone>, int> = 0>
        inline basic_context(basic_context&& ctx) noexcept
            : context_base{ this, nullptr }
            , _data(ctx.state(), __fst::move(ctx.get_memory_zone()))
        {
            ctx._data.first() = nullptr;
        }

        //1624, 1415
        //context() noexcept
        //{
        //    //1624
        //    // 2048
        //    _state = lua_newstate(
        //        [](void* ud, void* ptr, size_t osize, size_t nsize) noexcept -> void* { return ((context*) ud)->internal_alloc(ptr, osize, nsize); }, this);

        //    //_state = fst_lua_newstate([](void* ud, void* ptr, size_t osize, size_t nsize) noexcept -> void* { return ((context*) ud)->internal_alloc(ptr, osize, nsize); },
        //    //      this, _storage.data(), _storage.size());

        //    lua_atpanic(_state, &context::lua_pannic_callback);
        //    lua_setwarnf(
        //        _state, [](void* ud, const char* msg, int tocont) noexcept { ((context*) ud)->internal_warn(msg, tocont); }, this);
        //    luaL_openlibs(_state);
        //}

        inline ~basic_context() noexcept
        {
            if (state()) { lua_close(state()); }
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr __fst::lua::state_type get_state() const noexcept { return state().get_state(); }

        FST_NODISCARD FST_ALWAYS_INLINE __fst::lua::state state() const noexcept { return _data.first(); }

      

      private:
        storage_type _data;

        inline void init(bool open_libs) noexcept
        {
            _data.first() = lua_newstate([](void* ud, void* ptr, size_t osize, size_t nsize) noexcept -> void*
                { return ((basic_context*) ((context_base*) ud)->get_context_data())->internal_alloc(ptr, osize, nsize); },
                this->base());

            state_type s = _data.first();

            lua_atpanic(s, &basic_context::lua_pannic_callback);
            lua_setwarnf(
                s, [](void* ud, const char* msg, int tocont) noexcept { ((basic_context*) ((context_base*) ud)->get_context_data())->internal_warn(msg, tocont); },
                this->base());

            if (open_libs) { luaL_openlibs(s); }
        }

        void* internal_alloc(void* ptr, size_t osize, size_t nsize) noexcept
        {
            if (nsize == 0)
            {
                if (ptr) { _data.second().deallocate(ptr, _MemoryCategory::id()); }
                return nullptr;
            }
            else if (ptr && osize >= nsize) { return ptr; }

            void* pdata = _data.second().allocate(nsize, _MemoryCategory::id());
            if (!pdata) { return nullptr; }

            if (ptr)
            {
                __fst::memcpy(pdata, ptr, osize);
                _data.second().deallocate(ptr, _MemoryCategory::id());
            }

            return pdata;
        }

        void internal_warn(const char* msg, FST_ATTRIBUTE_UNUSED int tocont) noexcept{ fst::print("DSLJDSKLDJLKSD", msg); }

        static int lua_pannic_callback(state_type s) noexcept
        {
            const char* msg = lua_tostring(s, -1);
            if (msg == NULL) msg = "error object is not a string";
            lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n", msg);
            return 0; /* return to Lua to abort */
        }
    };

    

FST_END_SUB_NAMESPACE
