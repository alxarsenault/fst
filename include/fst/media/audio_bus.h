#pragma once

#include "fst/common.h"
#include "fst/media/audio_buffer.h"

FST_BEGIN_NAMESPACE
//
template <typename T>
class audio_bus
{
  public:
    using element_type = T;
    using value_type = __fst::remove_cv_t<T>;
    using size_type = uint32_t;
    using channel_pointer = element_type*;
    using channel_const_pointer = const element_type*;
    using buffer_pointer = element_type* const*;
    using buffer_const_pointer = const element_type* const*;

    static_assert(__fst::is_floating_point_v<value_type>, "au::audio_bus only works with floating point value type.");

    constexpr audio_bus() noexcept = default;
    constexpr audio_bus(const audio_bus&) noexcept = default;
    constexpr audio_bus(audio_bus&&) noexcept = default;

    inline constexpr audio_bus(buffer_pointer buffers, size_type chan_size, size_type buf_size) noexcept
        : _buffers(buffers)
        , _buffer_size(buf_size)
        , _channel_size(chan_size)
    {}

    template <class U, __fst::enable_if_t<__fst::is_buffer_convertible_v<T, U>, int> = 0>
    inline constexpr audio_bus(const audio_bus<U>& b) noexcept
        : _buffers{ b.data() }
        , _buffer_size(b.buffer_size())
        , _channel_size(b.channel_size())
    {}

    template <class U, __fst::enable_if_t<__fst::is_buffer_convertible_v<T, U>, int> = 0>
    inline constexpr audio_bus(audio_buffer<U>& b) noexcept
        : _buffers{ b.data() }
        , _buffer_size(b.buffer_size())
        , _channel_size(b.channel_size())
    {}

    template <class U, __fst::enable_if_t<__fst::is_buffer_convertible_v<T, U>, int> = 0>
    inline constexpr audio_bus(const audio_buffer<U>& b) noexcept
        : _buffers{ b.data() }
        , _buffer_size(b.buffer_size())
        , _channel_size(b.channel_size())
    {}

    ~audio_bus() noexcept = default;

    audio_bus& operator=(const audio_bus&) noexcept = default;
    audio_bus& operator=(audio_bus&&) noexcept = default;

    FST_NODISCARD inline bool valid() const noexcept { return (bool) _buffers; }

    FST_NODISCARD inline size_type channel_size() const noexcept { return _channel_size; }
    FST_NODISCARD inline size_type buffer_size() const noexcept { return _buffer_size; }

    FST_NODISCARD inline buffer_pointer data() noexcept { return _buffers; }
    FST_NODISCARD inline buffer_const_pointer data() const noexcept { return _buffers; }

    FST_NODISCARD inline channel_pointer channel(size_type index) noexcept { return _buffers[index]; }
    FST_NODISCARD inline channel_const_pointer channel(size_type index) const noexcept { return _buffers[index]; }

    FST_NODISCARD inline channel_pointer operator[](size_type index) noexcept { return _buffers[index]; }
    FST_NODISCARD inline channel_const_pointer operator[](size_type index) const noexcept { return _buffers[index]; }

    // FST_NODISCARD inline _AU::audio_wire<T> wire(size_type index) noexcept { return _AU::audio_wire<T>(_buffers[index], _buffer_size); }

    // FST_NODISCARD inline _AU::audio_wire<const value_type> wire(size_type index) const noexcept
    // {
        // return _AU::audio_wire<const value_type>(_buffers[index], _buffer_size);
    // }

  private:
    buffer_pointer _buffers = nullptr;
    size_type _buffer_size = 0;
    size_type _channel_size = 0;
};
//
//template <typename T>
//class interleaved_audio_bus
//{
//  public:
//    using element_type = T;
//    using value_type = __fst::remove_cv_t<T>;
//    using size_type = size_t;
//    using reference = element_type&;
//    using const_reference = const element_type&;
//    using pointer = element_type*;
//    using const_pointer = const element_type*;
//    using buffer_pointer = element_type* const*;
//    using buffer_const_pointer = const element_type* const*;
//
//    static_assert(__fst::is_floating_point_v<value_type>, "au::interleaved_audio_bus only works with floating point value type.");
//
//    constexpr interleaved_audio_bus() noexcept = default;
//    constexpr interleaved_audio_bus(const interleaved_audio_bus&) noexcept = default;
//    constexpr interleaved_audio_bus(interleaved_audio_bus&&) noexcept = default;
//
//    inline constexpr interleaved_audio_bus(pointer __buffer, size_type __channel_size, size_type __buffer_size) noexcept
//        : _buffer(__buffer)
//        , _buffer_size(__buffer_size)
//        , _channel_size(__channel_size)
//    {}
//
//    template <class U, _AU::enable_if_buffer_convertible_t<T, U> = nullptr>
//    inline constexpr interleaved_audio_bus(const interleaved_audio_bus<U>& b) noexcept
//        : _buffer{ b.data() }
//        , _buffer_size(b.buffer_size())
//        , _channel_size(b.channel_size())
//    {}
//
//    template <class U, _AU::enable_if_buffer_convertible_t<T, U> = nullptr>
//    inline constexpr interleaved_audio_bus(interleaved_audio_buffer<U>& b) noexcept
//        : _buffer{ b.data() }
//        , _buffer_size(b.buffer_size())
//        , _channel_size(b.channel_size())
//    {}
//
//    template <class U, _AU::enable_if_buffer_convertible_t<T, U> = nullptr>
//    inline constexpr interleaved_audio_bus(const interleaved_audio_buffer<U>& b) noexcept
//        : _buffer{ b.data() }
//        , _buffer_size(b.buffer_size())
//        , _channel_size(b.channel_size())
//    {}
//
//    ~interleaved_audio_bus() noexcept = default;
//
//    interleaved_audio_bus& operator=(const interleaved_audio_bus&) noexcept = default;
//    interleaved_audio_bus& operator=(interleaved_audio_bus&&) noexcept = default;
//
//    FST_NODISCARD inline bool valid() const noexcept { return (bool) _buffer; }
//
//    FST_NODISCARD inline size_type channel_size() const noexcept { return _channel_size; }
//    FST_NODISCARD inline size_type buffer_size() const noexcept { return _buffer_size; }
//
//    FST_NODISCARD inline pointer data() noexcept { return _buffer; }
//    FST_NODISCARD inline const_pointer data() const noexcept { return _buffer; }
//
//    FST_NODISCARD inline reference operator[](size_type index) noexcept { return data()[index]; }
//    FST_NODISCARD inline const_reference operator[](size_type index) const noexcept { return data()[index]; }
//
//  private:
//    pointer _buffer = nullptr;
//    size_type _buffer_size = 0;
//    size_type _channel_size = 0;
//};
FST_END_NAMESPACE
