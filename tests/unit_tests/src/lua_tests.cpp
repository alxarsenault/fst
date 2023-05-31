#include "utest.h"
#include "fst/array.h"
#include "fst/profiler.h"
#include "fst/trace.h"
#include "fst/file_view.h"
#include "fst/memory_pool.h"

#include "fst/lua.h"

namespace
{
    struct my_obj
    {
        my_obj()
            : _value(0)
        {
            fst::print("my_obj::my_obj::default", _value);
        }

        my_obj(int v)
            : _value(v)
        {
            fst::print("my_obj::my_obj", _value);
        }

        ~my_obj() { fst::print("my_obj::~my_obj"); }

        inline void check(int a) { fst::print("check", a, _value); }

        static void ggg(my_obj* obj, int b) { fst::print("ggg", obj->_value, b); }

        static void fff(int b) { fst::print("fff", b); }

        int _value;
    };

    void print_this(int a)
    {
        fst::print("print_this", a);
    }

    TEST_CASE("fst::lua::module", "[script]")
    {
        fst::lua::context ctx;
        fst::lua::state s = ctx.state();

        fst::lua::module_register<"fst"> module(s);
        module.add_static_function<&print_this>("print_this");
        module.import();

        if (fst::error_result err = s.run_string("fst.print_this(32);")) { fst::print(err.message()); }

        fst::lua::class_register<my_obj, "my_obj">(s).add_constructor<int>("new").import("fst");
        if (fst::error_result err = s.run_string("a = fst.my_obj.new(354);")) { fst::print(err.message()); }

    }

    TEST_CASE("fst::lua::classgg", "[script]")
    {
        fst::lua::context ctx;
        fst::lua::state s = ctx.state();

        s.set_global("ctx", fst::lua::table_tag{});
        s.get_global("ctx");
        s.set_field("a", "alex");
        s.set_field("b", 5);

        s.pop();

        if (fst::error_result err = s.run_string("print(ctx.a);")) { fst::print(err.message()); }

        fst::lua::class_register<my_obj, "my_obj">(s).add_constructor<int>("new").import("ctx");

        if (fst::error_result err = s.run_string("a = ctx.my_obj.new(354);")) { fst::print(err.message()); }
        //if (fst::error_result err = s.run_string("print(a.bingo);")) { fst::print(err.message()); }

        if (fst::error_result err = s.run_string("print(os.clock());")) { fst::print(err.message()); }
    }
    TEST_CASE("fst::lua::class", "[script]")
    {
        fst::lua::context ctx;
        fst::lua::state s = ctx.state();

        void* ctx_data = s.get_context_data();
        REQUIRE(ctx_data == (void*) &ctx);

        fst::lua::class_register<my_obj, "my_obj"> reg(s);

        reg.add_constructor<>("enew");
        reg.add_constructor<int>("new");
        reg.add_member_function<&my_obj::check>("check");
        reg.add_custom_member_function<&my_obj::ggg>("ggg");
        reg.add_static_function<&my_obj::fff>("fff");

        reg.add_global_constructor<int>("MyObj");
        reg.import();

        if (fst::error_result err = s.run_string("aa = MyObj(354);")) { fst::print(err.message()); }
        if (fst::error_result err = s.run_string("a = my_obj.new(34);")) { fst::print(err.message()); }
        if (fst::error_result err = s.run_string("a:check(56);")) { fst::print(err.message()); }
        if (fst::error_result err = s.run_string("a:ggg(59);")) { fst::print(err.message()); }
        if (fst::error_result err = s.run_string("a.fff(69);")) { fst::print(err.message()); }
        if (fst::error_result err = s.run_string("b = my_obj.enew();")) { fst::print(err.message()); }
    }

    template <class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    struct lua_pool_zone : fst::memory_zone<lua_pool_zone<_MemoryCategory, _MemoryZone>>
    {
        static constexpr const char* name = "lua_pool";

        lua_pool_zone(size_t buckets, size_t bucket_size) noexcept
            : _pool(buckets, bucket_size)
        {}

        //lua_pool_zone(const pool_memory_zone& other) noexcept;
        //lua_pool_zone(pool_memory_zone&& other) noexcept;

        //~lua_pool_zone() noexcept;

        //lua_pool_zone& operator=(const lua_pool_zone& other) noexcept;
        //lua_pool_zone& operator=(lua_pool_zone&& other) noexcept;

        FST_NODISCARD void* allocate(size_t size, __fst::memory_category_id) const noexcept { return _pool.aligned_allocate(size, _pool.minimum_alignment); }
        void deallocate(void* ptr, __fst::memory_category_id) const noexcept { _pool.aligned_deallocate(ptr); }

        FST_NODISCARD void* aligned_allocate(size_t size, size_t alignment, __fst::memory_category_id) const noexcept { return _pool.aligned_allocate(size, alignment); }
        void aligned_deallocate(void* ptr, __fst::memory_category_id) const noexcept { _pool.aligned_deallocate(ptr); }

      private:
        mutable fst::small_memory_pool<_MemoryCategory, _MemoryZone> _pool;
    };

    class Foo
    {
      public:
        inline Foo(const fst::string& name)
            : name(name)
        {
            fst::cout << "Foo is born" << fst::endl;
        }

        inline fst::string Add(int a, int b)
        {
            fst::string s;
            fst::output_stream<char> ss = fst::string_stream(s);
            ss << name << ": " << a << " + " << b << " = " << (a + b);
            return s;
        }

        inline ~Foo() { fst::cout << "Foo is gone" << fst::endl; }

      private:
        fst::string name;
    };

    // The general pattern to binding C++ class to Lua is to write a Lua
    // thunk for every method for the class, so here we go:

    int l_Foo_constructor(lua_State* L)
    {
        const char* name = luaL_checkstring(L, 1);

        // We could actually allocate Foo itself as a user data but
        // since user data can be GC'ed and we gain unity by using CRT's heap
        // all along.
        Foo** udata = (Foo**) lua_newuserdatauv(L, sizeof(Foo*), 1);
        *udata = new Foo(name);

        // Usually, we'll just use "Foo" as the second parameter, but I
        // say luaL_Foo here to distinguish the difference:
        //
        // This 2nd parameter here is an _internal label_ for luaL, it is
        // _not_ exposed to Lua by default.
        //
        // Effectively, this metatable is not accessible by Lua by default.
        // luaL_getmetatable(l, "luaL_Foo");
        lua_getfield(L, LUA_REGISTRYINDEX, "luaL_Foo");

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

    Foo* l_CheckFoo(lua_State* L, int n)
    {
        // This checks that the argument is a userdata with the metatable "luaL_Foo"
        return *(Foo**) luaL_checkudata(L, n, "luaL_Foo");
    }

    int l_Foo_add(lua_State* L)
    {
        Foo* foo = l_CheckFoo(L, 1);
        int a = (int) luaL_checknumber(L, 2);
        int b = (int) luaL_checknumber(L, 3);

        fst::string s = foo->Add(a, b);
        lua_pushstring(L, s.c_str());

        // The Lua stack at this point looks like this:
        //
        //     4| result string          |-1
        //     3| metatable "luaL_foo"   |-2
        //     2| userdata               |-3
        //     1| string parameter       |-4
        //
        // Return 1 to return the result string to Lua callsite.

        return 1;
    }

    int l_Foo_destructor(lua_State* L)
    {
        Foo* foo = l_CheckFoo(L, 1);
        delete foo;

        return 0;
    }

    void RegisterFoo(lua_State* L)
    {
        //function newButton () return setmetatable({},{__index=Button}) end
        luaL_Reg sFooRegs[] = { { "new", l_Foo_constructor }, { "add", l_Foo_add }, { "__gc", l_Foo_destructor }, { nullptr, nullptr } };

        // Create a luaL metatable.
        // This metatable is not exposed to Lua.
        // The "luaL_Foo" label is used by luaL internally to identity things.
        luaL_newmetatable(L, "luaL_Foo");

        // Register the C functions _into_ the metatable we just created.
        luaL_setfuncs(L, sFooRegs, 0);

        // The Lua stack at this point looks like this:
        //
        //     1| metatable "luaL_Foo"   |-1
        lua_pushvalue(L, -1);

        // The Lua stack at this point looks like this:
        //
        //     2| metatable "luaL_Foo"   |-1
        //     1| metatable "luaL_Foo"   |-2

        // Set the "__index" field of the metatable to point to itself.
        // This pops the stack
        lua_setfield(L, -1, "__index");

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
        lua_setglobal(L, "Foo");
    }

    static int l_create_foo(lua_State* L)
    {
        if (lua_gettop(L) != 1)
        {
            //lua_pushinteger(L, 0);
            //return 1;
            return luaL_error(L, "expecting exactly 1 arguments");
        }

        const char* name = luaL_checkstring(L, 1);

        // We could actually allocate Foo itself as a user data but
        // since user data can be GC'ed and we gain unity by using CRT's heap
        // all along.
        Foo** udata = (Foo**) lua_newuserdata(L, sizeof(Foo*));
        *udata = new Foo(name);

        // Usually, we'll just use "Foo" as the second parameter, but I
        // say luaL_Foo here to distinguish the difference:
        //
        // This 2nd parameter here is an _internal label_ for luaL, it is
        // _not_ exposed to Lua by default.
        //
        // Effectively, this metatable is not accessible by Lua by default.
        luaL_getmetatable(L, "luaL_Foo");

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

    void my_function(fst::string_view a, double b)
    {
        fst::print("my_function", a, b);
    }

    /* static int l_call_my_function(lua_State* L)
    {

    }*/

    //template <auto Fct, class Ret, class P1, class P2>
    //inline void create_function_call(fst::lua::state s, const char* name, Ret (*my_fct)(P1, P2))
    //{ //static int l_create_foo(lua_State * L)

    //    lua_CFunction fct = [](lua_State* L) -> int
    //    {
    //        fst::lua::state s = L;

    //        if (s.top() != 2)
    //        {
    //            //lua_pushinteger(L, 0);
    //            //return 1;
    //            return luaL_error(s, "expecting exactly 2 arguments");
    //        }

    //        P1 p1 = s.get<P1>(1);
    //        P2 p2 = s.get<P2>(2);

    //        Fct(p1, p2);

    //        return 0;
    //    };

    //    s.set_global(name, fct);
    //}

    //template <auto Fct, class Ret, class ..._Args>
    //inline void create_function_call_t(fst::lua::state s, const char* name, Ret (*my_fct)(_Args...))
    //{
    //    lua_CFunction fct = [](lua_State* L) -> int
    //    {
    //        fst::lua::state s = L;

    //        if (s.top() != sizeof...(_Args))
    //        {
    //            return luaL_error(s, "expecting exactly %d arguments", (int)sizeof...(_Args));
    //        }

    //        fst::lua::call_state_function(fst::lua::state_parameters<_Args...>(s, 1), Fct);
    //        return 0;
    //    };

    //    s.set_global(name, fct);
    //}

    //template <auto Fct>
    //inline void create_function_call2(fst::lua::state s, const char* name)
    //{
    //    return create_function_call_t<Fct>(s, name, Fct);
    //}

    inline void my_ff(int, int) {}

    struct my_struct
    {
        inline int banana(double) { return 0; }
    };

    TEST_CASE("fst::lua::a", "[script]"){ { using ftraits = fst::lua::call_traits<&my_ff>;
    REQUIRE(fst::is_same_v<typename ftraits::return_t, void>);
    REQUIRE(fst::is_same_v<typename ftraits::args_list, fst::type_list<int, int>>);
    REQUIRE(ftraits::ftype == fst::lua::function_type::normal);
} // namespace

{
    using ftraits = fst::lua::call_traits<&my_struct::banana>;
    REQUIRE(fst::is_same_v<typename ftraits::return_t, int>);
    REQUIRE(fst::is_same_v<typename ftraits::args_list, fst::type_list<double>>);
    REQUIRE(ftraits::ftype == fst::lua::function_type::member);
}

FST_PRAGMA_PUSH()
FST_PRAGMA_DISABLE_WARNING_MSVC(5264)
{
    constexpr auto f = [](int) -> double
    {
        return 0.0;
    };

    using ftraits = fst::lua::call_traits<f>;

    REQUIRE(fst::is_same_v<typename ftraits::return_t, double>);
    REQUIRE(fst::is_same_v<typename ftraits::args_list, fst::type_list<int>>);
    REQUIRE(ftraits::ftype == fst::lua::function_type::normal);
}

FST_PRAGMA_POP()
}

TEST_CASE("fst::lua::", "[script]")
{
    using pool_type = lua_pool_zone<fst::default_memory_category, fst::default_memory_zone>;
    using context_type = fst::lua::basic_context<fst::default_memory_category, fst::memory_zone_proxy>;

    pool_type pool(8, 8);
    context_type ctx(pool.proxy());
    fst::lua::state s = ctx.state();

    s.add_global_function<&my_function>("banana");

    if (fst::error_result err = s.run_string("banana('Alex', 3.2);")) { fst::print(err.message()); }

    constexpr auto fff = [](fst::string_view a, double b) -> double
    {
        fst::print("my_function_lamda", a, b);
        return 2.0 * b;
    };

    s.add_global_function<fff>("fff");

    if (fst::error_result err = s.run_string("print(fff('Alex', 3.2));")) { fst::print(err.message()); }

    /*using fct_type = void (*)(fst::string_view, double);

        create_function_call<fff>(st, "my_func", (fct_type) fff);

        create_function_call2<(fct_type) fff>(st, "my_func2");
        if (fst::error_result err = st.run_string("my_func2('Alex', 3.2);")) { fst::print(err.message()); }

        if (fst::error_result err = st.run_string("my_func('john', 33.2);")) { fst::print(err.message()); }*/

    s.run_string("bingo = 23;");

    int stack_base = s.top();
    fst::lua::type gg = s.get_global(fst::string_view("bingo"));

    REQUIRE(gg == fst::lua::type::number);
    REQUIRE(s.get_type(stack_base + 1) == fst::lua::type::number);

    s.pop();

    RegisterFoo(s);

    s.set_global("Faa", &l_create_foo);

    if (fst::error_result err = s.run_string("myfoo = Foo.new('alex');")) { fst::print(err.message()); }

    if (fst::error_result err = s.run_string("myfoo2 = Faa('alex');")) { fst::print(err.message()); }

    if (fst::error_result err = s.run_string("print(myfoo2:add(2, 3));")) { fst::print(err.message()); }

    //local foo = MyObject.new();
}
//static int l_fst_check(lua_State* L)
//{
//    bool v = (bool) lua_toboolean(L, 1);
//    REQUIRE(v);
//    return 0;
//}

//static int l_check_string_param(lua_State* L)
//{
//    if (lua_gettop(L) != 1) {
//        lua_pushinteger(L, 0);
//        return 1;
//        //return luaL_error(L, "expecting exactly 2 arguments");
//    }

//    fst::string_view str = luaL_checkstring(L, 1);
//    REQUIRE(str == "banana");
//    lua_pushinteger(L, 33);
//    return 1;
//}

//TEST_CASE("fst::lua::", "[script]")
//{
//    //https://subscription.packtpub.com/book/programming/9781789343229/6/ch06lvl1sec52/working-with-tables-in-c
//    fst::script::engine engine;

//    // Add check function.
//    engine.set_global("fst_check", &l_fst_check);

//    engine.set_global("fst_check_string_param", &l_check_string_param);

//    // Add glob_str = 'patate'
//    engine.set_global("glob_str", "patate");
//    engine.run_string("fst_check(glob_str == 'patate');");

//    // Add ctx = {a: 'alex', b: 5}
//    engine.set_global("ctx", fst::script::table_tag{});
//    engine.get_global("ctx");
//    engine.set_field("a", "alex");
//    engine.set_field("b", 5);

//    engine.pop();

//    engine.run_string("fst_check(ctx.a == 'alex');");
//    engine.run_string("fst_check(ctx.b == 5);");

//   engine.run_string("fst_check(fst_check_string_param('banana') == 33);");

//   engine.run_string("peter = 34;");
//   engine.run_string("michel = 'abc';");
//
//   int stack_base = engine.top();
//   engine.get_global("peter"); // number
//   engine.get_global("michel"); // string
//   engine.get_global("bingo"); // nil

//   REQUIRE(engine.get_type(stack_base + 1) == fst::script::types::number);
//   REQUIRE(engine.get_type(stack_base + 2) == fst::script::types::string);
//   REQUIRE(engine.get_type(stack_base + 3) == fst::script::types::nil);

//   {

//        fst::script::var v0(engine.state(), stack_base + 1);
//        int value0 = v0.to_value<int>();
//        REQUIRE(value0 == 34);
//   }

//   {
//        fst::script::var v0(engine.state(), stack_base + 2);
//        fst::string_view value0 = v0.to_value<fst::string_view>();
//        REQUIRE(value0 == "abc");
//   }

//   engine.pop(3);

//   //engine.get_global("michel");

//   //const char* class_p = lua_tostring(L, stack_base + 1);
//   //engine.get_global("ctx");

//}

//TEST_CASE("fst::lua", "[script]")
//{
//    const char* fpath = FST_TEST_RESOURCES_DIRECTORY "/script.lua";

//    fst::script::engine engine;
//    engine.run_string("print('skalskalksaklsa')");
//    engine.run_string("prinjkk')");

//    engine.set_global("john", "patate");

//    engine.set_global("ctx", fst::script::table_tag{});

//    engine.get_global("ctx");
//    engine.set_field("a", "kk");

//    engine.run_file(fpath);
//}
} // namespace
