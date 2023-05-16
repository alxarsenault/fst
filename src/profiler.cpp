#include "fst/profiler.h"

#if FST_USE_PROFILER
#include "fst/small_vector.h"
#include "fst/string.h"
#include "fst/unordered_map.h"
#include "fst/time.h"
#include <stdio.h>

#if __FST_MSVC__
#pragma section("fst_profiler", read, write)
#define FST_PROFILER_SECTION __declspec(allocate("fst_profiler"))
#else
#define FST_PROFILER_SECTION
#endif //

namespace prof_detail
{
    template <class _Key, class _Value>
    using unordered_map = __fst::small_unordered_map<_Key, _Value, 32, alignof(_Value), __fst::profiler_memory_zone,
        __fst::profiler_memory_category>; //, std::hash<_Key>, std::equal_to<_Key>, map_allocator<_Key, _Value>>;

    //template <class _Key, class _Value>
    //using unordered_map = __fst::unordered_map<_Key, _Value, alignof(_Value), __fst::profiler_memory_zone,
    //__fst::profiler_memory_category>; //, std::hash<_Key>, std::equal_to<_Key>, map_allocator<_Key, _Value>>;

} // namespace prof_detail

FST_BEGIN_NAMESPACE

#if FST_PROFILER_WARNING_IS_ERROR
#define FST_PROFILER_WARNING(...) fst_error(__VA_ARGS__)
#else
#define FST_PROFILER_WARNING(...) fst_warning(__VA_ARGS__)
#endif //

    namespace
    {
        struct tracer
        {
            tracer()
                : _stream{ this, [](void* data, const char* str, size_t size, stream_modifier)noexcept-> size_t
                    {
                        return ::fwrite(str, size, 1, ((tracer*) data)->_file);
                    } }

            {
                _file = ::fopen(FST_PROFILER_LOG_PATH, "a+");
            }

            ~tracer() { ::fclose(_file); }

            FILE* _file;
            __fst::output_stream<char> _stream;
        };

        FST_PRAGMA_PUSH()
        FST_PRAGMA_DISABLE_WARNING_CLANG("-Wexit-time-destructors")
        static inline tracer& get_tracer() noexcept
        {
            FST_PROFILER_SECTION
            static tracer _tracer;
            return _tracer;
        }
        FST_PRAGMA_POP()

    } // namespace.

    __fst::output_stream<char>& profiler::tracer(const char* section)
    {
        static size_t count = 0;

#if FST_USE_PROFILER_COLOR

        __fst::term_color col = __fst::term_color::white;
        if (__fst::strncmp(section, "profiler", 8) == 0) { col = __fst::term_color::cyan; }
        else if (__fst::strncmp(section, "unit_test", 9) == 0) { col = __fst::term_color::green; }

        else if (__fst::strncmp(section, "memory", 6) == 0) { col = __fst::term_color::yellow; }

        else if (__fst::strncmp(section, "section", 7) == 0) { col = __fst::term_color::magenta; }

        return get_tracer()._stream << __fst::stream_detail::sep_t{} << __fst::padded_right<6>(++count) << __fst::stream_detail::sep_t{} << __fst::term_color::cyan
                                    << __fst::format_time_ms(std::chrono::system_clock::now()) << __fst::term_color::reset << __fst::stream_detail::sep_t{} << col
                                    << __fst::term_color::bold << __fst::padded<14>(section) << __fst::term_color::reset;
#else

        return get_tracer()._stream << __fst::separator << __fst::padded_right<6>(++count) << __fst::separator << __fst::system_clock::now() << __fst::separator
                                    << __fst::padded<14>(section);
#endif
    }

    struct profiler_content
    {
        using string_type = __fst::basic_string<char, __fst::allocator<char, __fst::profiler_memory_category, __fst::profiler_memory_zone>>;
        using vector_type = __fst::small_vector<string_type, 32, alignof(string_type), __fst::profiler_memory_zone, __fst::profiler_memory_category>;
        vector_type zones;
        vector_type categories;

        inline const char* get_category_name(__fst::memory_category_id mid) const noexcept
        {
            return categories.size() > mid ? categories[(size_t) mid].c_str() : "unknown";
        }

        inline void add_category(__fst::memory_category_id mid, const char* name) noexcept
        {
            if (categories.size() <= mid) categories.resize((size_t) mid + 1);

            categories[(size_t) mid] = name;
        }

        inline const char* get_zone_name(__fst::memory_zone_id zid) const noexcept { return zones.size() > zid ? zones[(size_t) zid].c_str() : "unknown"; }

        inline void add_zone(__fst::memory_zone_id zid, const char* name) noexcept
        {
            if (zones.size() <= zid) zones.resize((size_t) zid + 1);
            zones[(size_t) zid] = name;
        }
    };

    FST_PRAGMA_PUSH()
    FST_PRAGMA_DISABLE_WARNING_CLANG("-Wexit-time-destructors")
    static profiler_content& get_profiler_names() noexcept
    {
        FST_PROFILER_SECTION
        static profiler_content _content;
        return _content;
    }
    FST_PRAGMA_POP()

    struct item
    {
        size_t size;
    };

    struct pointer_map : prof_detail::unordered_map<void*, item>
    {
        int64_t _used = 0;

        inline void add(void* ptr, item it)
        {
            if (auto f = this->find(ptr); f != this->end())
            {
                fst_error("pointer reallocation");
                f->size += it.size;
            }
            else { this->operator[](ptr) = it; }

            _used += it.size;
        }

        inline void remove(void* ptr)
        {
            if (ptr == nullptr) { return; }

            if (auto f = this->find(ptr); f != this->end())
            {
                _used -= f->size;
                this->erase(f);
            }
            else { fst_error("free error"); }
        }

        inline void remove(iterator it)
        {
            fst_assert(it != this->end());
            _used -= it->size;
            this->erase(it);
        }
    };

    struct profiler_data
    {
        using category_vector = __fst::small_vector<pointer_map, 32, alignof(pointer_map), __fst::profiler_memory_zone, __fst::profiler_memory_category>;
        using zone_vector = __fst::small_vector<category_vector, 32, alignof(category_vector), __fst::profiler_memory_zone, __fst::profiler_memory_category>;

        zone_vector zones;

        inline category_vector* get_zone(__fst::memory_zone_id zid) noexcept { return zones.size() > zid ? &zones[(size_t) zid] : nullptr; }

        inline category_vector& operator[](__fst::memory_zone_id zid) noexcept
        {
            if (zones.size() <= zid) zones.resize((size_t) zid + 1);

            return zones[(size_t) zid];
        }
    };

    FST_PRAGMA_PUSH()
    FST_PRAGMA_DISABLE_WARNING_CLANG("-Wexit-time-destructors")
    static profiler_data& get_profiler_zones() noexcept
    {
        FST_PROFILER_SECTION
        static profiler_data _data;
        return _data;
    }
    FST_PRAGMA_POP()

    struct impl
    {
        enum class item_type {
            alloc,
            dealloc
        };

        struct item_content
        {
            __fst::memory_category_id mid;
            size_t size;
            void* ptr;
            time_point time;
            item_type type;

            friend inline __fst::output_stream<char>& operator<<(__fst::output_stream<char>& stream, const item_content& it)
            {
                stream << __fst::space_padding(14, ((it.type == impl::item_type::alloc) ? "alloc   " : "dealloc ")) << __fst::space_padding(14, it.size) << " "
                       << __fst::space_padding(14, (uintptr_t) it.ptr) << " ";

                FST_PRAGMA_PUSH()
                FST_PRAGMA_DISABLE_WARNING_CLANG("-Wctad-maybe-unsupported")
                stream << it.time;
                FST_PRAGMA_POP()

                return stream;
            }
        };

        impl() {}

        static inline void trace_event(const char* zone_name, const char* cat_name, void* ptr, size_t size, item_type type)
        {
            const char* label = type == impl::item_type::alloc ? "alloc" : "dealloc";
            __fst::profiler::trace("memory", __fst::padded<14>(zone_name ? zone_name : "unknown"), __fst::padded<32>(label), __fst::padded_right<14>(size),
                __fst::padded_right<14>((uintptr_t) ptr), __fst::padded<14>(cat_name ? cat_name : "unknown"));
        }

        static inline void add_event(__fst::memory_zone_id zid, item_content it)
        {
            const char* zone_name = get_profiler_names().get_zone_name(zid);
            if (!zone_name) { zone_name = "unknown memory zone"; }

            const char* category_name = get_profiler_names().get_category_name(it.mid);
            if (!category_name) { category_name = "unknown memory category"; }

            trace_event(zone_name, category_name, it.ptr, it.size, it.type);
        }

        static inline void add_profiler_event(
            FST_ATTRIBUTE_UNUSED __fst::memory_zone_id zid, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid, void* ptr, size_t size, item_type type)
        {
            trace_event("profiler", "profiler", ptr, size, type);
        }

        static inline bool has_pointers(const pointer_map& it) { return !it.empty(); }

        void move_allocation(void* ptr, __fst::memory_zone_id zid, __fst::memory_category_id from_mid, FST_ATTRIBUTE_UNUSED __fst::memory_category_id to_mid)
        {
            if (auto z = get_profiler_zones().get_zone(zid))
            {
                if (auto c = (z->size() > from_mid ? &z->operator[]((size_t) from_mid) : nullptr))
                {
                    if (typename pointer_map::iterator ptr_it = c->find(ptr); ptr_it != c->end())
                    {
                        // add_event(item_content{ mid, ptr_it->second.size, ptr,
                        // clock::now(), item_type::dealloc });
                        //get_profiler_zones().zones
                        //
                        //
                        //

                        // TODO:
                        //
                        //z->operator[](to_mid)[ptr] = ptr_it;
                        c->remove(ptr_it);
                        return;
                    }
                }
                //if (typename profiler_data::category_map::iterator it = z->find(from_mid); it != z->second.end())
                //    {
                //        if (typename pointer_map::iterator ptr_it = it->second.find(ptr); ptr_it != it->second.end())
                //        {
                //            // add_event(item_content{ mid, ptr_it->second.size, ptr,
                //            // clock::now(), item_type::dealloc });
                //            z->second[to_mid][ptr] = ptr_it->second;
                //            it->second.remove(ptr_it);
                //            return;
                //        }
                //}
            }

            fst_error("DLKJDAKLJDA");
        }

        static void allocated(void* ptr, size_t size, __fst::memory_zone_id zid, __fst::memory_category_id mid, const char* name)
        {
            if (ptr == nullptr)
            {
                if (size)
                {
                    fst_error("allocating on nullptr");
                    return;
                }

                if (name)
                {
                    if (zid == __fst::invalid_memory_zone) { get_profiler_names().add_category(mid, name); }
                    else if (mid == __fst::invalid_memory_category) { get_profiler_names().add_zone(zid, name); }
                    return;
                }

                fst_error("wrong allocation");
                return;
            }

            add_event(zid, item_content{ mid, size, ptr, __fst::system_clock::now(), item_type::alloc });

            auto& z = get_profiler_zones()[zid];
            if (z.size() <= mid) { z.resize((size_t) mid + 1); }

            z[(size_t) mid].add(ptr, item{ size });
        }

        struct pointer_result
        {
            __fst::memory_zone_id zid;
            __fst::memory_category_id mid;
            pointer_map* ptr_map;
            typename pointer_map::iterator ptr_iterator;

            inline explicit operator bool() const noexcept { return ptr_map != nullptr; }
        };

        static pointer_result find_ptr(void* ptr)
        {
            for (size_t i = 0; i < get_profiler_zones().zones.size(); i++)
            {
                auto& z = get_profiler_zones().zones[i];

                size_t n = 0;
                for (auto& k : z)
                {
                    if (typename pointer_map::iterator pit = k.find(ptr); pit != k.end())
                    {
                        return pointer_result{ (__fst::memory_zone_id) i, (__fst::memory_category_id) n, &k, pit };
                    }

                    n++;
                }
            }

            return { __fst::invalid_memory_zone, __fst::invalid_memory_category, nullptr, {} };
        }

        static pointer_result find_ptr(__fst::memory_zone_id zid, void* ptr)
        {
            if (auto z = get_profiler_zones().get_zone(zid))
            {
                size_t n = 0;

                for (auto& k : *z)
                {
                    if (typename pointer_map::iterator pit = k.find(ptr); pit != k.end()) { return pointer_result{ zid, (__fst::memory_category_id) n, &k, pit }; }

                    n++;
                }
            }

            return find_ptr(ptr);
        }

        static void deallocated(void* ptr, __fst::memory_zone_id zid, __fst::memory_category_id mid)
        {
            enum class state_enum {
                searching,
                cant_find_memory_zone,
                cant_find_memory_category,
                cant_find_pointer
            };

            state_enum state = state_enum::searching;

            if (auto z = get_profiler_zones().get_zone(zid))
            {
                if (auto it = (z->size() > mid ? &z->operator[]((size_t) mid) : nullptr))
                {
                    if (typename pointer_map::iterator ptr_it = it->find(ptr); ptr_it != it->end())
                    {
                        add_event(zid, item_content{ mid, ptr_it->size, ptr, __fst::system_clock::now(), item_type::dealloc });
                        it->remove(ptr_it);
                        return;
                    }
                    else { state = state_enum::cant_find_pointer; }
                }
                else { state = state_enum::cant_find_memory_category; }

                __fst::unused(state);

                if (pointer_result sr = find_ptr(zid, ptr))
                {
                    add_event(zid, item_content{ mid, sr.ptr_iterator->size, ptr, __fst::system_clock::now(), item_type::dealloc });
                    sr.ptr_map->remove(sr.ptr_iterator);
                    __fst::profiler::trace("memory", " warning wrong dealloc");
                    FST_PROFILER_WARNING("WRONG DEALLOC MEMORY CATEGORY");
                    return;
                }
                else { state = state_enum::cant_find_memory_zone; }
            }

            if (pointer_result sr = find_ptr(ptr))
            {
                add_event(zid, item_content{ mid, sr.ptr_iterator->size, ptr, __fst::system_clock::now(), item_type::dealloc });
                sr.ptr_map->remove(sr.ptr_iterator);
                __fst::profiler::trace("memory", " warning wrong dealloc");

                FST_PROFILER_WARNING("WRONG DEALLOC MEMORY ZONE");
                return;
            }

            __fst::profiler::trace("memory", " warning wrong dealloc");

            fst_error("WRONG DEALLOC");
        }
    };

    FST_PRAGMA_PUSH()
    FST_PRAGMA_DISABLE_WARNING_CLANG("-Wexit-time-destructors")
    static inline impl& get_profiler_impl() noexcept
    {
        FST_PROFILER_SECTION
        static impl _impl;
        return _impl;
    }
    FST_PRAGMA_POP()

    void profiler::move_allocation(void* ptr, __fst::memory_zone_id zid, __fst::memory_category_id from_mid, __fst::memory_category_id to_mid)
    {
        get_profiler_impl().move_allocation(ptr, zid, from_mid, to_mid);
    }

    void profiler::allocated(void* ptr, size_t size, __fst::memory_zone_id zid, __fst::memory_category_id mid, const char* name)
    {
        impl::allocated(ptr, size, zid, mid, name);
    }

    void profiler::deallocated(void* ptr, __fst::memory_zone_id zid, __fst::memory_category_id mid)
    {
        impl::deallocated(ptr, zid, mid);
    }

    profiler::result_type profiler::report()
    {
        result_type results;

        size_t z_count = 0;
        for (const auto& zit : get_profiler_zones().zones)
        {
            size_t c_count = 0;
            for (const auto& it : zit)
            {
                if (!it.empty())
                {
                    for (auto ptr_it : it)
                    {
                        results.push_back({ (__fst::memory_zone_id) z_count, (__fst::memory_category_id) c_count, ptr_it.size });
                    }
                }
                c_count++;
            }

            z_count++;
        }

        return results;
    }

    void profiler::output(__fst::output_stream<char> & stream)
    {
        for (const auto& zit : get_profiler_zones().zones)
        {
            size_t c_count = 0;
            for (const auto& it : zit)
            {
                if (!it.empty())
                {
                    stream << "Wrong deallocation" << __fst::endl;
                    stream << get_profiler_names().get_category_name((__fst::memory_category_id) c_count) << " " << it._used << __fst::endl;

                    for (size_t i = 0; i < it.size(); i++)
                    {
                        stream << "    " << it.keys()[i] << " " << it.values()[i].size << __fst::endl;
                    }
                }

                c_count++;
            }
        }
    }

    void* profiler_memory_zone::allocate(size_t size, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        void* ptr = ::malloc(size);
        FST_IF_PROFILE(impl::add_profiler_event(profiler_memory_zone::id(), mid, ptr, size, impl::item_type::alloc));
        return ptr;
    }

    void profiler_memory_zone::deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        FST_IF_PROFILE(impl::add_profiler_event(profiler_memory_zone::id(), mid, ptr, 0, impl::item_type::dealloc));
        ::free(ptr);
    }

    void* profiler_memory_zone::aligned_allocate(size_t size, size_t alignment, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
#if __FST_WINDOWS__
        void* ptr = ::_aligned_malloc(size, alignment);
#else
        void* ptr = ::aligned_alloc(alignment, size);
#endif // __FST_WINDOWS__

        FST_IF_PROFILE(impl::add_profiler_event(profiler_memory_zone::id(), mid, ptr, size, impl::item_type::alloc));

        return ptr;
    }

    void profiler_memory_zone::aligned_deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        FST_IF_PROFILE(impl::add_profiler_event(profiler_memory_zone::id(), mid, ptr, 0, impl::item_type::dealloc));

#if __FST_WINDOWS__
        ::_aligned_free(ptr);
#else
        ::free(ptr);
#endif // __FST_WINDOWS__
    }

FST_END_NAMESPACE

#endif // FST_USE_PROFILER
