#pragma once
#include "fst/common.h"
#include "fst/memory.h"

FST_BEGIN_NAMESPACE

    template <class _T, class _MemoryCategory = __fst::simd_memory_category, class _MemoryZone = __fst::simd_memory_zone>
    class audio_buffer
    {
      public:
        using value_type = _T;
        using size_type = uint32_t;
        using channel_pointer = value_type*;
        using channel_const_pointer = const value_type*;
        using buffer_pointer = value_type* const*;
        using buffer_const_pointer = const value_type* const*;

        static_assert(__fst::is_floating_point<value_type>::value, "au::audio_buffer only works with floating point value type.");

        audio_buffer() noexcept = default;

        inline audio_buffer(const audio_buffer& d) noexcept
        {
            _buffers = (_T**) allocate_audio_buffer(d.channel_size(), d.buffer_size(), sizeof(_T));

            if (_buffers)
            {
                _buffer_size = d.buffer_size();
                _channel_size = d.channel_size();

                for (size_t i = 0; i < _channel_size; i++)
                {
                    __fst::memcpy(channel(i), d.channel(i), _buffer_size * sizeof(_T));
                }
            }
        }

        inline audio_buffer(const audio_buffer& d, __fst::status_code& sc) noexcept
        {

            _buffers = (_T**) allocate_audio_buffer(d.channel_size(), d.buffer_size(), sizeof(_T));

            if (!_buffers)
            {
                sc = __fst::status_code::not_enough_memory;
                return;
            }

            _buffer_size = d.buffer_size();
            _channel_size = d.channel_size();

            for (uint32_t i = 0; i < _channel_size; i++)
            {
                __fst::memcpy(channel(i), d.channel(i), _buffer_size * sizeof(_T));
            }

            sc = __fst::status_code::success;
        }

        inline audio_buffer(size_type __channel_size, size_type __buffer_size, bool clear_memory = true) noexcept
        {

            _buffers = (_T**) allocate_audio_buffer(__channel_size, __buffer_size, sizeof(_T));

            if (_buffers)
            {
                _buffer_size = __buffer_size;
                _channel_size = __channel_size;

                if (clear_memory) { clear_unchecked(); }
            }
        }

        inline audio_buffer(audio_buffer&& d) noexcept
            : _buffers(d._buffers)
            , _buffer_size(d._buffer_size)
            , _channel_size(d._channel_size)
        {
            d._buffers = nullptr;
            d._buffer_size = 0;
            d._channel_size = 0;
        }

        inline ~audio_buffer() noexcept { reset(); }

        inline audio_buffer& operator=(const audio_buffer& d) noexcept
        {
            __fst::status_code sc = resize(d.channel_size(), d.buffer_size());
            if (sc != __fst::status_code::success)
            {
                fst_error("Couldn't resize buffer");
                return *this;
            }

            for (uint32_t i = 0; i < _channel_size; i++)
            {
                __fst::memcpy(channel(i), d.channel(i), _buffer_size * sizeof(value_type));
            }

            return *this;
        }

        inline audio_buffer& operator=(audio_buffer&& d) noexcept
        {
            reset();
            _buffers = d._buffers;
            _buffer_size = d._buffer_size;
            _channel_size = d._channel_size;
            d._buffers = nullptr;
            d._buffer_size = 0;
            d._channel_size = 0;
            return *this;
        }

        ///
        FST_NODISCARD inline bool valid() const noexcept { return (bool) _buffers; }

        ///
        /// {@
        FST_NODISCARD inline size_type channel_size() const noexcept { return _channel_size; }
        FST_NODISCARD inline size_type buffer_size() const noexcept { return _buffer_size; }

        ///
        /// {@
        FST_NODISCARD inline buffer_pointer data() noexcept { return _buffers; }
        FST_NODISCARD inline buffer_const_pointer data() const noexcept { return _buffers; }
        /// @}

        ///
        /// {@
        FST_NODISCARD inline channel_pointer channel(size_type index) noexcept
        {
            fst_assert(index < _channel_size, "Out of bounds index");
            return _buffers[index];
        }

        FST_NODISCARD inline channel_const_pointer channel(size_type index) const noexcept
        {
            fst_assert(index < _channel_size, "Out of bounds index");
            return _buffers[index];
        }
        /// @}

        ///
        /// {@
        FST_NODISCARD inline channel_pointer operator[](size_type index) noexcept
        {
            fst_assert(index < _channel_size, "Out of bounds index");
            return _buffers[index];
        }

        FST_NODISCARD inline channel_const_pointer operator[](size_type index) const noexcept
        {
            fst_assert(index < _channel_size, "Out of bounds index");
            return _buffers[index];
        }
        /// @}

        // FST_NODISCARD inline _AU::audio_wire<value_type> wire(size_type index) noexcept { return _AU::audio_wire<value_type>(_buffers[index], _buffer_size); }

        // FST_NODISCARD inline _AU::audio_wire<const value_type> wire(size_type index) const noexcept
        // {
        // return _AU::audio_wire<const value_type>(_buffers[index], _buffer_size);
        // }

        ///
        inline void clear() noexcept
        {
            if (_buffers) { clear_unchecked(); }
        }

        ///
        inline void reset() noexcept
        {
            deallocate_audio_buffer((void*) _buffers);
            _buffers = nullptr;
            _buffer_size = 0;
            _channel_size = 0;
        }

        ///
        FST_NODISCARD inline __fst::status_code resize(size_type __channel_size, size_type __buffer_size, bool clear_memory = true) noexcept
        {

            fst_assert(__channel_size != 0 && __buffer_size != 0, "channel_size and buffer_size cannot be zero");

            if (_buffers)
            {
                deallocate_audio_buffer((void*) _buffers);
                //// Nothing to do if same as current channel and buffer size.
                //if (_buffer_size == __buffer_size && _channel_size == __channel_size)
                //{
                //    if (clear_memory) { clear_unchecked(); }
                //    return __fst::status_code::success;
                //}

                //_T** buffers = (_T**) reallocate_audio_buffer((void*) _buffers, __channel_size, __buffer_size, sizeof(_T));
                //if (!buffers)
                //{
                //    deallocate_audio_buffer((void*) _buffers);
                //    _buffers = nullptr;
                //    _buffer_size = 0;
                //    _channel_size = 0;
                //    return __fst::status_code::not_enough_memory;
                //}

                //_buffers = buffers;
                //_buffer_size = __buffer_size;
                //_channel_size = __channel_size;

                //if (clear_memory) { clear_unchecked(); }

                //return __fst::status_code::success;
            }

            //
            _buffers = (_T**) allocate_audio_buffer(__channel_size, __buffer_size, sizeof(_T));

            if (!_buffers)
            {
                _buffer_size = 0;
                _channel_size = 0;
                return __fst::status_code::not_enough_memory;
            }

            _buffer_size = __buffer_size;
            _channel_size = __channel_size;

            if (clear_memory) { clear_unchecked(); }

            return __fst::status_code::success;
        }

      private:
        buffer_pointer _buffers = nullptr;
        size_type _buffer_size = 0;
        size_type _channel_size = 0;

        inline void clear_unchecked() noexcept
        {
            for (uint32_t i = 0; i < _channel_size; i++)
            {
                __fst::memset(channel(i), 0, _buffer_size * sizeof(value_type));
            }
        }

        void deallocate_audio_buffer(void* buffers) noexcept
        {
            if (buffers)
            {
                _MemoryZone::deallocate(buffers, _MemoryCategory::id());
            }
        }

        FST_NODISCARD void** allocate_audio_buffer(size_t channel_size, size_t buffer_size, size_t type_size) noexcept
        {
            fst_assert(buffer_size != 0 && channel_size != 0, "channel_size and buffer_size cannot be zero");
            if (!buffer_size || !channel_size) { return nullptr; }

            const size_t cache_size = __fst::mem_cache_size();

            const size_t buffer_ptr_size = channel_size * sizeof(void*);
            const size_t channel_buffer_size = buffer_size * type_size;

            const size_t total_buffer_size = channel_size * channel_buffer_size + channel_size * (cache_size - 1);
            const size_t total_size = buffer_ptr_size + total_buffer_size;

            void* bytes = _MemoryZone::allocate(total_size, _MemoryCategory::id());

            if (bytes == nullptr) { return nullptr; }

            void** buffers = (void**) bytes;

            void* raw_ptr = ((uint8_t*) bytes) + buffer_ptr_size;
            const uint8_t* raw_ptr_end = static_cast<uint8_t*>(raw_ptr) + total_buffer_size;

            size_t sp = total_buffer_size;

            for (size_t i = 0; i < channel_size; i++)
            {
                void* channel_ptr = __fst::align(cache_size, channel_buffer_size, raw_ptr, sp);

                fst_assert(channel_ptr != nullptr, "Can't align buffer with cache size.");

                if (channel_ptr == nullptr)
                {
                    deallocate_audio_buffer(buffers);
                    return nullptr;
                }

                buffers[i] = channel_ptr;

                raw_ptr = static_cast<uint8_t*>(channel_ptr) + channel_buffer_size;
                sp = (size_t)(raw_ptr_end - static_cast<uint8_t*>(static_cast<void*>(buffers[i])));
            }

            return buffers;
        }
    };
FST_END_NAMESPACE
//#include "au/memory.h"
//#include "au/status.h"
//#include "au/audio_wire.h"
//
//AU_BEGIN_NAMESPACE
//
/////
/////
/////
//template <typename T, _AU::memory_section _Section = _AU::memory_section::audio_buffer>
//class audio_buffer
//{
//  public:
//    using value_type = T;
//    using size_type = uint32_t;
//    using channel_pointer = value_type*;
//    using channel_const_pointer = const value_type*;
//    using buffer_pointer = value_type* const*;
//    using buffer_const_pointer = const value_type* const*;
//
//    static_assert(std::is_floating_point<value_type>::value, "au::audio_buffer only works with floating point value type.");
//
//    audio_buffer() noexcept = default;
//
//    inline audio_buffer(const audio_buffer& d) noexcept;
//    inline audio_buffer(const audio_buffer& d, _AU::status_code& sc) noexcept;
//
//    inline audio_buffer(size_type __channel_size, size_type __buffer_size, bool clear_memory = true) noexcept;
//
//    inline audio_buffer(audio_buffer&& d) noexcept
//        : _buffers(d._buffers)
//        , _buffer_size(d._buffer_size)
//        , _channel_size(d._channel_size)
//    {
//        d._buffers = nullptr;
//        d._buffer_size = 0;
//        d._channel_size = 0;
//    }
//
//    inline ~audio_buffer() noexcept { reset(); }
//
//    inline audio_buffer& operator=(const audio_buffer& d) noexcept
//    {
//        _AU::status_code sc = resize(d.channel_size(), d.buffer_size());
//        if (sc != _AU::status_code::success)
//        {
//            au_error("Couldn't resize buffer");
//            return *this;
//        }
//
//        for (uint32_t i = 0; i < _channel_size; i++)
//        {
//            au_memcpy(channel(i), d.channel(i), _buffer_size * sizeof(T));
//        }
//
//        return *this;
//    }
//
//    inline audio_buffer& operator=(audio_buffer&& d) noexcept
//    {
//        reset();
//        _buffers = d._buffers;
//        _buffer_size = d._buffer_size;
//        _channel_size = d._channel_size;
//        d._buffers = nullptr;
//        d._buffer_size = 0;
//        d._channel_size = 0;
//        return *this;
//    }
//
//    ///
//    FST_NODISCARD inline bool valid() const noexcept { return (bool) _buffers; }
//
//    ///
//    /// {@
//    FST_NODISCARD inline size_type channel_size() const noexcept { return _channel_size; }
//    FST_NODISCARD inline size_type buffer_size() const noexcept { return _buffer_size; }
//
//    ///
//    /// {@
//    FST_NODISCARD inline buffer_pointer data() noexcept { return _buffers; }
//    FST_NODISCARD inline buffer_const_pointer data() const noexcept { return _buffers; }
//    /// @}
//
//    ///
//    /// {@
//    FST_NODISCARD inline channel_pointer channel(size_type index) noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return _buffers[index];
//    }
//
//    FST_NODISCARD inline channel_const_pointer channel(size_type index) const noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return _buffers[index];
//    }
//    /// @}
//
//    ///
//    /// {@
//    FST_NODISCARD inline channel_pointer operator[](size_type index) noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return _buffers[index];
//    }
//
//    FST_NODISCARD inline channel_const_pointer operator[](size_type index) const noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return _buffers[index];
//    }
//    /// @}
//
//    FST_NODISCARD inline _AU::audio_wire<value_type> wire(size_type index) noexcept { return _AU::audio_wire<value_type>(_buffers[index], _buffer_size); }
//
//    FST_NODISCARD inline _AU::audio_wire<const value_type> wire(size_type index) const noexcept
//    {
//        return _AU::audio_wire<const value_type>(_buffers[index], _buffer_size);
//    }
//
//    ///
//    inline void clear() noexcept
//    {
//        if (_buffers) { clear_unchecked(); }
//    }
//
//    ///
//    inline void reset() noexcept;
//
//    ///
//    FST_NODISCARD inline _AU::status_code resize(size_type __channel_size, size_type __buffer_size, bool clear_memory = true) noexcept;
//
//  private:
//    buffer_pointer _buffers = nullptr;
//    size_type _buffer_size = 0;
//    size_type _channel_size = 0;
//
//    inline void clear_unchecked() noexcept
//    {
//        for (uint32_t i = 0; i < _channel_size; i++)
//        {
//            au_memset(channel(i), 0, _buffer_size * sizeof(T));
//        }
//    }
//};
//
/////
/////
/////
//template <typename T>
//class interleaved_audio_buffer
//{
//  public:
//    using value_type = T;
//    using size_type = size_t;
//    using reference = value_type&;
//    using const_reference = const value_type&;
//    using pointer = value_type*;
//    using const_pointer = const value_type*;
//    using buffer_pointer = value_type* const*;
//    using buffer_const_pointer = const value_type* const*;
//
//    static_assert(std::is_floating_point<value_type>::value, "au::interleaved_audio_buffer only works with floating point value type.");
//
//    interleaved_audio_buffer() noexcept = default;
//
//    inline interleaved_audio_buffer(size_type __channel_size, size_type __buffer_size, bool clear_memory = true) noexcept;
//
//    inline interleaved_audio_buffer(const interleaved_audio_buffer& d) noexcept;
//    inline interleaved_audio_buffer(const interleaved_audio_buffer& d, _AU::status_code& sc) noexcept;
//
//    inline interleaved_audio_buffer(interleaved_audio_buffer&& d) noexcept
//        : _buffers(d._buffers)
//        , _buffer_size(d._buffer_size)
//        , _channel_size(d._channel_size)
//    {
//        d._buffers = nullptr;
//        d._buffer_size = 0;
//        d._channel_size = 0;
//    }
//
//    inline ~interleaved_audio_buffer() noexcept { reset(); }
//
//    inline interleaved_audio_buffer& operator=(const interleaved_audio_buffer& d) noexcept
//    {
//
//        if (_AU::status_code sc = resize(d.channel_size(), d.buffer_size()); sc != _AU::status_code::success)
//        {
//            au_error("Couldn't resize buffer");
//            return *this;
//        }
//
//        au_memcpy(data(), d.data(), _channel_size * _buffer_size * sizeof(T));
//        return *this;
//    }
//
//    inline interleaved_audio_buffer& operator=(interleaved_audio_buffer&& d) noexcept
//    {
//        reset();
//        _buffers = d._buffers;
//        _buffer_size = d._buffer_size;
//        _channel_size = d._channel_size;
//        d._buffers = nullptr;
//        d._buffer_size = 0;
//        d._channel_size = 0;
//        return *this;
//    }
//
//    FST_NODISCARD inline bool valid() const noexcept { return (bool) _buffers; }
//
//    FST_NODISCARD inline size_type channel_size() const noexcept { return _channel_size; }
//    FST_NODISCARD inline size_type buffer_size() const noexcept { return _buffer_size; }
//
//    FST_NODISCARD inline pointer data() noexcept { return *_buffers; }
//    FST_NODISCARD inline const_pointer data() const noexcept { return *_buffers; }
//
//    struct channel_view
//    {
//        interleaved_audio_buffer& buffer;
//        size_t channel_index;
//
//        FST_NODISCARD inline value_type& operator[](size_t index) noexcept { return buffer.data()[buffer.channel_size() * index + channel_index]; }
//
//        FST_NODISCARD inline value_type operator[](size_t index) const noexcept { return buffer.data()[buffer.channel_size() * index + channel_index]; }
//    };
//
//    FST_NODISCARD inline channel_view channel(size_type index) noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return channel_view{ *this, index };
//    }
//
//    FST_NODISCARD inline channel_view channel(size_type index) const noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return channel_view{ *this, index };
//    }
//
//    FST_NODISCARD inline channel_view operator[](size_type index) noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return channel_view{ *this, index };
//    }
//
//    FST_NODISCARD inline channel_view operator[](size_type index) const noexcept
//    {
//        fst_assert(index < _channel_size, "Out of bounds index");
//        return channel_view{ *this, index };
//    }
//
//    ///
//    inline void clear() noexcept
//    {
//        if (_buffers) { clear_unchecked(); }
//    }
//
//    ///
//    inline void reset() noexcept;
//
//    ///
//    FST_NODISCARD inline _AU::status_code resize(size_type __channel_size, size_type __buffer_size, bool clear_memory = true) noexcept;
//
//  private:
//    buffer_pointer _buffers = nullptr;
//    size_type _buffer_size = 0;
//    size_type _channel_size = 0;
//
//    inline void clear_unchecked() noexcept { au_memset(data(), 0, _channel_size * _buffer_size * sizeof(T)); }
//};
//
////-------------------------------------------------------------------------
//// Implementation
////-------------------------------------------------------------------------
//
/////
//extern FST_NODISCARD void** allocate_audio_buffer(size_t channel_size, size_t buffer_size, size_t type_size, _AU::memory_section mem_section) noexcept;
//
/////
//extern FST_NODISCARD void** reallocate_audio_buffer(void* ptr, size_t channel_size, size_t buffer_size, size_t type_size, _AU::memory_section mem_section) noexcept;
//
/////
//extern FST_NODISCARD void** allocate_interleaved_audio_buffer(size_t channel_size, size_t buffer_size, size_t type_size) noexcept;
//
/////
//extern FST_NODISCARD void** reallocate_interleaved_audio_buffer(void* ptr, size_t channel_size, size_t buffer_size, size_t type_size) noexcept;
//
/////
//extern void deallocate_audio_buffer(void* buffers, _AU::memory_section mem_section) noexcept;
//
//
//
////
////-------------------------------------------------------------------------
////
//
//template <class T>
//inline interleaved_audio_buffer<T>::interleaved_audio_buffer(size_type __channel_size, size_type __buffer_size, bool clear_memory) noexcept
//{
//
//    _buffers = (T**) _AU::allocate_interleaved_audio_buffer(__channel_size, __buffer_size, sizeof(T));
//
//    if (_buffers)
//    {
//        _buffer_size = __buffer_size;
//        _channel_size = __channel_size;
//
//        if (clear_memory) { clear_unchecked(); }
//    }
//}
//
//template <class T>
//inline interleaved_audio_buffer<T>::interleaved_audio_buffer(const interleaved_audio_buffer& d) noexcept
//{
//    _buffers = (T**) _AU::allocate_interleaved_audio_buffer(d.channel_size(), d.buffer_size(), sizeof(T));
//
//    if (_buffers)
//    {
//        _buffer_size = d.buffer_size();
//        _channel_size = d.channel_size();
//
//        au_memcpy(data(), d.data(), _channel_size * _buffer_size * sizeof(T));
//    }
//}
//
//template <class T>
//inline interleaved_audio_buffer<T>::interleaved_audio_buffer(const interleaved_audio_buffer& d, _AU::status_code& sc) noexcept
//{
//    _buffers = (T**) _AU::allocate_interleaved_audio_buffer(d.channel_size(), d.buffer_size(), sizeof(T));
//
//    if (!_buffers)
//    {
//        sc = _AU::status_code::not_enough_memory;
//        return;
//    }
//
//    _buffer_size = d.buffer_size();
//    _channel_size = d.channel_size();
//
//    au_memcpy(data(), d.data(), _channel_size * _buffer_size * sizeof(T));
//    sc = _AU::status_code::success;
//}
//
//template <class T>
//inline void interleaved_audio_buffer<T>::reset() noexcept
//{
//    _AU::deallocate_audio_buffer((void*) _buffers, _AU::memory_section::audio_buffer);
//    _buffers = nullptr;
//    _buffer_size = 0;
//    _channel_size = 0;
//}
//
//template <class T>
//inline _AU::status_code interleaved_audio_buffer<T>::resize(size_type __channel_size, size_type __buffer_size, bool clear_memory) noexcept
//{
//
//    fst_assert(__channel_size != 0 && __buffer_size != 0, "channel_size and buffer_size cannot be zero");
//
//    if (_buffers)
//    {
//        // Nothing to do if same as current channel and buffer size.
//        if (_buffer_size == __buffer_size && _channel_size == __channel_size)
//        {
//            if (clear_memory) { clear_unchecked(); }
//            return _AU::status_code::success;
//        }
//
//        T** buffers = (T**) _AU::reallocate_interleaved_audio_buffer((void*) _buffers, __channel_size, __buffer_size, sizeof(T));
//        if (!buffers)
//        {
//            _AU::deallocate_audio_buffer((void*) _buffers, _AU::memory_section::audio_buffer);
//            _buffers = nullptr;
//            _buffer_size = 0;
//            _channel_size = 0;
//            return _AU::status_code::not_enough_memory;
//        }
//
//        _buffers = buffers;
//        _buffer_size = __buffer_size;
//        _channel_size = __channel_size;
//
//        if (clear_memory) { clear_unchecked(); }
//
//        return _AU::status_code::success;
//    }
//
//    //
//    _buffers = (T**) _AU::allocate_interleaved_audio_buffer(__channel_size, __buffer_size, sizeof(T));
//
//    if (!_buffers)
//    {
//        _buffer_size = 0;
//        _channel_size = 0;
//        return _AU::status_code::not_enough_memory;
//    }
//
//    _buffer_size = __buffer_size;
//    _channel_size = __channel_size;
//
//    if (clear_memory) { clear_unchecked(); }
//
//    return _AU::status_code::success;
//}
//AU_END_NAMESPACE
