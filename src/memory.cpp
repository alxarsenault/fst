#include "fst/memory.h"
#include "fst/profiler.h"
#include "fst/simd.h"
#include <string.h>

#if FST_USE_RP_MALLOC
#define FST_RP_MALLOC_NAMESPACE //rp_malloc_zone
#define ENABLE_PRELOAD 1
#define ENABLE_ASSERTS 1
#define ENABLE_VALIDATE_ARGS 1
#include "rpmalloc.h"
#endif // FST_USE_RP_MALLOC

#if __FST_MSVC__
#pragma section("fst_global", read, write)
#define FST_GLOBAL_SECTION __declspec(allocate("fst_global"))
#else
#define FST_GLOBAL_SECTION
#endif //

FST_BEGIN_NAMESPACE

    namespace memory
    {

        memory_category_id get_next_memory_category_id() noexcept
        {

            FST_GLOBAL_SECTION static __fst::memory_category_id __id = (__fst::memory_category_id) 0;
            return __id++;
        }

        memory_zone_id get_next_memory_zone_id() noexcept
        {
            FST_GLOBAL_SECTION static __fst::memory_zone_id _id = (__fst::memory_zone_id) 0;
            return _id++;
        }

        void zone_base::restricted_base::move_allocation(FST_ATTRIBUTE_UNUSED void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_zone_id zid,
            FST_ATTRIBUTE_UNUSED __fst::memory_category_id from_mid, FST_ATTRIBUTE_UNUSED __fst::memory_category_id to_mid) noexcept
        {
            FST_IF_PROFILE(profiler::move_allocation(ptr, zid, from_mid, to_mid));
        }

    } // namespace memory.

    FST_NODISCARD size_t mem_cache_size() noexcept
    {
        return 64;
    }

    FST_NODISCARD size_t mem_page_size() noexcept
    {
        return 4096;
    }

// Category.
#if FST_USE_PROFILER
    bool register_memory_category(FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid, FST_ATTRIBUTE_UNUSED const char* name) noexcept
    {
        //#if FST_USE_PROFILER
        FST_IF_PROFILE(__fst::profiler::register_name(mid, name));
        //#else
        //__fst::unused(mid, name);
        //#endif // FST_PROFILE
        return true;
    }

    void register_memory_zone(FST_ATTRIBUTE_UNUSED __fst::memory_zone_id zid, FST_ATTRIBUTE_UNUSED const char* name) noexcept
    {

        //#if FST_USE_PROFILER
        FST_IF_PROFILE(__fst::profiler::register_name(zid, name));
        //#else
        //__fst::unused(mid, name);
        //#endif // FST_PROFILE
    }
#endif
    //#if FST_USE_PROFILER
    //    FST_PRAGMA_PUSH()
    //    FST_PRAGMA_DISABLE_WARNING_CLANG("-Wexit-time-destructors")
    //    FST_GLOBAL_SECTION static memory_category_register<__fst::default_memory_category> __default_memory_category_registration = {};
    //    FST_GLOBAL_SECTION static memory_category_register<__fst::simd_memory_category> __simd_memory_category_registration = {};
    //    FST_GLOBAL_SECTION static memory_category_register<__fst::dsp_memory_category> __dsp_memory_category_registration = {};
    //    FST_PRAGMA_POP()
    //#endif

    void* default_memory_zone::allocate(size_t size, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        void* ptr = ::malloc(size);

        //#if FST_USE_PROFILER
        FST_IF_PROFILE(__fst::profiler::allocated(ptr, size, id(), mid));
        //#else
        //__fst::unused(mid);
        //#endif // FST_PROFILE

        return ptr;
    }

    void default_memory_zone::deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        FST_IF_PROFILE(__fst::profiler::deallocated(ptr, id(), mid));

        ::free(ptr);
    }

    void* default_memory_zone::aligned_allocate(size_t size, size_t alignment, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
#if __FST_WINDOWS__
        void* ptr = ::_aligned_malloc(size, alignment);
#else
        void* ptr = ::aligned_alloc(alignment, size);
#endif // __FST_WINDOWS__

        FST_IF_PROFILE(__fst::profiler::allocated(ptr, size, id(), mid));

        return ptr;
    }

    void default_memory_zone::aligned_deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        FST_IF_PROFILE(__fst::profiler::deallocated(ptr, id(), mid));

#if __FST_WINDOWS__
        ::_aligned_free(ptr);
#else
        ::free(ptr);
#endif // __FST_WINDOWS__
    }

    //
    size_t simd_memory_zone::default_alignment() noexcept
    {
        return __fst::simd::vector_type_alignment_v<float>;
    }

    void* simd_memory_zone::allocate(size_t size, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        return simd_memory_zone::aligned_allocate(size, default_alignment(), mid);
    }

    void simd_memory_zone::deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        simd_memory_zone::aligned_deallocate(ptr, mid);
    }

    void* simd_memory_zone::aligned_allocate(size_t size, size_t alignment, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        alignment = __fst::maximum(default_alignment(), alignment);

#if FST_USE_RP_MALLOC
        void* ptr = FST_RP_MALLOC_NAMESPACE::rpaligned_alloc(alignment, size);
#else
#if __FST_WINDOWS__
        void* ptr = ::_aligned_malloc(size, alignment);
#else
        void* ptr = ::aligned_alloc(alignment, size);
#endif // __FST_WINDOWS__
#endif
        FST_IF_PROFILE(__fst::profiler::allocated(ptr, size, id(), mid));

        return ptr;
    }

    void simd_memory_zone::aligned_deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        FST_IF_PROFILE(__fst::profiler::deallocated(ptr, id(), mid));

#if FST_USE_RP_MALLOC
        FST_RP_MALLOC_NAMESPACE::rpfree(ptr);
#else
#if __FST_WINDOWS__
        ::_aligned_free(ptr);
#else
        ::free(ptr);
#endif // __FST_WINDOWS__
#endif
    }

FST_END_NAMESPACE
