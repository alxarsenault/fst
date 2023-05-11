//#pragma once
//
//#include "fst/memory.h"
//#include "fst/pointer.h"
//#include "fst/array.h"
//#include "fst/atomic.h"
//#include "fst/math.h"
//
//// The MIT License (MIT)
////
//// 	Copyright (c) 2017-2021 Sergey Makeev
////
//// 	Permission is hereby granted, free of charge, to any person obtaining a copy
//// 	of this software and associated documentation files (the "Software"), to deal
//// 	in the Software without restriction, including without limitation the rights
//// 	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//// 	copies of the Software, and to permit persons to whom the Software is
//// 	furnished to do so, subject to the following conditions:
////
////      The above copyright notice and this permission notice shall be included in
//// 	all copies or substantial portions of the Software.
////
//// 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//// 	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//// 	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//// 	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//// 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//// 	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//// 	THE SOFTWARE.
//
////#include <initializer_list>
////#include <memory>
////#include <stdint.h>
//
//#if defined(_M_X64) || _LP64
//#define SMMMALLOC_X64
//#define SMM_MAX_CACHE_ITEMS_COUNT (7)
//#else
//#define SMMMALLOC_X86
//#define SMM_MAX_CACHE_ITEMS_COUNT (10)
//#endif
//
//#define SMM_CACHE_LINE_SIZE (64)
//#define SMM_MAX_BUCKET_COUNT (8)
//
//namespace sm
//{
//    FST_INLINE_VAR constexpr size_t kMinValidAlignment = 16;
//
//    /*struct GenericAllocator
//    {
//        static void* Alloc(size_t bytesCount, size_t alignment)
//        {
//            if (alignment < sm::kMinValidAlignment) { alignment = sm::kMinValidAlignment; }
//
//            void* ptr = __fst::aligned_allocate(bytesCount, alignment);
//            return ptr;
//        }
//
//        static void Free(void* ptr)
//        {
//            if (!ptr) { return; }
//            __fst::aligned_deallocate(ptr);
//        }
//    };*/
//
//    ///
//    struct pool_bucket
//    {
//        inline pool_bucket() noexcept
//            : _head(tagged_index::invalid)
//            , _global_tag(0)
//            , _data(nullptr)
//            , _data_end(nullptr)
//        {}
//
//        inline void create(size_t elementSize, uint8_t* begin, uint8_t* end) noexcept
//        {
//            fst_assert(elementSize >= 16 && "Invalid element size");
//
//            _data = begin;
//            _data_end = end;
//
//            // build inplace single linked list
//            _global_tag.store(0);
//            uint8_t* node = _data;
//
//            tagged_index head_value;
//            head_value.p.tag = _global_tag.load();
//            head_value.p.offset = (uint32_t) (node - _data);
//            _head.store(head_value.u);
//
//            while (true)
//            {
//                uint8_t* next = node + elementSize;
//                if ((next + elementSize) <= _data_end)
//                {
//                    tagged_index next_value;
//                    next_value.p.tag = _global_tag.load();
//                    next_value.p.offset = (uint32_t) (next - _data);
//                    *((tagged_index*) (node)) = next_value;
//                }
//                else
//                {
//                    // last node
//                    ((tagged_index*) (node))->u = tagged_index::invalid;
//                    break;
//                }
//
//                node = next;
//                ++_global_tag;
//            }
//        }
//
//        FST_ALWAYS_INLINE void* allocate() noexcept
//        {
//            uint8_t* p = nullptr;
//            tagged_index head_value;
//            head_value.u = _head.load();
//
//            while (true)
//            {
//                // list is empty, exiting
//                if (head_value.u == tagged_index::invalid) { return nullptr; }
//
//                // get head pointer
//                p = (_data + head_value.p.offset);
//
//                // read head->next value
//                tagged_index next_value = *((tagged_index*) (p));
//
//                /*
//                    ABA problem here.
//
//                    pop = Alloc
//                    push = Free
//
//                    1. Thread 0 begins a pop and sees "A" as the top, followed by "B".
//                    2. Thread 1 begins and completes a pop, returning "A".
//                    3. Thread 1 begins and completes a push of "D".
//                    4. Thread 1 pushes "A" back onto the stack and completes.
//                    5. Thread 0 sees that "A" is on top and returns "A", setting the new top to "B".
//                    6. Node D is lost.
//
//                    Visual representation here
//                    http://15418.courses.cs.cmu.edu/spring2013/lecture/lockfree/slide_016
//
//                    Solive ABA problem using taggged indexes.
//                    Each time when thread push to the stack, unique 'tag' increased.
//                    And prevent invalid CAS operation.
//
//                */
//
//                // try to swap head to head->next
//                if (_head.compare_exchange(head_value.u, next_value.u))
//                {
//                    // success
//                    break;
//                }
//                // can't swap values, head is changed (now headValue has new head loaded) try again
//            }
//
//            return p;
//        }
//
//        FST_ALWAYS_INLINE void deallocate_interval(void* _pHead, void* _pTail) noexcept
//        {
//            uint8_t* pHead = (uint8_t*) _pHead;
//            uint8_t* pTail = (uint8_t*) _pTail;
//
//            // attach already linked list pHead->pTail to lock free list
//            // replace head and link tail with old list head
//
//            /*
//
//            Need to use atomic increment here.
//            Without atomic increment the operation++ works like this:
//            1. uint32_t temp = load(addr);
//            2. temp++;
//            3. store(addr, temp);
//
//            if one of the thread will superseded before step #3, it will actually roll back the counter (for all other threads) by writing
//            the old value into globalTag.
//            */
//            uint32_t tag = _global_tag++;
//
//            tagged_index node_value;
//            node_value.p.offset = (uint32_t) (pHead - _data);
//            node_value.p.tag = tag;
//
//            tagged_index head_value;
//            head_value.u = _head.load();
//
//            while (true)
//            {
//                // store current head as node->next
//                *((tagged_index*) (pTail)) = head_value;
//
//                // try to swap node and head
//                if (_head.compare_exchange(head_value.u, node_value.u))
//                {
//                    // succes
//                    break;
//                }
//                // can't swap values, head is changed (now headValue has new head loaded) try again
//            }
//        }
//
//        inline const uint8_t* data() const noexcept { return _data; }
//
//      private:
//        union tagged_index
//        {
//            struct
//            {
//                uint32_t tag;
//                uint32_t offset;
//            } p;
//
//            uint64_t u;
//            static const uint64_t invalid = (__fst::numeric_limits<uint64_t>::max)();
//        };
//
//        __fst::atomic<uint64_t> _head;
//        __fst::atomic<uint32_t> _global_tag;
//        uint8_t* _data;
//        uint8_t* _data_end;
//    };
//
//    class Allocator
//    {
//      public:
//        inline Allocator(size_t _bucketsCount, size_t _bucketSizeInBytes) noexcept
//            : _buckets_count(_bucketsCount)
//            , _bucket_size(_bucketSizeInBytes)
//            , pBuffer(nullptr)
//        {
//
//            fst_assert(_bucketsCount > 0 && _bucketsCount <= 64);
//            if (_bucketsCount == 0) { return; }
//
//            size_t max_alignment = __fst::round_to_power_of_two(16 * _buckets_count);
//            _bucket_size = __fst::align(_bucketSizeInBytes, max_alignment);
//
//            size_t totalBytesCount = _bucket_size * _buckets_count;
//            pBuffer.reset((uint8_t*)__fst::aligned_allocate(totalBytesCount, max_alignment));
//
//            size_t elementSize = 16;
//            for (size_t i = 0; i < _buckets_count; i++)
//            {
//                uint8_t* bdata = pBuffer.get() + i * _bucket_size;
//                fst_assert(__fst::is_aligned(bdata, __fst::round_to_power_of_two(elementSize)) && "Alignment failed");
//
//                pool_bucket& bucket = _buckets[i];
//                bucket.create(elementSize, bdata, bdata + _bucket_size);
//                elementSize += 16;
//            }
//        }
//
//        FST_ALWAYS_INLINE void* aligned_allocate(size_t size, size_t alignment) noexcept
//        {
//            fst_assert(alignment <= kMaxValidAlignment);
//
//            if (FST_UNLIKELY(size == 0)) { return (void*) alignment; }
//
//            size_t bucket_index = ((__fst::maximum(size, alignment) - 1) >> 4);
//
//            while (bucket_index < _buckets_count)
//            {
//                if (void* ptr = _buckets[bucket_index].allocate()) { return ptr; }
//
//                // try next bucket
//                bucket_index++;
//            }
//
//            // fallback to generic allocator
//            return __fst::aligned_allocate(size, alignment);
//        }
//
//        FST_ALWAYS_INLINE void aligned_deallocate(void* ptr) noexcept
//        {
//            // Assume that p is the pointer that is allocated by passing the zero size.
//            if (FST_UNLIKELY(!is_readable(ptr))) { return; }
//
//            if (size_t bucket_index = find_bucket(ptr); bucket_index < _buckets_count)
//            {
//                pool_bucket* bucket = &_buckets[bucket_index];
//                bucket->deallocate_interval(ptr, ptr);
//                return;
//            }
//
//            // fallback to generic allocator
//            __fst::aligned_deallocate(ptr);
//        }
//
//      private:
//        size_t _buckets_count;
//        size_t _bucket_size;
//        __fst::array<pool_bucket, SMM_MAX_BUCKET_COUNT> _buckets;
//        __fst::unique_ptr<uint8_t> pBuffer;
//
//        static const size_t kMaxValidAlignment = 16384;
//
//        FST_ALWAYS_INLINE static bool is_readable(void* p) noexcept { return (uintptr_t(p) > kMaxValidAlignment); }
//
//        FST_ALWAYS_INLINE size_t find_bucket(const void* p) const noexcept
//        {
//            // if p is our pointer it located inside pBuffer and pBufferEnd
//            // pBuffer and bucketsDataBegin[0] the same pointers
//            uintptr_t index = (uintptr_t) p - (uintptr_t) _buckets[0].data();
//
//            // if p is less than pBuffer we get very huge number due to overflow and this is valid result, since bucketIndex checked before use
//            // if p is greater than pBufferEnd we get huge number and this is valid result too, since bucketIndex checked before use
//            return (index / _bucket_size);
//        }
//    };
//} // namespace sm
//
//FST_BEGIN_NAMESPACE
//
//FST_END_NAMESPACE
