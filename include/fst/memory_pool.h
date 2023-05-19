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
        static constexpr size_t minimum_alignment = __fst::default_alignment;
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

    /// This allocator allocate memory blocks from pre-allocated memory chunks.
    ///
    /// The memory chunks are allocated by _MemoryZone, which is fst::default_memory_zone by default.
    ///
    /// User may also supply a buffer as the first chunk.
    ///
    /// If the user-buffer is full then additional chunks are allocated by _MemoryZone.
    ///
    /// The user-buffer is not deallocated by this allocator.
    ///
    template <class _MemoryCategory = __fst::default_memory_category, class _MemoryZone = __fst::default_memory_zone>
    class forward_memory_pool
    {
      public:
        using size_type = size_t;
        using chunk_size_type = uint32_t;
        using memory_category_type = _MemoryCategory;
        using memory_zone_type = _MemoryZone;

        static_assert(__fst::is_static_memory_zone<memory_zone_type>::value, "memory_zone_type must a static");

      private:
        // Chunk header for perpending to each chunk.
        // Chunks are stored as a singly linked list.
        struct chunk_header
        {
            chunk_header* next; // Next chunk in the linked list.
            chunk_size_type capacity; // Capacity of the chunk in bytes (excluding the header itself).
            chunk_size_type size; // Current size of allocated memory in bytes.
        };

        struct shared_data
        {
            __fst::packed_pointer data;
            size_type refcount;

            // Head of the chunk linked-list. Only the head chunk serves allocation.
            inline chunk_header* head() const noexcept { return data.get_pointer<chunk_header*>(); }
            inline void set_head(chunk_header* h) noexcept { data.set_pointer(h); }

            inline void set_owned(bool owned) noexcept { data.set_int(owned); }
            inline bool is_owned() const noexcept { return data.get_int<bool>(); }
        };

      public:
        static constexpr size_type default_chunk_capacity = 64 * 1024;
        static constexpr size_type shared_data_size = __fst::align(sizeof(shared_data), __fst::default_alignment);
        static constexpr size_type chunk_header_size = __fst::align(sizeof(chunk_header), __fst::default_alignment);
        static constexpr size_type total_reserved_size = shared_data_size + chunk_header_size;
        static constexpr size_type chunk_alignment = __fst::default_alignment;

        /// @param chunk_size The size of memory chunk, default is default_chunk_capacity.
        /// @param allocate_chunk Whether or not the chunk_size should be pre allocated.
        inline explicit forward_memory_pool(size_type chunk_size = default_chunk_capacity, bool allocate_chunk = false) noexcept
            : _chunk_capacity(chunk_size)
        {
            fst_assert(chunk_size, "chunk_size cannot be zero");

            const chunk_size_type init_capacity = allocate_chunk ? (chunk_size_type) chunk_size : 0;

            _shared = (shared_data*) memory_zone_type::aligned_allocate(total_reserved_size + init_capacity, chunk_alignment, memory_category_type::id());

            if (!_shared)
            {
                fst_error("allocation failed");
                return;
            }

            chunk_header* head = get_chunk_head(_shared);
            _shared->data.set_pointer(head);
            head->capacity = init_capacity;
            head->size = 0;
            head->next = nullptr;
            _shared->set_owned(true);
            _shared->refcount = 1;
        }

        /// The user buffer will be used firstly. When it is full, memory pool allocates new chunk with chunk size.
        ///
        /// The user buffer will not be deallocated when this allocator is destructed.
        ///
        /// @param buffer User supplied buffer.
        /// @param size Size of the buffer in bytes. It must at least larger than sizeof(chunk_header).
        /// @param chunk_size The size of memory chunk. The default is default_chunk_capacity.
        inline forward_memory_pool(__fst::not_null<void*> buffer, size_type size, size_type chunk_size = default_chunk_capacity) noexcept
            : _chunk_capacity(chunk_size)
        {
            size_t sp = size;
            _shared = (shared_data*) __fst::align(chunk_alignment, size, buffer, sp);
            fst_assert(sp >= total_reserved_size);

            if (!_shared)
            {
                fst_error("allocation failed");
                return;
            }

            chunk_header* head = get_chunk_head(_shared);
            _shared->data.set_pointer(head);
            head->capacity = (chunk_size_type) (size - total_reserved_size);
            head->size = 0;
            head->next = nullptr;
            _shared->set_owned(false);
            _shared->refcount = 1;
        }

        inline forward_memory_pool(const forward_memory_pool& rhs) noexcept
            : _chunk_capacity(rhs._chunk_capacity)
            , _shared(rhs._shared)
        {
            fst_assert(_shared);

            fst_assert(_shared->refcount > 0);
            ++_shared->refcount;
        }

        inline forward_memory_pool(forward_memory_pool&& rhs) noexcept
            : _chunk_capacity(rhs._chunk_capacity)
            , _shared(__fst::exchange(rhs._shared, nullptr))
        {}

        /// This deallocates all memory chunks, excluding the user-supplied buffer.
        inline ~forward_memory_pool() noexcept { reset(); }

        inline forward_memory_pool& operator=(const forward_memory_pool& rhs) noexcept
        {
            fst_assert(rhs._shared);
            fst_assert(rhs._shared->refcount > 0);

            ++rhs._shared->refcount;
            reset();

            _chunk_capacity = rhs._chunk_capacity;
            _shared = rhs._shared;
            return *this;
        }

        inline forward_memory_pool& operator=(forward_memory_pool&& rhs) noexcept
        {
            fst_assert(rhs._shared);
            fst_assert(rhs._shared->refcount > 0);
            reset();
            _chunk_capacity = rhs._chunk_capacity;
            _shared = __fst::exchange(rhs._shared, nullptr);
            return *this;
        }

        /// Deallocates all memory chunks, excluding the first/user one.
        void clear() noexcept
        {
            fst_assert(_shared);
            fst_assert(_shared->refcount > 0);
            while (true)
            {
                chunk_header* c = _shared->head();

                if (!c->next) { break; }

                _shared->set_head(c->next);
                memory_zone_type::aligned_deallocate(c, memory_category_type::id());
            }

            _shared->head()->size = 0;
        }

        /// Computes the total capacity of allocated memory chunks.
        /// @return total capacity in bytes.
        FST_NODISCARD inline size_type capacity() const noexcept
        {
            fst_assert(_shared->refcount > 0);

            size_type capacity = 0;
            for (chunk_header* c = _shared->head(); c; c = c->next)
            {
                capacity += c->capacity;
            }

            return capacity;
        }

        /// Computes the memory blocks allocated.
        /// @return total used bytes.
        FST_NODISCARD inline size_type size() const noexcept
        {
            fst_assert(_shared);
            fst_assert(_shared->refcount > 0);
            size_type size = 0;
            for (chunk_header* c = _shared->head(); c; c = c->next)
            {
                size += c->size;
            }
            return size;
        }

        FST_NODISCARD inline bool is_valid() const noexcept { return _shared != nullptr; }

        /// Whether the allocator is shared.
        FST_NODISCARD inline bool is_shared() const noexcept
        {
            fst_assert(_shared);
            fst_assert(_shared->refcount > 0);
            return _shared->refcount > 1;
        }

        /// Allocates a memory block.
        FST_NODISCARD void* aligned_allocate(size_type size, size_type alignment = __fst::default_alignment) noexcept
        {
            fst_assert(_shared->refcount > 0);
            if (!size) { return nullptr; }

            //size = __fst::align(size, alignment);
            size_t aligned_size = __fst::required_aligned_size(chunk_alignment, size, alignment, true);

            chunk_header* head = _shared->head();

            if (FST_UNLIKELY(head->size + aligned_size > head->capacity))
            {
                if (!add_chunk(_chunk_capacity > aligned_size ? _chunk_capacity : aligned_size)) { return nullptr; }
            }

            head = _shared->head();
            void* buffer = get_chunk_buffer(_shared) + head->size;

            size_t sp = aligned_size;
            buffer = __fst::align(alignment, size, buffer, sp);
            fst_assert(buffer);

            head->size += (chunk_size_type) aligned_size;
            return buffer;
        }

        static void aligned_deallocate(void* ptr) noexcept
        {
            // Does nothing.
            __fst::unused(ptr);
        }

        FST_NODISCARD inline bool operator==(const forward_memory_pool& rhs) const noexcept
        {
            fst_assert(_shared->refcount > 0);
            fst_assert(rhs._shared->refcount > 0);
            return _shared == rhs._shared;
        }

        FST_NODISCARD inline bool operator!=(const forward_memory_pool& rhs) const noexcept { return !operator==(rhs); }

      private:
        static FST_ALWAYS_INLINE chunk_header* get_chunk_head(shared_data* shared) noexcept
        {
            return reinterpret_cast<chunk_header*>(reinterpret_cast<uint8_t*>(shared) + shared_data_size);
        }

        static FST_ALWAYS_INLINE uint8_t* get_chunk_buffer(shared_data* shared) noexcept { return reinterpret_cast<uint8_t*>(shared->head()) + chunk_header_size; }

        inline void reset() noexcept
        {
            if (!_shared)
            {
                // Does nothing if moved.
                return;
            }

            if (_shared->refcount > 1)
            {
                --_shared->refcount;
                return;
            }

            clear();

            if (_shared->is_owned()) { memory_zone_type::aligned_deallocate(_shared, memory_category_type::id()); }
        }

        bool add_chunk(size_type capacity) noexcept
        {
            chunk_header* chunk = (chunk_header*) memory_zone_type::aligned_allocate(chunk_header_size + capacity, chunk_alignment, memory_category_type::id());
            if (!chunk) { return false; }

            chunk->capacity = (chunk_size_type) capacity;
            chunk->size = 0;
            chunk->next = _shared->head();
            _shared->set_head(chunk);
            return true;
        }

        size_type _chunk_capacity;
        shared_data* _shared;
    };

FST_END_NAMESPACE
