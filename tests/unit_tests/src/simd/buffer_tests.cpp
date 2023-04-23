//#include "utest.h"
//#include "fst/math.h"
//#include "fst/simd.h"
//
//namespace
//{
//    TEST_CASE("fst::simd::buffers", "[simd]")
//    {
//        /*{
//            fst::simd::buffer<float> a(64);
//            fst::simd::fixed_buffer<float, 64> b;
//            REQUIRE(b.size() == 64);
//
//            b.fill(32);
//            fst::test::require_compare_buffer(b, 32.0f);
//
//            fst::simd::fixed_buffer<float, 64> c = std::move(b);
//            b = fst::simd::fixed_buffer<float, 64>(a.fill(21));
//            fst::test::require_compare_buffer(b, 21.0f);
//        }*/
//
//        using value_type = float;
//        using buffer_type = fst::simd::buffer<value_type>;
//        using array_type = fst::simd::array<value_type, 64>;
//        using range_type = fst::simd::fixed_range<value_type, 64>;
//
//        {
//            fst::simd::array<value_type, 64, 64> a;
//            fst::simd::array<value_type, 128, 64> b;
//            fst::simd::buffer<value_type, 64> c(64);
//            b.fill(4);
//            c.fill(7);
//
//            size_t sum = 0;
//            for (size_t i = 0; i < a.size(); i++)
//            {
//                c[i] = (value_type) i;
//                sum += i;
//            }
//
//            REQUIRE_EQ(c.horizontal_sum(), sum);
//            c.fill(7);
//            a.fill(2);
//
//            b.seq_add(a);
//            fst::test::require_compare_buffer(b, 6.0f);
//
//            b.seq_add(c);
//            fst::test::require_compare_buffer(b, 13.0f);
//
//            c.fill(2);
//            b.seq_mul(c);
//            fst::test::require_compare_buffer(b, 26.0f);
//
//            b.seq_rsub(c);
//            fst::test::require_compare_buffer(b, -24.0f);
//
//            b.seq_assign(c);
//            fst::test::require_compare_buffer(b, 2.0f);
//        }
//
//        {
//            buffer_type a(64);
//            const buffer_type& c = a;
//            buffer_type b(c);
//            array_type d(c);
//        }
//
//        buffer_type buffer1(64);
//        buffer_type buffer2(64);
//
//        TEST_BLOCK("conv")
//        {
//            {
//                fst::simd::buffer<value_type, fst::simd::vector_type_alignment<value_type>::value, fst::simd_memory_zone, fst::default_memory_category> buf(64);
//                buffer_type buffer3(buf);
//
//                range_type buffer4 = buf;
//            }
//
//            {
//                fst::simd::array<value_type, 64> arr;
//                arr.fill(25.0f);
//
//                buffer_type buffer3 = arr;
//                REQUIRE(buffer3.size() == arr.size());
//                fst::test::require_compare_buffer(buffer3, 25.0f);
//
//                arr.fill(24.0f);
//                buffer3 = arr;
//                REQUIRE(buffer3.size() == arr.size());
//                fst::test::require_compare_buffer(buffer3, 24.0f);
//            }
//
//            {
//                fst::simd::array<value_type, 128> arr;
//                arr.fill(25.0f);
//
//                buffer_type buffer3 = arr;
//                REQUIRE(buffer3.size() == arr.size());
//                fst::test::require_compare_buffer(buffer3, 25.0f);
//
//                arr.fill(24.0f);
//                buffer3 = arr;
//                REQUIRE(buffer3.size() == arr.size());
//                fst::test::require_compare_buffer(buffer3, 24.0f);
//            }
//
//            {
//                fst::simd::array<value_type, 64> arr;
//                fst::simd::detail::range_base<value_type, 64> r = arr;
//                r.fill(25.0f);
//
//                buffer_type buffer3 = r;
//                REQUIRE(buffer3.size() == r.size());
//                fst::test::require_compare_buffer(buffer3, 25.0f);
//
//                r.fill(24.0f);
//                buffer3 = r;
//                REQUIRE(buffer3.size() == r.size());
//                fst::test::require_compare_buffer(buffer3, 24.0f);
//
//                buffer3.fill(12.0f);
//                arr = buffer3;
//                fst::test::require_compare_buffer(arr, 12.0f);
//
//                r = buffer3;
//                r.fill(14.0f);
//                fst::test::require_compare_buffer(arr, 12.0f);
//                fst::test::require_compare_buffer(buffer3, 14.0f);
//
//                fst::simd::array<value_type, 64> arr2 = buffer3 + arr;
//                fst::simd::buffer<value_type> arr4 = buffer3 + arr;
//            }
//        }
//
//        TEST_BLOCK("fill")
//        {
//            buffer1.fill(32.0f);
//            fst::test::require_compare_buffer(buffer1, 32.0f);
//
//            buffer1.fill_increment(1);
//            for (size_t i = 0; i < buffer1.size(); i++)
//            {
//                REQUIRE(buffer1[i] == i);
//            }
//
//            buffer1.fill_increment(2);
//            for (size_t i = 0; i < buffer1.size(); i++)
//            {
//                REQUIRE(buffer1[i] == i * 2);
//            }
//
//            buffer1.fill_increment(2, 1);
//            for (size_t i = 0; i < buffer1.size(); i++)
//            {
//                REQUIRE(buffer1[i] == (i + 1) * 2);
//            }
//        }
//
//        TEST_BLOCK("clear")
//        {
//            buffer1.clear();
//            fst::test::require_compare_buffer(buffer1, 0.0f);
//        }
//
//        TEST_BLOCK("scalar += add")
//        {
//            buffer1.fill(1);
//            buffer1 += 3.0f;
//            fst::test::require_compare_buffer(buffer1, 4.0f);
//
//            buffer1.add(3.0f);
//            fst::test::require_compare_buffer(buffer1, 7.0f);
//        }
//
//        TEST_BLOCK("scalar -= sub")
//        {
//            buffer1.fill(5);
//            buffer1 -= 3.0f;
//            fst::test::require_compare_buffer(buffer1, 2.0f);
//
//            buffer1.sub(1.0f);
//            fst::test::require_compare_buffer(buffer1, 1.0f);
//        }
//
//        TEST_BLOCK("scalar *= mul")
//        {
//            buffer1.fill(2);
//            buffer1 *= 4.0f;
//            fst::test::require_compare_buffer(buffer1, 8.0f);
//
//            buffer1.mul(2.0f);
//            fst::test::require_compare_buffer(buffer1, 16.0f);
//        }
//
//        TEST_BLOCK("scalar /= div")
//        {
//            buffer1.fill(4);
//            buffer1 /= 2.0f;
//            fst::test::require_compare_buffer(buffer1, 2.0f);
//
//            buffer1.div(2.0f);
//            fst::test::require_compare_buffer(buffer1, 1.0f);
//        }
//
//        TEST_BLOCK("+= add")
//        {
//            buffer1.fill(1);
//            buffer2.fill(2);
//            buffer1 += buffer2;
//            fst::test::require_compare_buffer(buffer1, 3.0f);
//
//            buffer1.add(buffer2);
//            fst::test::require_compare_buffer(buffer1, 5.0f);
//        }
//
//        TEST_BLOCK("-= sub")
//        {
//            buffer1.fill(3);
//            buffer2.fill(2);
//            buffer1 -= buffer2;
//            fst::test::require_compare_buffer(buffer1, 1.0f);
//
//            buffer1.sub(buffer2);
//            fst::test::require_compare_buffer(buffer1, -1.0f);
//        }
//
//        TEST_BLOCK("*= mul")
//        {
//            buffer1.fill(2);
//            buffer2.fill(4);
//            buffer1 *= buffer2;
//            fst::test::require_compare_buffer(buffer1, 8.0f);
//
//            buffer1.mul(buffer2);
//            fst::test::require_compare_buffer(buffer1, 32.0f);
//        }
//
//        TEST_BLOCK("/= div")
//        {
//            buffer1.fill(4);
//            buffer2.fill(2);
//            buffer1 /= buffer2;
//            fst::test::require_compare_buffer(buffer1, 2.0f);
//
//            buffer1.div(buffer2);
//            fst::test::require_compare_buffer(buffer1, 1.0f);
//        }
//
//        TEST_BLOCK("scalar +")
//        {
//            buffer1.fill(4);
//            buffer_type a = buffer1 + 3.0f;
//            fst::test::require_compare_buffer(a, 7.0f);
//            fst::test::require_compare_buffer(buffer1, 4.0f);
//        }
//
//        TEST_BLOCK("scalar -")
//        {
//            buffer1.fill(4);
//            buffer_type a = buffer1 - 3.0f;
//            fst::test::require_compare_buffer(a, 1.0f);
//            fst::test::require_compare_buffer(buffer1, 4.0f);
//        }
//
//        TEST_BLOCK("scalar *")
//        {
//            buffer1.fill(4);
//            buffer_type a = buffer1 * 3.0f;
//            fst::test::require_compare_buffer(a, 12.0f);
//            fst::test::require_compare_buffer(buffer1, 4.0f);
//        }
//
//        TEST_BLOCK("scalar /")
//        {
//            buffer1.fill(6);
//            buffer_type a = buffer1 / 3.0f;
//            fst::test::require_compare_buffer(a, 2.0f);
//            fst::test::require_compare_buffer(buffer1, 6.0f);
//        }
//
//        TEST_BLOCK("+")
//        {
//            buffer1.fill(4);
//            buffer2.fill(2);
//            buffer_type a = buffer1 + buffer2;
//            fst::test::require_compare_buffer(a, 6.0f);
//            fst::test::require_compare_buffer(buffer1, 4.0f);
//        }
//
//        TEST_BLOCK("-")
//        {
//            buffer1.fill(4);
//            buffer2.fill(2);
//            buffer_type a = buffer1 - buffer2;
//            fst::test::require_compare_buffer(a, 2.0f);
//            fst::test::require_compare_buffer(buffer1, 4.0f);
//        }
//
//        TEST_BLOCK("*")
//        {
//            buffer1.fill(4);
//            buffer2.fill(2);
//            buffer_type a = buffer1 * buffer2;
//            fst::test::require_compare_buffer(a, 8.0f);
//            fst::test::require_compare_buffer(buffer1, 4.0f);
//        }
//
//        TEST_BLOCK("/")
//        {
//            buffer1.fill(6);
//            buffer2.fill(3);
//            buffer_type a = buffer1 / buffer2;
//            fst::test::require_compare_buffer(a, 2.0f);
//            fst::test::require_compare_buffer(buffer1, 6.0f);
//        }
//    }
//} // namespace
