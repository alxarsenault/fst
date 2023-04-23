#include "utest.h"

//#include <string>
#include "fst/traits.h"
#include "fst/memory_range.h"

//#include <string>

TEST_CASE("fst::memory_range<>")
{
    {

        fst::array<float, 5> buffer;
        fst::memory_range<float, 5> range = buffer;
        fst::memory_range<float> r2 = range;
        fst::memory_range<float, 5> r3 = r2;
    }
}

template <class T, class U = T>
void check_subscript_operator(fst::array<T, 5> buffer)
{
    fst::memory_range<U> range(buffer);
    const fst::memory_range<U>& rrange = range;
    REQUIRE(range.size() == buffer.size());
    REQUIRE(rrange.size() == buffer.size());

    for (size_t i = 0; i < range.size(); i++)
    {
        REQUIRE(range[i] == buffer[i]);
        REQUIRE(rrange[i] == buffer[i]);
    }
}

template <class T, class U = T>
void check_accessor(fst::array<T, 5> buffer)
{

    fst::memory_range<U> range(buffer);
    const fst::memory_range<U>& rrange = range;

    TEST_BLOCK("size, size_bytes, empty")
    {
        {
            fst::memory_range<U> empty_range;
            REQUIRE(empty_range.size() == 0);
            REQUIRE(empty_range.size_bytes() == 0);
            REQUIRE(empty_range.empty());
        }

        REQUIRE(range.size() == 5);
        REQUIRE(range.size_bytes() == 5 * sizeof(T));
        REQUIRE(!range.empty());

        REQUIRE(rrange.size() == 5);
        REQUIRE(rrange.size_bytes() == 5 * sizeof(T));
        REQUIRE(!rrange.empty());
    }

    TEST_BLOCK("front")
    {
        REQUIRE(range.front() == buffer.front());
        REQUIRE(rrange.front() == buffer.front());
    }

    TEST_BLOCK("back")
    {
        REQUIRE(range.back() == buffer.back());
        REQUIRE(rrange.back() == buffer.back());
    }

    TEST_BLOCK("data")
    {
        {
            U* data = range.data();
            const U* rdata = rrange.data();

            for (size_t i = 0; i < range.size(); i++)
            {
                REQUIRE(data[i] == buffer[i]);
                REQUIRE(rdata[i] == buffer[i]);
            }
        }

        {
            U* data = range.data(1);
            const U* rdata = rrange.data(1);

            for (size_t i = 1; i < range.size(); i++)
            {
                REQUIRE(data[i - 1] == buffer[i]);
                REQUIRE(rdata[i - 1] == buffer[i]);
            }
        }
    }

    TEST_BLOCK("subrange")
    {
        {
            fst::memory_range<U> srange = range.subrange(0);
            fst::memory_range<const U> srrange = rrange.subrange(0);
            REQUIRE(srange.size() == 5);
            REQUIRE(srrange.size() == 5);
        }

        {
            fst::memory_range<U> srange = range.subrange(1);
            fst::memory_range<const U> srrange = rrange.subrange(1);
            REQUIRE(srange.size() == 4);
            REQUIRE(srrange.size() == 4);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 1]);
                REQUIRE(srrange[i] == buffer[i + 1]);
            }
        }

        {
            fst::memory_range<U> srange = range.subrange(3);
            fst::memory_range<const U> srrange = rrange.subrange(3);
            REQUIRE(srange.size() == 2);
            REQUIRE(srrange.size() == 2);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 3]);
                REQUIRE(srrange[i] == buffer[i + 3]);
            }
        }
        {
            fst::memory_range<U> srange = range.subrange(4);
            fst::memory_range<const U> srrange = rrange.subrange(4);
            REQUIRE(srange.size() == 1);
            REQUIRE(srrange.size() == 1);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 4]);
                REQUIRE(srrange[i] == buffer[i + 4]);
            }
        }

        ///

        {
            fst::memory_range<U> srange = range.subrange(0, 0);
            fst::memory_range<const U> srrange = rrange.subrange(0, 0);
            REQUIRE(srange.size() == 0);
            REQUIRE(srrange.size() == 0);
        }

        {
            fst::memory_range<U> srange = range.subrange(0, 50);
            fst::memory_range<const U> srrange = rrange.subrange(0, 50);
            REQUIRE(srange.size() == 5);
            REQUIRE(srrange.size() == 5);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i]);
                REQUIRE(srrange[i] == buffer[i]);
            }
        }

        {
            fst::memory_range<U> srange = range.subrange(0, 2);
            fst::memory_range<const U> srrange = rrange.subrange(0, 2);
            REQUIRE(srange.size() == 2);
            REQUIRE(srrange.size() == 2);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i]);
                REQUIRE(srrange[i] == buffer[i]);
            }
        }

        {
            fst::memory_range<U> srange = range.subrange(1, 3);
            fst::memory_range<const U> srrange = rrange.subrange(1, 3);
            REQUIRE(srange.size() == 3);
            REQUIRE(srrange.size() == 3);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 1]);
                REQUIRE(srrange[i] == buffer[i + 1]);
            }
        }

        {
            fst::memory_range<U> srange = range.subrange(4, 1);
            fst::memory_range<const U> srrange = rrange.subrange(4, 1);
            REQUIRE(srange.size() == 1);
            REQUIRE(srrange.size() == 1);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 4]);
                REQUIRE(srrange[i] == buffer[i + 4]);
            }
        }

        {
            fst::memory_range<U> srange = range.subrange(4, 100);
            fst::memory_range<const U> srrange = rrange.subrange(4, 100);
            REQUIRE(srange.size() == 1);
            REQUIRE(srrange.size() == 1);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 4]);
                REQUIRE(srrange[i] == buffer[i + 4]);
            }
        }
    }

    TEST_BLOCK("first range")
    {
        {
            fst::memory_range<U> srange = range.first(0);
            fst::memory_range<const U> srrange = rrange.first(0);
            REQUIRE(srange.size() == 0);
            REQUIRE(srrange.size() == 0);
        }

        {
            fst::memory_range<U> srange = range.first(1);
            fst::memory_range<const U> srrange = rrange.first(1);
            REQUIRE(srange.size() == 1);
            REQUIRE(srrange.size() == 1);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i]);
                REQUIRE(srrange[i] == buffer[i]);
            }
        }

        {
            fst::memory_range<U> srange = range.first(5);
            fst::memory_range<const U> srrange = rrange.first(5);
            REQUIRE(srange.size() == 5);
            REQUIRE(srrange.size() == 5);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i]);
                REQUIRE(srrange[i] == buffer[i]);
            }
        }

        {
            fst::memory_range<U> srange = range.first(3);
            fst::memory_range<const U> srrange = rrange.first(3);
            REQUIRE(srange.size() == 3);
            REQUIRE(srrange.size() == 3);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i]);
                REQUIRE(srrange[i] == buffer[i]);
            }
        }
    }

    TEST_BLOCK("last range")
    {
        {
            fst::memory_range<U> srange = range.last(0);
            fst::memory_range<const U> srrange = rrange.last(0);
            REQUIRE(srange.size() == 0);
            REQUIRE(srrange.size() == 0);
        }

        {
            fst::memory_range<U> srange = range.last(5);
            fst::memory_range<const U> srrange = rrange.last(5);
            REQUIRE(srange.size() == 5);
            REQUIRE(srrange.size() == 5);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i]);
                REQUIRE(srrange[i] == buffer[i]);
            }
        }

        {
            fst::memory_range<U> srange = range.last(3);
            fst::memory_range<const U> srrange = rrange.last(3);
            REQUIRE(srange.size() == 3);
            REQUIRE(srrange.size() == 3);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 2]);
                REQUIRE(srrange[i] == buffer[i + 2]);
            }
        }

        {
            fst::memory_range<U> srange = range.last(1);
            fst::memory_range<const U> srrange = rrange.last(1);
            REQUIRE(srange.size() == 1);
            REQUIRE(srrange.size() == 1);

            for (size_t i = 0; i < srange.size(); i++)
            {
                REQUIRE(srange[i] == buffer[i + 4]);
                REQUIRE(srrange[i] == buffer[i + 4]);
            }
        }
    }
}
//} // namespace

//}
//
TEST_CASE("fst::memory_range")
{
    //check_accessor<float>(fst::array<float, 5>{ 0, 1, 2, 3, 4 });
    //check_accessor<float, const float>({ 0, 1, 2, 3, 4 });

    //check_accessor<fst::string>({ "a", "b", "c", "d", "e" });
    //check_accessor<fst::string, const fst::string>({ "a", "b", "c", "d", "e" });

    TEST_BLOCK("constructor")
    {
        // memory_range()
        {
            fst::memory_range<float> range;
            REQUIRE(range.size() == 0);
        }

        // memory_range(pointer ptr, size_type count)
        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<float> range(&buffer[0], 5);
            REQUIRE(range.size() == 5);
        }

        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<const float> range(&buffer[0], 5);
            REQUIRE(range.size() == 5);
        }

        {
            const float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<const float> range(&buffer[0], 5);
            REQUIRE(range.size() == 5);
        }

        // memory_range(pointer first, pointer last)
        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<float> range(&buffer[0], &buffer[5]);
            REQUIRE(range.size() == 5);
        }

        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<const float> range(&buffer[0], &buffer[5]);
            REQUIRE(range.size() == 5);
        }

        {
            const float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<const float> range(&buffer[0], &buffer[5]);
            REQUIRE(range.size() == 5);
        }

        // memory_range(element_type(&__arr)[Size])
        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<float> range(buffer);
            REQUIRE(range.size() == 5);
        }

        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<const float> range(buffer);
            REQUIRE(range.size() == 5);
        }

        {
            const float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<const float> range(buffer);
            REQUIRE(range.size() == 5);
        }

        // memory_range(fst::array<U, Size>& arr)
        // memory_range(const fst::array<U, Size>& arr)
        {
            //fst::array<float, 5> buffer = { 0, 1, 2, 3, 4 };
            //fst::memory_range<float> range(buffer);
            //REQUIRE(range.size() == 5);
        }

        {
            //fst::array<float, 5> buffer = { 0, 1, 2, 3, 4 };
            //fst::memory_range<const float> range(buffer);
            //REQUIRE(range.size() == 5);
        }

        {
            //fst::array<const float, 5> buffer = { 0, 1, 2, 3, 4 };
            //fst::memory_range<const float> range(buffer);
            //REQUIRE(range.size() == 5);
        }

        // memory_range(Container& c, enable_if_compatible_t<Container> = nullptr)
        // memory_range(const Container& c, enable_if_compatible_t<const Container> = nullptr)
        {
            /*fst::vector<float> buffer = { 0, 1, 2, 3, 4 };
                fst::memory_range<float> range(buffer);
                REQUIRE(range.size() == 5);*/
        }

        {
            //fst::vector<float> buffer = { 0, 1, 2, 3, 4 };
            //fst::memory_range<const float> range(buffer);
            //REQUIRE(range.size() == 5);
        }

        // memory_range(memory_range<U> r, enable_if_convertible_t<U> = nullptr)
        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<float> range(buffer);
            REQUIRE(range.size() == 5);

            fst::memory_range<const float> crange = range;
            REQUIRE(crange.size() == 5);
        }

        // memory_range(const memory_range&) noexcept
        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<float> range(buffer);
            REQUIRE(range.size() == 5);

            fst::memory_range<float> range2 = range;
            REQUIRE(range2.size() == 5);
        }

        {
            float buffer[5] = { 0, 1, 2, 3, 4 };
            fst::memory_range<const float> range(buffer);
            REQUIRE(range.size() == 5);

            fst::memory_range<const float> range2 = range;
            REQUIRE(range2.size() == 5);
        }
    }
}

//template<class T>
//using is_vector = fst::is_template_of <fst::vector, T>;

//TEST_CASE("fst::ff")
//{
//    float buffer[5] = { 0, 1, 2, 3, 4 };

//    REQUIRE(fst::is_memory_range<fst::memory_range<int>>::value);
//    REQUIRE(!fst::is_memory_range<fst::vector<int>>::value);

//    REQUIRE(is_vector<fst::vector<int>>::value);
//    REQUIRE(!is_vector<int>::value);

//    REQUIRE(fst::is_template_of<fst::vector, fst::vector<int>>::value);
//    REQUIRE(!fst::is_template_of<fst::vector, int>::value);

//    //REQUIRE(fst::bb<fst::vector>::is_template_of<fst::vector<int>>::value);
//    //REQUIRE(!fst::bb<fst::vector>::is_template_of<int>::value);
//    //REQUIRE(fst::is_template_of<fst::vector<int>>::value);
//    fst::memory_range<float> range(buffer);
//    fst::memory_range<float> ccc = range.cast_range<float>();
//    fst::memory_range<const float> ccec = range.cast_range<const float>();
//    fst::memory_range< const float> cceec = ccec.cast_range<  float>();

//    REQUIRE(range.as<fst::little_endian_tag, float>(1) == range[1]);
//    REQUIRE(range.as<fst::big_endian_tag, float>(1) == fst::byte_swap(range[1]));

//    fst::byte_range br = range.byte_range();
//    float dddd = br.as<float>(4 * sizeof(float));

//    float& ddddd = br.as<float>(br.begin() + 4 * sizeof(float));

//    REQUIRE(dddd == 4);
//    REQUIRE(ddddd == 4);
//    ddddd = 89;
//    REQUIRE(ddddd == 89);
//    REQUIRE(range[4] == 89);

//    float* bbb = range.data(1);
//    REQUIRE(*bbb == 1);

//    float aa = range.as<float>(2);
//    REQUIRE(aa == 2);

//    fst::memory_range<const float> crange = range;
//    const int32_t* data = crange.data<  int32_t>();

//    //fst::memory_range<  float> ecrange = crange;
//    fst::memory_view<float> vv = range.view();

//    fst::memory_range<float> sr = range.subrange(2);
//    REQUIRE(sr.size() == 3);

//    fst::memory_view<float> mv = range;
//    crange = mv;

//    REQUIRE(range.size() == 5);
//    REQUIRE(range.size_bytes() == 5 * sizeof(float));
//}
//} // namespace
