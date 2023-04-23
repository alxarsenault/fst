#include "utest.h"
#include "fst/array.h"

namespace
{
    TEST_CASE("fst::abasic_heap_array", "[array]")
    {
        {
            fst::fixed_heap_array<float, 32> a;
            REQUIRE(a.size() == 32);

            fst::fixed_heap_array<float, 32> b = std::move(a);
            REQUIRE(b.size() == 32);
            REQUIRE(a.size() == 32);
            REQUIRE(a.empty());
            REQUIRE(a.data() == nullptr);
        }
    }

    struct my_memory_zone : fst::memory_zone<my_memory_zone>
    {
        using base = fst::memory_zone<my_memory_zone>;
        //using base::aligned_allocate;
        //using base::aligned_deallocate;
        //using base::allocate;
        //using base::deallocate;

        my_memory_zone() = delete;
        my_memory_zone(const my_memory_zone&) = default;
        my_memory_zone(my_memory_zone&&) = default;

        my_memory_zone(char* __data, size_t& __size, size_t __capacity)
            : _data(__data)
            , _size(__size)
            , _capacity(__capacity)
        {}

        my_memory_zone& operator=(const my_memory_zone&) = delete;
        my_memory_zone& operator=(my_memory_zone&&) = delete;

        inline void* allocate(size_t size, fst::memory_category_id) noexcept
        {
            if (size + _size > _capacity)
            {
                fst_error("Wrong alloc");
                return nullptr;
            }

            size_t index = _size;
            _size += size;
            return _data + index;
        }

        inline void deallocate(void*, fst::memory_category_id) noexcept {}

        inline void* aligned_allocate(size_t size, size_t alignment, fst::memory_category_id mid) noexcept
        {
            fst::unused(alignment, mid);
            if (size + _size > _capacity)
            {
                fst_error("Wrong alloc");
                return nullptr;
            }

            size_t index = _size;
            _size += size;
            return _data + index;
        }

        inline void aligned_deallocate(void*, fst::memory_category_id) noexcept {}

        char* _data;
        size_t& _size;
        size_t _capacity;
    };

    TEST_CASE("fst::basic_heap_array", "[array]")
    {
        {
            alignas(16) char _data[128];
            size_t _size = 0;
            my_memory_zone zone(_data, _size, 128);

            fst::heap_array<float, alignof(float), my_memory_zone> a(zone);
            REQUIRE(a.size() == 0);
            REQUIRE(a.data() == nullptr);

            a.reset(32);
            REQUIRE(a.size() == 32);

            for (size_t i = 0; i < a.size(); i++)
            {
                a[i] = (float) i;
            }

            fst::heap_array<float> b = a;
            REQUIRE(b.size() == 32);

            fst::test::check_compare_buffers(a, b, 0.0f);
        }
    }
} // namespace
