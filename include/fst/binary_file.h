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

#include "fst/common.h"
#include "fst/memory.h"
#include "fst/traits.h"
#include "fst/status_code.h"
#include "fst/utility.h"
#include "fst/vector.h"
#include "fst/small_vector.h"
#include "fst/string.h"
#include "fst/small_string.h"
#include "fst/memory_range.h"
#include "fst/file_view.h"
#include "fst/file.h"
#include "fst/memory_pool.h"

FST_BEGIN_NAMESPACE

    struct data_file
    {
        static constexpr size_t data_header_id_size = 4;
        static constexpr size_t header_id_size = 4;
        static constexpr size_t chunk_id_size = 8;
        static constexpr size_t data_alignment = __fst::default_alignment;

        //
        FST_PACKED_START
        struct data_chunk_header
        {
            static constexpr size_t uid_size = data_header_id_size;

            char uid[uid_size];
            uint8_t type;
            uint8_t offset;
            uint16_t data_0;
            uint32_t size;
            uint32_t reserved;
        };
        FST_PACKED_END

        //
        FST_PACKED_START
        struct chunk_info
        {
            static constexpr size_t uid_size = chunk_id_size;

            char uid[uid_size];
            uint32_t size;
            uint16_t udata;
            uint16_t reserved;
        };
        FST_PACKED_END

        //
        FST_PACKED_START
        struct header
        {
            static constexpr size_t uid_size = header_id_size;

            char uid[uid_size];
            uint8_t type;
            uint8_t key;
            uint16_t n_chunk;
            uint32_t size;
            uint32_t reserved;

            static inline header create(const char* uid, uint8_t type, uint16_t n_chunk, uint32_t file_size) noexcept
            {
                return header{ { uid[0], uid[1], uid[2], uid[3] }, type, 0, n_chunk, file_size, 0 };
            }

            inline const chunk_info* get_chunk_info_from_offset(size_t offset) const noexcept
            {
                return reinterpret_cast<const chunk_info*>(reinterpret_cast<const uint8_t*>(this) + offset);
            }
        };
        FST_PACKED_END

        static_assert(sizeof(data_chunk_header) == 16, "sizeof(data_chunk_header) should be 16.");
        static_assert(sizeof(chunk_info) == 16, "sizeof(chunk_info) should be 16.");
        static_assert(sizeof(header) == 16, "sizeof(header) should be 16.");

        static inline constexpr size_t get_chunk_info_offset(size_t index) noexcept { return sizeof(header) + index * sizeof(chunk_info); }

        // reader.
        template <class _MemoryCategory, class _MemoryZone>
        class reader
        {
          public:
            using size_type = size_t;
            using memory_category_type = _MemoryCategory;
            using memory_zone_type = _MemoryZone;

            using name_vector_type = __fst::small_vector<__fst::string_view, 8, alignof(__fst::string_view), _MemoryZone, _MemoryCategory>;
            using data_vector_type = __fst::small_vector<__fst::byte_view, 8, alignof(__fst::byte_view), _MemoryZone, _MemoryCategory>;

            __fst::error_result load(const char* file_path) noexcept
            {
                _file.close();

                if (__fst::error_result err = _file.open(file_path)) { return err; }

                if (_file.size() < sizeof(data_file::header)) { return __fst::status_code::invalid_argument; }

                return load(__fst::byte_view(_file.data(), _file.size()));
            }

            __fst::error_result load(__fst::byte_view bv) noexcept
            {
                _names.clear();
                _data.clear();
                if (bv.size() < sizeof(data_file::header)) { return __fst::status_code::invalid_argument; }

                const data_file::header& h = *(const data_file::header*) bv.data();

                if (__fst::string_view(h.uid, data_file::header::uid_size) != "fstb") { return __fst::status_code::invalid_argument; }

                if (h.n_chunk == 0) { return __fst::status_code::invalid_argument; }

                uint32_t offset = (uint32_t) (sizeof(data_file::header) + h.n_chunk * sizeof(data_file::chunk_info));

                for (size_t i = 0; i < h.n_chunk; i++)
                {
                    size_t c_offset = data_file::get_chunk_info_offset(i);
                    if (c_offset + sizeof(data_file::chunk_info) > bv.size()) { return __fst::status_code::invalid_argument; }

                    const data_file::chunk_info* c = h.get_chunk_info_from_offset(c_offset);

                    if (c->size == 0) { continue; }

                    if (offset + c->size > bv.size()) { return __fst::status_code::invalid_argument; }

                    _names.push_back(__fst::string_view((const char*) c->uid, ::strnlen((const char*) c->uid, data_file::chunk_info::uid_size)));
                    _data.push_back(__fst::byte_view(bv.data() + offset, c->size));

                    offset += (uint32_t) __fst::align(c->size, data_file::data_alignment);
                }

                return __fst::status_code::success;
            }

            inline __fst::byte_view get_data(__fst::string_view name) const noexcept
            {
                for (size_t i = 0; i < _names.size(); i++)
                {
                    if (_names[i] == name) { return _data[i]; }
                }

                return __fst::byte_view();
            }

            inline __fst::string_view get_data_string(__fst::string_view name) const noexcept
            {
                for (size_t i = 0; i < _names.size(); i++)
                {
                    if (_names[i] == name) { return __fst::string_view(_data[i].template data<const char>(), _data[i].size()); }
                }

                return __fst::string_view();
            }

            inline __fst::byte_view operator[](__fst::string_view name) const noexcept { return get_data(name); }

            inline bool contains(__fst::string_view name) const noexcept
            {
                for (const auto& n : _names)
                {
                    if (n == name) { return true; }
                }

                return false;
            }

            inline __fst::memory_view<__fst::string_view> get_names() const noexcept { return _names; }

          private:
            name_vector_type _names;
            data_vector_type _data;
            __fst::file_view _file;
        };

        // file_reader.
        template <class _MemoryCategory, class _MemoryZone>
        class file_reader : public reader<_MemoryCategory, _MemoryZone>
        {
          public:
            using reader_type = reader<_MemoryCategory, _MemoryZone>;
            __fst::error_result load(const char* file_path) noexcept
            {
                _file.close();

                if (__fst::error_result err = _file.open(file_path)) { return err; }

                if (_file.size() < sizeof(data_file::header)) { return __fst::status_code::invalid_argument; }

                return reader_type::load(__fst::byte_view(_file.data(), _file.size()));
            }

          private:
            __fst::file_view _file;
        };

        // writer.
        template <class _MemoryCategory, class _MemoryZone>
        class writer
        {
          public:
            using size_type = size_t;
            using memory_category_type = _MemoryCategory;
            using memory_zone_type = _MemoryZone;

            using string_type = __fst::small_string<data_file::chunk_id_size>;

            inline __fst::error_result add_chunk(const string_type& name, __fst::byte_view data, bool copy_data, uint16_t udata = 0) noexcept
            {
                // Make sure data is not empty.
                if (data.empty()) { return __fst::status_code::empty_data; }

                // Make sure name doesn't already exist.
                if (contains(name)) { return __fst::status_code::name_exists; }

                if (copy_data)
                {
                    void* buffer = _pool.aligned_allocate(data.size());
                    if (!buffer) { return __fst::status_code::not_enough_memory; }

                    __fst::memcpy(buffer, data.data(), data.size());
                    data = __fst::byte_view((const __fst::byte*) buffer, data.size());

                    //_chunks.push_back(data_info{ name, __fst::byte_view((const __fst::byte*) buffer, data.size()), udata, nullptr });

                    //_write_size += sizeof(data_file::chunk_info);
                    //_write_size += __fst::align(data.size(), data_file::data_alignment);
                }

                _write_size += sizeof(data_file::chunk_info);
                _write_size += __fst::align(data.size(), data_file::data_alignment);

                _chunks.push_back(data_info{ name, data, udata, nullptr });

                return __fst::status_code::success;
            }

            inline __fst::error_result add_chunk(const string_type& name, writer& wrt, bool copy_data, uint16_t udata = 0) noexcept
            {
                // Make sure data is not empty.
                //if (data.empty()) { return __fst::status_code::empty_data; }

                // Make sure name doesn't already exist.
                if (contains(name)) { return __fst::status_code::name_exists; }

                if (copy_data)
                {
                    size_t data_size = wrt.write_size();

                    void* buffer = _pool.aligned_allocate(data_size);
                    if (!buffer) { return __fst::status_code::not_enough_memory; }

                    wrt.write_to_buffer(__fst::byte_range((__fst::byte*) buffer, data_size));
                    //__fst::memcpy(buffer, data.data(), data.size());

                    _chunks.push_back(data_info{ name, __fst::byte_view((const __fst::byte*) buffer, data_size), udata, nullptr });

                    _write_size += sizeof(data_file::chunk_info);
                    _write_size += __fst::align(data_size, data_file::data_alignment);
                }
                else
                {

                    _write_size += sizeof(data_file::chunk_info);
                    _write_size += __fst::align(wrt.write_size(), data_file::data_alignment);

                    _chunks.push_back(data_info{ name, __fst::byte_view(), udata, &wrt });
                }

                /*size_t total_file_size = sizeof(data_file::header) + _chunks.size() * sizeof(data_file::chunk_info);
                for (size_t i = 0; i < _chunks.size(); i++)
                {
                    if (_chunks[i].wrt) { total_file_size += __fst::align(_chunks[i].wrt->write_size(), data_file::data_alignment); }
                    else { total_file_size += __fst::align(_chunks[i].data.size(), data_file::data_alignment); }
                }*/

                return __fst::status_code::success;
            }

            inline bool contains(const string_type& name) const noexcept
            {
                for (const auto& n : _chunks)
                {
                    if (n.name == name) { return true; }
                }

                return false;
            }

            inline size_t write_size() const noexcept
            {
                return _write_size;
                /* size_t total_file_size = sizeof(data_file::header) + _chunks.size() * sizeof(data_file::chunk_info);
                for (size_t i = 0; i < _chunks.size(); i++)
                {
                    if (_chunks[i].wrt) { total_file_size += __fst::align(_chunks[i].wrt->write_size(), data_file::data_alignment); }
                    else { total_file_size += __fst::align(_chunks[i].data.size(), data_file::data_alignment); }
                }

                return total_file_size;*/
            }

            inline __fst::error_result write_to_file(const char* filepath) const noexcept
            {
                __fst::file output_file;
                if (__fst::error_result err = output_file.open(filepath, __fst::open_mode::write | __fst::open_mode::create_always)) { return err; }

                __fst::error_result err = internal_write<__fst::file, const char*, size_t>(output_file, write_size());

                output_file.close();

                return err;
            }

            inline __fst::error_result write_to_stream(__fst::output_stream<__fst::byte>& stream) const noexcept
            {
                __fst::error_result err = internal_write<__fst::output_stream<__fst::byte>, const __fst::byte*, size_t>(stream, write_size());
                return err;
            }

            struct dsjklds
            {
                __fst::byte_range data;
                size_t _write_index = 0;

                FST_NODISCARD size_t write(const void* buffer, size_t buffer_size) noexcept

                {
                    __fst::memcpy((void*) (data.data() + _write_index), buffer, buffer_size);
                    _write_index += buffer_size;
                    return buffer_size;
                }
                //FST_NODISCARD size_t size() const noexcept;
            };

            inline __fst::error_result write_to_buffer(__fst::byte_range data) const noexcept
            {
                const size_t sz = write_size();
                if (data.size() < sz) { return __fst::status_code::no_buffer_space; }

                dsjklds jk;
                jk.data = data;

                __fst::output_stream<__fst::byte> stream{ &jk,
                    [](void* data, const __fst::byte* str, size_t size, stream_modifier) noexcept -> size_t
                    {
                        dsjklds* str_ptr = (dsjklds*) data;
                        return str_ptr->write(str, size);
                    } };

                return write_to_stream(stream);
                //return internal_write_to_buffer(data.data(), sz);
            }

          private:
            struct data_info
            {
                string_type name;
                __fst::byte_view data;
                uint16_t udata;
                writer* wrt;
            };

            using memory_pool_type = __fst::forward_memory_pool<_MemoryCategory, _MemoryZone>;
            using name_vector_type = __fst::small_vector<data_info, 8, alignof(data_info), _MemoryZone, _MemoryCategory>;

            name_vector_type _chunks;
            memory_pool_type _pool;
            size_t _write_size = sizeof(data_file::header);

            template <class _Writer, class _DataPtrType, class _DataSizeType>
            inline __fst::error_result internal_write(_Writer& w, size_t file_size) const noexcept
            {
                using data_ptr_type = _DataPtrType;
                using data_size_type = _DataSizeType;

                data_file::header h = data_file::header::create("fstb", 0, (uint16_t) _chunks.size(), (uint32_t) file_size);
                //data_file::header h{ { 'f', 's', 't', 'b' }, (uint16_t) _chunks.size(), 0, 0 };

                if (const size_t sz = w.write((data_ptr_type) &h, (data_size_type) sizeof(data_file::header)); sz != sizeof(data_file::header))
                {
                    return __fst::status_code::no_buffer_space;
                }

                for (size_t i = 0; i < _chunks.size(); i++)
                {
                    data_file::chunk_info c_info;
                    __fst::memset((void*) &c_info, 0, sizeof(data_file::chunk_info));
                    __fst::memcpy((void*) &c_info.uid, _chunks[i].name.data(), _chunks[i].name.size());

                    c_info.udata = _chunks[i].udata;
                    c_info.size = _chunks[i].wrt ? (uint32_t) _chunks[i].wrt->write_size() : (uint32_t) _chunks[i].data.size();

                    if (const size_t sz = w.write((data_ptr_type) &c_info, (data_size_type) sizeof(data_file::chunk_info)); sz != sizeof(data_file::chunk_info))
                    {
                        return __fst::status_code::no_buffer_space;
                    }
                }

                const uint8_t empty_buffer[16] = { 0 };

                for (size_t i = 0; i < _chunks.size(); i++)
                {
                    if (_chunks[i].wrt) { _chunks[i].wrt->template internal_write<_Writer, _DataPtrType, _DataSizeType>(w, _chunks[i].wrt->write_size()); }
                    else
                    {

                        __fst::byte_view data = _chunks[i].data;

                        if (const size_t sz = w.write((data_ptr_type) data.data(), (data_size_type) data.size()); sz != data.size())
                        {
                            return __fst::status_code::no_buffer_space;
                        }

                        if (data_size_type delta = __fst::align(data.size(), data_file::data_alignment) - data.size())
                        {
                            fst_assert(delta <= 16);
                            if (const size_t sz = w.write((data_ptr_type) &empty_buffer[0], delta); sz != delta) { return __fst::status_code::no_buffer_space; }
                        }
                    }
                }

                return __fst::status_code::success;
            }

            //            inline __fst::error_result internal_write_to_buffer(__fst::byte* __data, size_t file_size) const noexcept
            //            {
            //                size_t offset = 0;
            //                data_file::header h = data_file::header::create("fstb", 0, (uint16_t) _chunks.size(), (uint32_t) file_size);
            //
            //                //data_file::header h{ { 'f', 's', 't', 'b' }, (uint16_t) _chunks.size(), 0, 0 };
            //                {
            //                    __fst::memcpy(__data, (const __fst::byte*) &h, sizeof(data_file::header));
            //                    offset += sizeof(data_file::header);
            //
            //                    for (size_t i = 0; i < _chunks.size(); i++)
            //                    {
            //                        data_file::chunk_info c_info;
            //                        __fst::memset((void*) &c_info, 0, sizeof(data_file::chunk_info));
            //                        __fst::memcpy((void*) &c_info.uid, _chunks[i].name.data(), _chunks[i].name.size());
            //
            //                        //c_info.size = (uint32_t) _chunks[i].data.size();
            //                                            c_info.size =  _chunks[i].wrt ? (uint32_t)_chunks[i].wrt->write_size() : (uint32_t)_chunks[i].data.size();
            //
            //                        c_info.udata = _chunks[i].udata;
            //
            //                        __fst::memcpy(__data + offset, (const void*) &c_info, sizeof(data_file::chunk_info));
            //                        offset += sizeof(data_file::chunk_info);
            //                    }
            //
            //                    for (size_t i = 0; i < _chunks.size(); i++)
            //                    {
            //                         if (_chunks[i].wrt) {
            //
            //                        _chunks[i].wrt->internal_write_to_buffer(__data + offset, _chunks[i].wrt->write_size());
            //                                                offset += __fst::align(_chunks[i].wrt->write_size(), data_file::data_alignment);
            //
            //}else {
            //                        __fst::byte_view data = _chunks[i].data;
            //                        __fst::memcpy(__data + offset, (const void*) data.data(), data.size());
            //                        offset += __fst::align(data.size(), data_file::data_alignment);
            //                        }
            //                    }
            //
            //                    return __fst::status_code::success;
            //                }
            //            }
        };
    };

FST_END_NAMESPACE
