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

#include "fst/memory.h"
#include "fst/pointer.h"
#include "fst/array.h"
#include "fst/atomic.h"
#include "fst/stack_vector.h"
#include "fst/math.h"
#include "fst/memory_range.h"

FST_BEGIN_NAMESPACE
    namespace detail
    {
        ///
        struct pool_bucket
        {
            inline pool_bucket() noexcept
                : _head(tagged_index::invalid)
                , _global_tag(0)
            {}

            pool_bucket(const pool_bucket&) = delete;
            pool_bucket(pool_bucket&&) = delete;

            ~pool_bucket() noexcept = default;

            pool_bucket& operator=(const pool_bucket&) = delete;
            pool_bucket& operator=(pool_bucket&&) = delete;

            inline void create(size_t element_size, __fst::byte_range buffer) noexcept
            {
                //fst_assert(element_size >= 16 && "Invalid element size");
                _buffer = buffer;

                // build inplace single linked list
                _global_tag.store(0);
                uint8_t* node = _buffer.data();

                tagged_index head_value;
                head_value.p.tag = _global_tag.load();
                head_value.p.offset = (uint32_t) (node - _buffer.data());
                _head.store(head_value.u);

                while (true)
                {
                    uint8_t* next = node + element_size;
                    if ((next + element_size) <= _buffer.end())
                    {
                        tagged_index next_value;
                        next_value.p.tag = _global_tag.load();
                        next_value.p.offset = (uint32_t) (next - _buffer.data());
                        *((tagged_index*) (node)) = next_value;
                    }
                    else
                    {
                        // last node
                        ((tagged_index*) (node))->u = tagged_index::invalid;
                        break;
                    }

                    node = next;
                    ++_global_tag;
                }
            }

            FST_NODISCARD inline void* allocate() noexcept
            {
                tagged_index head_value;
                head_value.u = _head.load();

                uint8_t* p = nullptr;
                while (true)
                {
                    // list is empty, exiting
                    if (head_value.u == tagged_index::invalid) { return nullptr; }

                    // get head pointer
                    p = _buffer.data() + head_value.p.offset;

                    // read head->next value
                    tagged_index next_value = *((tagged_index*) p);

                    // try to swap head to head->next
                    if (_head.compare_exchange(head_value.u, next_value.u))
                    {
                        // success
                        break;
                    }
                    // can't swap values, head is changed (now head_value has new head loaded) try again
                }

                return p;
            }

            inline void deallocate_interval(void* _pHead, void* _pTail) noexcept
            {
                uint8_t* pHead = (uint8_t*) _pHead;
                uint8_t* pTail = (uint8_t*) _pTail;
                uint32_t tag = _global_tag++;

                tagged_index node_value;
                node_value.p.offset = (uint32_t) (pHead - _buffer.data());
                node_value.p.tag = tag;

                tagged_index head_value;
                head_value.u = _head.load();

                while (true)
                {
                    // store current head as node->next
                    *((tagged_index*) pTail) = head_value;

                    // try to swap node and head
                    if (_head.compare_exchange(head_value.u, node_value.u))
                    {
                        // succes
                        break;
                    }
                    // can't swap values, head is changed (now head_value has new head loaded) try again
                }
            }

            FST_NODISCARD FST_ALWAYS_INLINE const uint8_t* data() const noexcept { return _buffer.data(); }

          private:
            union tagged_index
            {
                struct
                {
                    uint32_t tag;
                    uint32_t offset;
                } p;

                uint64_t u;
                static constexpr uint64_t invalid = (__fst::numeric_limits<uint64_t>::max)();
            };

            __fst::byte_range _buffer;
            __fst::atomic<uint64_t> _head;
            __fst::atomic<uint32_t> _global_tag;
        };
    } // namespace detail

    ///
    template <class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    class small_memory_pool
    {
      public:
        static constexpr size_t minimum_alignment = 16;
        static constexpr size_t minimum_alignment_bit_index = __fst::power_of_two_bit_index(minimum_alignment);

        static constexpr size_t maximum_alignment = 512;
        static constexpr size_t maximum_bucket_count = 8;

        using buffer_pointer = __fst::optional_ptr<uint8_t, _MemoryCategory, _MemoryZone>;

        inline small_memory_pool(size_t buckets_count, size_t bucket_size, __fst::byte_range buffer = __fst::byte_range()) noexcept
            : _buckets_count(buckets_count)
            , _bucket_size(bucket_size)
        {
            fst_assert(buckets_count > 0 && buckets_count <= maximum_bucket_count);
            if (buckets_count == 0) { return; }

            const size_t alignment = __fst::round_to_power_of_two(minimum_alignment * _buckets_count);
            _bucket_size = __fst::align(bucket_size, alignment);

            const size_t total_size = _bucket_size * _buckets_count;
            if (buffer.size() >= total_size) { _buffer = buffer_pointer(buffer.data(), false); }
            else { _buffer = buffer_pointer((uint8_t*) _MemoryZone::aligned_allocate(total_size, alignment, _MemoryCategory::id()), true); }

            _buckets.resize(_buckets_count);

            {
                size_t element_size = minimum_alignment;
                uint8_t* data = _buffer.get();
                for (size_t i = 0; i < _buckets_count; i++)
                {
                    fst_assert(__fst::is_aligned(data, __fst::round_to_power_of_two(element_size)), "Alignment failed");

                    _buckets[i].create(element_size, __fst::byte_range(data, _bucket_size));

                    element_size += minimum_alignment;
                    data += _bucket_size;
                }
            }
        }

        small_memory_pool(const small_memory_pool&) = delete;
        small_memory_pool(small_memory_pool&&) = delete;

        ~small_memory_pool() noexcept = default;

        small_memory_pool& operator=(const small_memory_pool&) = delete;
        small_memory_pool& operator=(small_memory_pool&&) = delete;

        FST_NODISCARD FST_ALWAYS_INLINE void* aligned_allocate(size_t size, size_t alignment) noexcept
        {
            fst_assert(size);
            fst_assert(alignment <= maximum_alignment);

            if (FST_UNLIKELY(size == 0)) { return nullptr; }

            size_t bucket_index = (__fst::maximum(size, alignment) - 1) >> minimum_alignment_bit_index;
            while (bucket_index < _buckets_count)
            {
                if (void* ptr = _buckets[bucket_index++].allocate()) { return ptr; }
            }

            // fallback to generic allocator
            return _MemoryZone::aligned_allocate(size, alignment, _MemoryCategory::id());
        }

        FST_ALWAYS_INLINE void aligned_deallocate(void* ptr) noexcept
        {
            fst_assert(ptr);

            if (size_t bucket_index = find_bucket(ptr); bucket_index < _buckets_count) { return _buckets[bucket_index].deallocate_interval(ptr, ptr); }

            // fallback to generic allocator
            _MemoryZone::aligned_deallocate(ptr, _MemoryCategory::id());
        }

      private:
        size_t _buckets_count;
        size_t _bucket_size;
        __fst::stack_vector<detail::pool_bucket, maximum_bucket_count> _buckets;
        buffer_pointer _buffer;

        FST_NODISCARD FST_ALWAYS_INLINE size_t find_bucket(const void* p) const noexcept { return ((uintptr_t) p - (uintptr_t) _buckets[0].data()) / _bucket_size; }
    };

    ///
    template <size_t _Size, size_t _Alignment, class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    class object_memory_pool
    {
      public:
        static constexpr size_t bucket_size = __fst::maximum(__fst::align(_Size, _Alignment), _Alignment);

        static constexpr size_t minimum_alignment = _Alignment;
        static constexpr size_t minimum_alignment_bit_index = __fst::power_of_two_bit_index(_Alignment);

        //static constexpr size_t maximum_alignment = 512;
        //static constexpr size_t maximum_bucket_count = 8;

        using buffer_pointer = __fst::optional_ptr<uint8_t, _MemoryCategory, _MemoryZone>;

        inline object_memory_pool(size_t buckets_count, __fst::byte_range buffer = __fst::byte_range()) noexcept
            : _buckets_count(buckets_count)
        //, _bucket_size(bucket_size)
        {
            fst_assert(buckets_count > 0);
            if (buckets_count == 0) { return; }

            //const size_t alignment = __fst::round_to_power_of_two(minimum_alignment * _buckets_count);
            //_bucket_size = __fst::align(bucket_size, alignment);

            const size_t total_size = bucket_size * _buckets_count;
            if (buffer.size() >= total_size) { _buffer = buffer_pointer(buffer.data(), false); }
            else { _buffer = buffer_pointer((uint8_t*) _MemoryZone::aligned_allocate(total_size, _Alignment, _MemoryCategory::id()), true); }

            if (!_buffer)
            {
                fst_error("Allocation failed");
                return;
            }

            uint8_t* data = _buffer.get();
            fst_assert(__fst::is_aligned(data, _Alignment), "Alignment failed");
            _bucket.create(bucket_size, __fst::byte_range(data, bucket_size));
        }

        object_memory_pool(const object_memory_pool&) = delete;
        object_memory_pool(object_memory_pool&&) = delete;

        ~object_memory_pool() noexcept = default;

        object_memory_pool& operator=(const object_memory_pool&) = delete;
        object_memory_pool& operator=(object_memory_pool&&) = delete;

        FST_NODISCARD FST_ALWAYS_INLINE void* aligned_allocate(size_t size, size_t alignment) noexcept
        {
            fst_assert(size);
            fst_assert(alignment <= _Alignment);

            if (FST_UNLIKELY(size == 0)) { return nullptr; }

            size_t bucket_index = (__fst::maximum(size, alignment) - 1) >> minimum_alignment_bit_index;

            if (bucket_index == 0)
            {
                if (void* ptr = _bucket.allocate()) { return ptr; }
            }

            // fallback to generic allocator
            return _MemoryZone::aligned_allocate(size, alignment, _MemoryCategory::id());
        }

        FST_ALWAYS_INLINE void aligned_deallocate(void* ptr) noexcept
        {
            fst_assert(ptr);

            if (size_t bucket_index = find_bucket(ptr); bucket_index == 0) { return _bucket.deallocate_interval(ptr, ptr); }

            // fallback to generic allocator
            _MemoryZone::aligned_deallocate(ptr, _MemoryCategory::id());
        }

      private:
        size_t _buckets_count;
        detail::pool_bucket _bucket;
        buffer_pointer _buffer;

        FST_NODISCARD FST_ALWAYS_INLINE size_t find_bucket(const void* p) const noexcept { return ((uintptr_t) p - (uintptr_t) _bucket.data()) / bucket_size; }
    };

FST_END_NAMESPACE
