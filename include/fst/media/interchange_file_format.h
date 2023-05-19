#pragma once

#include "fst/common.h"
#include "fst/array.h"
#include "fst/media/audio_format.h"

FST_BEGIN_NAMESPACE

    ///
    union iff_four_cc
    {
        static constexpr int32_t size = 4;

        char bytes[4];
        uint32_t integer;

        iff_four_cc() = default;

        inline constexpr iff_four_cc(const char* buffer)
            : bytes{ buffer[0], buffer[1], buffer[2], buffer[3] }
        {}

        inline constexpr iff_four_cc(const uint8_t* buffer)
            : bytes{ (char) buffer[0], (char) buffer[1], (char) buffer[2], (char) buffer[3] }
        {}

        inline constexpr iff_four_cc(uint32_t v)
            : integer{ v }
        {}

        inline constexpr bool operator==(iff_four_cc h) const { return h.integer == integer; }

        inline constexpr bool operator==(const char* buffer) const { return iff_four_cc(buffer).integer == integer; }

        inline constexpr bool operator!=(const char* buffer) const { return !operator==(buffer); }

        // friend inline std::ostream& operator<<(std::ostream& s, iff_four_cc h)
        // {
        //     return s << h.bytes[0] << h.bytes[1] << h.bytes[2] << h.bytes[3];
        // }
    };

    static_assert(sizeof(iff_four_cc) == 4, "wrong size");

    using riff_four_cc = iff_four_cc;

    struct iff_header
    {
        iff_four_cc id;
        int32_t size;

        inline int32_t total_size() const noexcept { return 8 + size; }
    };

    static_assert(sizeof(iff_header) == 8, "wrong size");

    struct riff_header
    {
        iff_four_cc id;
        int32_t size;

        inline int32_t total_size() const noexcept { return 8 + size; }
    };

    static_assert(sizeof(riff_header) == 8, "wrong size");

    struct iff_header_info
    {
        size_t index;
        iff_four_cc id;
        size_t size;
    };

    struct riff_header_info
    {
        size_t index;
        iff_four_cc id;
        size_t size;
    };

    struct riff_header_list
    {
        inline riff_header_list(const uint8_t* data, size_t dataSize, size_t startIndex) noexcept
        {
            for (size_t i = startIndex; i < dataSize - 4;)
            {
                const size_t size = (size_t) * (const int32_t*) (data + i + 4);
                _headers[_count++] = { i, iff_four_cc(data + i), size };
                i += 8 + size;
            }
        }

        inline constexpr const riff_header_info* find(iff_four_cc id) const noexcept
        {
            for (size_t i = 0; i < _count; i++)
            {
                if (_headers[i].id == id) { return &_headers[i]; }
            }

            return nullptr;
        }

      private:
        riff_header_info _headers[16];
        size_t _count = 0;
    };

    struct iff_header_list
    {
        inline iff_header_list(const uint8_t* data, size_t dataSize, size_t startIndex) noexcept
        {
            for (size_t i = startIndex; i < dataSize - 4;)
            {
                const size_t size = (size_t) __fst::byte_swap(*(const int32_t*) (data + i + 4));
                _headers[_count++] = { i, iff_four_cc(data + i), size };
                i += 8 + size;
            }
        }

        inline constexpr const iff_header_info* find(iff_four_cc id) const noexcept
        {
            for (size_t i = 0; i < _count; i++)
            {
                if (_headers[i].id == id) { return &_headers[i]; }
            }

            return nullptr;
        }

      private:
        iff_header_info _headers[16];
        size_t _count = 0;
    };

    //
    // WAV
    //

    enum class wav_audio_format : int16_t {
        pcm = 0x0001,
        ieee = 0x0003,
        ALaw = 0x0006,
        MULaw = 0x0007,
        extensible = -2 //0xFFFE
    };

    inline constexpr bool is_valid_wav_audio_format(audio_format_type f)
    {

        FST_PRAGMA_PUSH()
        FST_PRAGMA_DISABLE_WARNING_MSVC(4062)
        switch (f)
        {
        case audio_format_type::pcm_8s:
        case audio_format_type::pcm_16s:
        case audio_format_type::pcm_24s:
        case audio_format_type::pcm_32s:
        case audio_format_type::ieee_32:
        case audio_format_type::ieee_64: return true;
        }
        FST_PRAGMA_POP()

        return false;
    }

    struct wav_format_header
    {
        wav_audio_format audio_format;
        int16_t channel_size;
        uint32_t sample_rate;
        uint32_t bytes_per_second;
        int16_t bytes_per_block;
        int16_t bit_depth;

        static inline wav_format_header create(int16_t channel_size, uint32_t sampling_rate, __fst::audio_format_type ft)
        {
            const int16_t bit_depth = (int16_t) __fst::audio_format_to_bit_depth(ft);
            const int16_t bytes_per_block = channel_size * bit_depth / 8;

            return wav_format_header{ __fst::is_floating_point_audio_format(ft) ? wav_audio_format::ieee : wav_audio_format::pcm, channel_size, sampling_rate,
                (uint32_t) bytes_per_block * sampling_rate, bytes_per_block, bit_depth };
        }

        inline int16_t sample_byte_size() const noexcept { return bit_depth / 8; }

        inline bool is_standard_layout() const noexcept
        {
            return (bytes_per_second == (channel_size * sample_byte_size() * sample_rate)) && (bytes_per_block == (channel_size * sample_byte_size()));
        }
    };

    static_assert(sizeof(wav_format_header) == 16, "wrong size");

    //
    // AIFF
    //

    enum class aiff_audio_format {
        uncompressed,
        compressed,
        unknown
    };

#pragma pack(1)
    struct aiff_common_chunk
    {
        int16_t numChannels;
        uint32_t numSampleFrames;
        int16_t sampleSize;
        __fst::array<uint8_t, 10> sampleRate;
    };
#pragma pack()

    static_assert(sizeof(aiff_common_chunk) == 18, "wrong size");

    struct aiff_sound_data_chunk
    {
        uint32_t offset;
        uint32_t blockSize;
    };

    static_assert(sizeof(aiff_sound_data_chunk) == 8, "wrong size");

FST_END_NAMESPACE
