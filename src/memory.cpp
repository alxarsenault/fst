#include "fst/memory.h"
#include "fst/profiler.h"
#include "fst/simd.h"
#include "fst/memory_pool.h"

#include <string.h>

//#include "smmalloc.h"

// https://developers.redhat.com/articles/2022/12/06/implementing-c20-atomic-waiting-libstdc#putting_together_the_pieces_for_a_notify_primitive
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
        //
        // memory_category
        //
        memory_category_id category_base::restricted_base::get_next_memory_category_id() noexcept
        {
            FST_GLOBAL_SECTION static __fst::memory_category_id __id = (__fst::memory_category_id) 0;
            return __id++;
        }

#if FST_USE_PROFILER
        bool category_base::restricted_base::register_memory_category(__fst::memory_category_id mid, const char* name) noexcept
        {
            __fst::profiler::register_name(mid, name);
            return true;
        }
#endif
        //
        // memory_zone
        //
        memory_zone_id zone_base::restricted_base::get_next_memory_zone_id() noexcept
        {
            FST_GLOBAL_SECTION static __fst::memory_zone_id _id = (__fst::memory_zone_id) 0;
            return _id++;
        }

#if FST_USE_PROFILER
        bool zone_base::restricted_base::register_memory_zone(__fst::memory_zone_id zid, const char* name) noexcept
        {
            __fst::profiler::register_name(zid, name);
            return true;
        }
#endif

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

    namespace
    {
        inline void* __fst_aligned_alloc(size_t size, size_t alignment) noexcept
        {
#if __FST_WINDOWS__
            void* ptr = ::_aligned_malloc(size, alignment);
#elif __FST_MACOS__
            void* ptr = ::aligned_alloc(alignment, size);
#else
            void* ptr = ::aligned_alloc(alignment, size);
#endif
            return ptr;
        }

        inline void __fst_aligned_dealloc(void* ptr) noexcept
        {
#if __FST_WINDOWS__
            ::_aligned_free(ptr);
#else
            ::free(ptr);
#endif
        }
    } // namespace

    //
    // default_memory_zone
    //

    void* default_memory_zone::allocate(size_t size, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        void* ptr = ::malloc(size);
        FST_IF_PROFILE(__fst::profiler::allocated(ptr, size, id(), mid));
        return ptr;
    }

    void default_memory_zone::deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        FST_IF_PROFILE(__fst::profiler::deallocated(ptr, id(), mid));
        ::free(ptr);
    }

    void* default_memory_zone::aligned_allocate(size_t size, size_t alignment, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        void* ptr = __fst_aligned_alloc(size, alignment);
        FST_IF_PROFILE(__fst::profiler::allocated(ptr, size, id(), mid));
        return ptr;
    }

    void default_memory_zone::aligned_deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) noexcept
    {
        FST_IF_PROFILE(__fst::profiler::deallocated(ptr, id(), mid));
        __fst_aligned_dealloc(ptr);
    }

    //
    // simd_memory_zone
    //

    size_t simd_memory_zone::default_alignment() noexcept
    {
        return __fst::default_alignment;
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
        void* ptr = __fst_aligned_alloc(size, alignment);
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
        __fst_aligned_dealloc(ptr);
#endif
    }

    struct pool_memory_zone::pool
    {
        inline pool(size_t buckets, size_t bucket_size) noexcept
            : _allocator(buckets, bucket_size)
            , _ref_count(1)
        {}

        pool(const pool&) = delete;
        pool(pool&&) = delete;

        ~pool() noexcept = default;

        pool& operator=(const pool&) = delete;
        pool& operator=(pool&&) = delete;

        __fst::small_memory_pool<__fst::default_memory_category, __fst::default_memory_zone> _allocator;
        size_t _ref_count;
    };

    pool_memory_zone::pool_memory_zone(size_t buckets, size_t bucket_size) noexcept
    {
        void* buffer = __fst::aligned_allocate(sizeof(pool), alignof(pool));
        _pool = fst_placement_new(buffer) pool(buckets, bucket_size);
    }

    pool_memory_zone::pool_memory_zone(const pool_memory_zone& other) noexcept
        : _pool(other._pool)
    {
        if (_pool) { _pool->_ref_count++; }
    }

    pool_memory_zone::pool_memory_zone(pool_memory_zone && other) noexcept
        : _pool(other._pool)
    {
        other._pool = nullptr;
    }

    pool_memory_zone::~pool_memory_zone() noexcept
    {
        if (_pool)
        {
            if (--_pool->_ref_count == 0)
            {
                _pool->~pool();
                __fst::aligned_deallocate(_pool);
            }
            _pool = nullptr;
        }
    }

    pool_memory_zone& pool_memory_zone::operator=(const pool_memory_zone& other) noexcept
    {
        if (_pool == other._pool) { return *this; }

        if (_pool)
        {
            if (--_pool->_ref_count == 0)
            {
                _pool->~pool();
                __fst::aligned_deallocate(_pool);
            }

            _pool = nullptr;
        }

        _pool = other._pool;
        if (_pool) { _pool->_ref_count++; }

        return *this;
    }

    pool_memory_zone& pool_memory_zone::operator=(pool_memory_zone&& other) noexcept
    {
        if (_pool == other._pool)
        {
            other._pool = nullptr;

            if (_pool && --_pool->_ref_count == 0) { fst_error("jkkjkjkljkl"); }
            return *this;
        }

        if (_pool)
        {
            if (--_pool->_ref_count == 0)
            {
                _pool->~pool();
                __fst::aligned_deallocate(_pool);
            }

            _pool = nullptr;
        }

        _pool = __fst::exchange(other._pool, nullptr);
        return *this;
    }

    void* pool_memory_zone::allocate(size_t size, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) const noexcept
    {
        fst_assert(_pool);
        void* ptr = _pool->_allocator.aligned_allocate(size, 1);
        FST_IF_PROFILE(__fst::profiler::allocated(ptr, size, id(), mid));
        return ptr;
    }

    void pool_memory_zone::deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) const noexcept
    {
        FST_IF_PROFILE(__fst::profiler::deallocated(ptr, id(), mid));
        _pool->_allocator.aligned_deallocate(ptr);
    }

    void* pool_memory_zone::aligned_allocate(size_t size, size_t alignment, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) const noexcept
    {
        //fst::debug()(_pool->_allocator.GetBucketElementSize(4));
        void* ptr = _pool->_allocator.aligned_allocate(size, alignment);
        //FST_IF_PROFILE(__fst::profiler::allocated(ptr, size, id(), mid));
        return ptr;
    }

    void pool_memory_zone::aligned_deallocate(void* ptr, FST_ATTRIBUTE_UNUSED __fst::memory_category_id mid) const noexcept
    {
        //FST_IF_PROFILE(__fst::profiler::deallocated(ptr, id(), mid));
        _pool->_allocator.aligned_deallocate(ptr);
    }

FST_END_NAMESPACE
