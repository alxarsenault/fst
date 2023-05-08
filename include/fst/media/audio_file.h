#pragma once

#include "fst/common.h"
#include "fst/media/audio_buffer.h"
#include "fst/media/audio_format.h"
//#include "fst/file.h"
#include "fst/status_code.h"
#include "fst/file_view.h"
#include "fst/media/interchange_file_format.h"

FST_BEGIN_NAMESPACE

    enum class audio_file_type {
        unknown,
        wav,
        aiff
    };

    ///
    template <class AudioBufferType>
    inline __fst::status decode_wave_data(
        const uint8_t* file_data, size_t file_size, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept;

    ///
    template <class AudioBufferType>
    inline __fst::status decode_aiff_data(
        const uint8_t* file_data, size_t file_size, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept;

    ///
    template <class AudioBufferType>
    inline __fst::status decode_audio_data(
        const uint8_t* file_data, size_t file_size, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept;

    ///
    template <class AudioBufferType>
    inline __fst::status read_wave_file(const char* filepath, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept;

    ///
    template <class AudioBufferType>
    inline __fst::status read_aiff_file(const char* filepath, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept;

    ///
    template <class AudioBufferType>
    inline __fst::status read_audio_file(const char* filepath, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept;

    ///
    template <class AudioBufferType>
    inline __fst::status write_wave_data(
        __fst::vector<uint8_t> & data, const AudioBufferType& input_buffer, uint32_t sampling_rate, __fst::audio_format_type format) noexcept;

    //template <class AudioBufferType>
    //inline __fst::status write_wave_file(const char* filepath, const AudioBufferType& input_buffer, uint32_t sampling_rate, __fst::audio_format_type format) noexcept
    //{
    //__fst::vector<uint8_t> data;
    //if (__fst::status st = __fst::write_wave_data(data, input_buffer, sampling_rate, format)) { return st; }

    //return data.write_to_file(filepath);
    //}

    //
    //
    //

    template <class AudioBufferType>
    inline __fst::status decode_wave_data(
        const uint8_t* file_data, size_t file_size, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& ft) noexcept
    {
        using T = typename AudioBufferType::value_type;
        constexpr __fst::audio_format_type dst_format_type = __fst::to_audio_format_v<T>;

        // HEADER CHUNK
        if (__fst::riff_four_cc(file_data) != "RIFF") { return __fst::status_code::invalid_file_format; }

        if (__fst::riff_four_cc(file_data + 8) != "WAVE") { return __fst::status_code::invalid_file_format; }

        // -----------------------------------------------------------
        // try and find the start points of key chunks
        const __fst::riff_header_list headers(file_data, file_size, 12);

        // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
        // then it is unlikely we'll able to read this file, so abort
        const __fst::riff_header_info* dataChunkInfo = headers.find("data");
        if (!dataChunkInfo) { return __fst::status_code::invalid_file_content; }

        const __fst::riff_header_info* formatChunkInfo = headers.find("fmt ");
        if (!formatChunkInfo) { return __fst::status_code::invalid_file_content; }

        // -----------------------------------------------------------
        // FORMAT CHUNK
        //au_assert(formatChunkInfo->size == sizeof(__fst::wav_format_header));

        const __fst::wav_format_header* format = (const __fst::wav_format_header*) (file_data + formatChunkInfo->index + 8);
        const uint16_t sample_byte_size = (uint16_t) format->sample_byte_size();

        // check that the audio format is PCM or Float or extensible
        if (!__fst::is_one_of(format->audio_format, wav_audio_format::pcm, wav_audio_format::ieee, wav_audio_format::extensible))
        {
            return __fst::status_code::invalid_audio_format;
        }

        // check the number of channels is mono or stereo
        if (format->channel_size < 1 || format->channel_size > 128) { return __fst::status_code::invalid_channel_size; }

        // Check header data is consistent.
        if (!format->is_standard_layout()) { return __fst::status_code::invalid_audio_format; }

        // Check bit depth.
        if (!__fst::is_one_of(format->bit_depth, 8, 16, 24, 32, 64)) { return __fst::status_code::invalid_audio_format; }

        // -----------------------------------------------------------
        // DATA CHUNK
        sampling_rate = format->sample_rate;

        const size_t buffer_size = dataChunkInfo->size / ((size_t) format->channel_size * (size_t) sample_byte_size);
        const uint8_t* sample_data = file_data + dataChunkInfo->index + 8;

        __fst::status st = __fst::status_code::unknown;

#define CONVERT_BUFFER(FMT)                                               \
    convert_audio_buffer<dst_format_type, __fst::audio_format_type::FMT>( \
        sample_data, output_buffer, buffer_size, (size_t) format->channel_size, (size_t) format->bytes_per_block, (size_t) sample_byte_size)

        switch (format->bit_depth)
        {
        case 8:
            ft = __fst::audio_format_type::pcm_8s;
            st = CONVERT_BUFFER(pcm_8u);
            break;
        case 16:
            ft = __fst::audio_format_type::pcm_16s;
            st = CONVERT_BUFFER(pcm_16s);
            break;
        //case 24:
            //ft = __fst::audio_format_type::pcm_24s;
            //st = CONVERT_BUFFER(pcm_24s);
            //break;
        case 32:
            if (format->audio_format == __fst::wav_audio_format::ieee)
            {
                ft = __fst::audio_format_type::ieee_32;
                st = CONVERT_BUFFER(ieee_32);
            }
            else // assume PCM
            {
                ft = __fst::audio_format_type::pcm_32s;
                st = CONVERT_BUFFER(pcm_32s);
            }
            break;

        case 64:
            if (format->audio_format == __fst::wav_audio_format::ieee)
            {
                ft = __fst::audio_format_type::ieee_64;
                st = CONVERT_BUFFER(ieee_64);
            }
            break;
        }

#undef CONVERT_BUFFER

        return st;
    }

    template <class AudioBufferType>
    inline __fst::status read_wave_file(const char* filepath, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept
    {
        __fst::file_view file;
        __fst::status s = file.open(filepath);
        return !s ? s : __fst::decode_wave_data(file.data(), file.size(), output_buffer, sampling_rate, format);
    }

    inline double read_float80(const uint8_t* buffer)
    {
        FST_PRAGMA_PUSH()
        FST_PRAGMA_DISABLE_WARNING_MSVC(5219)
        struct ieee80
        {
            uint64_t m : 63;
            uint32_t k : 1;
            uint32_t e : 15;
            uint32_t s : 1;

            inline operator double() const { return ((1.0 + (double) m / ((uint64_t) 1 << 63)) * (((int64_t) 1 << (((int64_t) (e) *2) - (int64_t) 16383)))); }
        };

        //return *(ieee80*)buffer;
        ////double ddd = *(ieee80*)byteSwap(*(ieee80::Data*)buffer).data;
        ////ieee80 ie = *(ieee80*)byteSwap(*(ieee80::Data*)buffer).data;
        ////double ddd = ((1.0 + (double)ie.m / ((uint64_t)1 << 63)) * (((int64_t)1 << (((int64_t)(ie.e) << 1) - (int64_t)16383))));

        ////int64_t exdp = ((int64_t)1 << (((int64_t)(ie.e) << 1) - (int64_t)16383));
        ////double ddd = ((1.0 + (double)ie.m * dtwoPow63) * (((int64_t)1 << (((int64_t)(ie.e) << 1) - (int64_t)16383))));

        ////int64_t exdp = ((int64_t)1 << (((int64_t)(ie.e) * 2) - (int64_t)16383));

        ////80 bit floating point value according to the IEEE-754 specification and the Standard Apple Numeric Environment specification:
        ////1 bit sign, 15 bit exponent, 1 bit normalization indication, 63 bit mantissa

        const uint32_t exponent = (((uint32_t) buffer[0] & 0x7F) << 8) | (uint32_t) buffer[1];
        uint64_t mantissa = __fst::byte_swap(*(const uint64_t*) (buffer + 2)); //readBytesAsBigEndian<uint64_t>(buffer + 2);

        ////If the highest bit of the mantissa is set, then this is a normalized number.
        double normalizeCorrection = (mantissa & 0x8000000000000000) != 0;
        mantissa &= 0x7FFFFFFFFFFFFFFF;

        ////value = (-1) ^ s * (normalizeCorrection + m / 2 ^ 63) * 2 ^ (e - 16383)

        double sign = (buffer[0] & 0x80) ? -1 : 1;
        int64_t exp = (int64_t) (sign * ((int64_t) 1 << ((int64_t) exponent - (int64_t) 16383)));

        ////std::cout << mantissa << "mantissa " << ie->m << " kkkk " << ddd << " dslkdlskldskld " << ((normalizeCorrection + (double)mantissa / twoPow63) * exp) << std::endl;
        //////iee = byteSwap(iee);

        ////std::cout << exponent << " bb " << ie->e * 2 << std::endl;
        ////std::cout << exp << " bb " << exdp << std::endl;
        constexpr uint64_t twoPow63 = ((uint64_t) 1 << 63);

        //        std::cout << ddd << " dslkdlskldskld " << ((normalizeCorrection + (double)mantissa / twoPow63) * exp) << std::endl;

        return (normalizeCorrection + (double) mantissa / twoPow63) * exp; // std::pow(2, (int32_t)exponent - 16383);
        FST_PRAGMA_POP()
    }

    template <class AudioBufferType>
    inline __fst::status decode_aiff_data(
        const uint8_t* file_data, size_t file_size, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& ft) noexcept
    {
        using T = typename AudioBufferType::value_type;
        constexpr audio_format_type dst_format_type = __fst::to_audio_format_v<T>;

        // HEADER CHUNK
        if (__fst::iff_four_cc(file_data) != "FORM") { return __fst::status_code::invalid_file_format; }

        const __fst::iff_four_cc format(*(const uint32_t*) (file_data + 8));

        if (!__fst::is_one_of(format, "AIFF", "AIFC")) { return __fst::status_code::invalid_file_format; }

        aiff_audio_format audioFormat = format == "AIFF" ? aiff_audio_format::uncompressed : aiff_audio_format::compressed;

        // -----------------------------------------------------------
        // try and find the start points of key chunks
        const __fst::iff_header_list headers(file_data, file_size, 12);

        const __fst::iff_header_info* commChunkInfo = headers.find("COMM");
        if (!commChunkInfo) { return __fst::status_code::invalid_file_format; }

        const __fst::iff_header_info* soundChunkInfo = headers.find("SSND");
        if (!soundChunkInfo) { return __fst::status_code::invalid_file_format; }

        // -----------------------------------------------------------
        // COMM CHUNK
        /*struct Extended
   {
       uint8_t data[10];
   };*/

        __fst::aiff_common_chunk cc = *(const __fst::aiff_common_chunk*) (file_data + commChunkInfo->index + 8);
        cc.numChannels = __fst::byte_swap(cc.numChannels);
        cc.numSampleFrames = __fst::byte_swap(cc.numSampleFrames);
        cc.sampleSize = __fst::byte_swap(cc.sampleSize);
        //cc.sample_rate = byteSwap(cc.sample_rate);
        //int ggg = getAiffSampleRate(file_data + commChunkInfo->index + 16, 0);
        int bitDepth = cc.sampleSize;

        sampling_rate = (uint32_t) read_float80(cc.sampleRate.data());

        // check the sample rate was properly decoded
        if (sampling_rate == 0) { return __fst::status_code::unknown; }

        // check the number of channels is mono or stereo
        if (cc.numChannels < 1 || cc.numChannels > 2) { return __fst::status_code::invalid_channel_size; }

        // check bit depth is either 8, 16, 24 or 32-bit
        if (!__fst::is_one_of(bitDepth, 8, 16, 24, 32)) { return __fst::status_code::invalid_audio_format; }

        // -----------------------------------------------------------
        // SSND CHUNK
        __fst::aiff_sound_data_chunk sndChunk = *(const __fst::aiff_sound_data_chunk*) (file_data + soundChunkInfo->index + 8);
        sndChunk.offset = __fst::byte_swap(sndChunk.offset);

        const uint16_t numBytesPerSample = static_cast<uint16_t>(bitDepth) / 8;
        const size_t numBytesPerFrame = (size_t) numBytesPerSample * (size_t) cc.numChannels;
        const size_t totalNumAudioSampleBytes = cc.numSampleFrames * numBytesPerFrame;
        const size_t samplesStartIndex = soundChunkInfo->index + 16 + sndChunk.offset;

        // sanity check the data
        if ((soundChunkInfo->size - 8) != totalNumAudioSampleBytes || totalNumAudioSampleBytes > static_cast<long>(file_size - samplesStartIndex))
        {
            return __fst::status_code::unknown;
        }

        const uint8_t* sampleData = file_data + samplesStartIndex;
        __fst::status st = __fst::status_code::unknown;

        switch (bitDepth)
        {
        case 8:
            ft = audio_format_type::pcm_8s;
            st = convert_audio_buffer<dst_format_type, audio_format_type::pcm_8s_be>(
                sampleData, output_buffer, cc.numSampleFrames, cc.numChannels, numBytesPerFrame, numBytesPerSample);
            break;
        case 16:
            ft = audio_format_type::pcm_16s;
            st = convert_audio_buffer<dst_format_type, audio_format_type::pcm_16s_be>(
                sampleData, output_buffer, cc.numSampleFrames, cc.numChannels, numBytesPerFrame, numBytesPerSample);
            break;
        case 24:
            ft = audio_format_type::pcm_24s;
            st = convert_audio_buffer<dst_format_type, audio_format_type::pcm_24s_be>(
                sampleData, output_buffer, cc.numSampleFrames, cc.numChannels, numBytesPerFrame, numBytesPerSample);
            break;
        case 32:
            if (audioFormat == aiff_audio_format::compressed)
            {
                ft = audio_format_type::ieee_32;
                st = convert_audio_buffer<dst_format_type, audio_format_type::ieee_32_be>(
                    sampleData, output_buffer, cc.numSampleFrames, cc.numChannels, numBytesPerFrame, numBytesPerSample);
            }
            else // assume PCM
            {
                ft = audio_format_type::pcm_32s;
                st = convert_audio_buffer<dst_format_type, audio_format_type::pcm_32s_be>(
                    sampleData, output_buffer, cc.numSampleFrames, cc.numChannels, numBytesPerFrame, numBytesPerSample);
            }
            break;
        }

        return st;
    }

    template <class AudioBufferType>
    inline __fst::status read_aiff_file(const char* filepath, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept
    {
        __fst::file_view file;
        __fst::status ec = file.open(filepath);
        return ec ? ec : __fst::decode_aiff_data(file.data(), file.size(), output_buffer, sampling_rate, format);
    }

    template <class AudioBufferType>
    inline __fst::status decode_audio_data(
        const uint8_t* file_data, size_t file_size, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept
    {
        // Get audio file format.
        const size_t headerChunkSize = __fst::minimum(file_size, (size_t) 4);
        if (__fst::strncmp((const char*) file_data, "RIFF", headerChunkSize) == 0)
        {
            return __fst::decode_wave_data(file_data, file_size, output_buffer, sampling_rate, format);
        }
        else if (__fst::strncmp((const char*) file_data, "FORM", headerChunkSize) == 0)
        {
            return __fst::decode_aiff_data(file_data, file_size, output_buffer, sampling_rate, format);
        }

        return __fst::status_code::invalid_file_format;
    }

    template <class AudioBufferType>
    inline __fst::status read_audio_file(const char* filepath, AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format) noexcept
    {
        __fst::file_view file;
        if (__fst::status ec = file.open(filepath)) { return ec; }

        return __fst::decode_audio_data(file.data(), file.size(), output_buffer, sampling_rate, format);
    }

    template <class AudioBufferType>
    inline __fst::status write_wave_data(__fst::vector<uint8_t> & data, const AudioBufferType& input_buffer, uint32_t sampling_rate, __fst::audio_format_type ft) noexcept
    {
        using value_type = typename AudioBufferType::value_type;
        constexpr audio_format_type src_format_type = __fst::to_audio_format_v<value_type>;

        if (!__fst::is_valid_wav_audio_format(ft)) { return __fst::status_code::invalid_audio_format; }

        const __fst::wav_format_header fmt_header = __fst::wav_format_header::create((int16_t) input_buffer.channel_size(), sampling_rate, ft);
        const __fst::riff_header fmt_chunk_header{ "fmt ", sizeof(__fst::wav_format_header) + (__fst::is_audio_format_pcm(ft) ? 0 : sizeof(int16_t)) };
        const __fst::riff_header data_chunk_header{ "data", (int32_t) input_buffer.buffer_size() * fmt_header.bytes_per_block };

        // 4 + (8 + SubChunk1Size) + ... + (8 + SubChunk2Size)
        // This is the size of the entire file in bytes minus 8 bytes for the two fields not included in this count :
        // ChunkID and ChunkSize.
        const __fst::riff_header riff_chunk_header{ "RIFF", __fst::iff_four_cc::size + fmt_chunk_header.total_size() + data_chunk_header.total_size() };

        //
        // Riff chunk.
        //
        data.push_back(riff_chunk_header);
        data.push_back("WAVE");

        //
        // Format chunk.
        //
        data.push_back(fmt_chunk_header);
        data.push_back(fmt_header);

        if (fmt_header.audio_format == __fst::wav_audio_format::ieee)
        {
            data.push_back<int16_t>(0); // extension size
        }

        //
        // Data chunk.
        //
        data.push_back(data_chunk_header);

        switch (ft)
        {
        case __fst::audio_format_type::pcm_8s: __fst::serialize_audio_buffer<__fst::audio_format_type::pcm_8s, src_format_type>(data, input_buffer); break;

        case __fst::audio_format_type::pcm_16s: __fst::serialize_audio_buffer<__fst::audio_format_type::pcm_16s, src_format_type>(data, input_buffer); break;

        case __fst::audio_format_type::pcm_24s: __fst::serialize_audio_buffer<__fst::audio_format_type::pcm_24s, src_format_type>(data, input_buffer); break;

        case __fst::audio_format_type::pcm_32s: __fst::serialize_audio_buffer<__fst::audio_format_type::pcm_32s, src_format_type>(data, input_buffer); break;

        case __fst::audio_format_type::ieee_32: __fst::serialize_audio_buffer<__fst::audio_format_type::ieee_32, src_format_type>(data, input_buffer); break;

        case __fst::audio_format_type::ieee_64: __fst::serialize_audio_buffer<__fst::audio_format_type::ieee_64, src_format_type>(data, input_buffer); break;
        }

        // Sanity check.
        if (riff_chunk_header.total_size() != (int32_t) data.size()) { return __fst::status_code::invalid_file_content; }

        return __fst::status();
    }
FST_END_NAMESPACE

//namespace raw {
//    namespace detail {
//        constexpr std::string_view header_id = "DKTR";
//
//        struct load_data {
//            std::uint32_t sampling_rate;
//            std::uint32_t channel_size;
//            std::uint32_t buffer_size;
//            std::size_t read_index;
//        };
//
//        template <typename _Tp>
//        load_error internal_load(fst::byte_view bv, load_data& l_data) {
//            using value_type = _Tp;
//
//            constexpr std::size_t minimum_file_size = header_id.size() + 3 * sizeof(std::uint32_t);
//
//            if (bv.size() < minimum_file_size) {
//                return load_error::invalid_file_format;
//            }
//
//            if (std::string_view(bv.data<char>(), header_id.size()) != header_id) {
//                return load_error::invalid_file_format;
//            }
//
//            std::size_t read_index = header_id.size();
//            l_data.sampling_rate = bv.as<std::uint32_t>(read_index);
//            read_index += sizeof(std::uint32_t);
//            l_data.channel_size = bv.as<std::uint32_t>(read_index);
//            read_index += sizeof(std::uint32_t);
//            l_data.buffer_size = bv.as<std::uint32_t>(read_index);
//            read_index += sizeof(std::uint32_t);
//
//            if (l_data.sampling_rate == 0) {
//                return load_error::invalid_sampling_rate;
//            }
//
//            if (l_data.channel_size == 0) {
//                return load_error::invalid_channel_size;
//            }
//
//            if (l_data.buffer_size == 0) {
//                return load_error::invalid_buffer_size;
//            }
//
//            if (l_data.channel_size * l_data.buffer_size * sizeof(value_type) > bv.size() - read_index) {
//                return load_error::invalid_file_size;
//            }
//
//            l_data.read_index = read_index;
//            return load_error::no_error;
//        }
//    } // namespace detail.
//
//    inline const char* error_to_string(load_error err) {
//        switch (err) {
//        case load_error::no_error:
//            return "";
//        case load_error::unable_to_open_file:
//            return "Unable to open file";
//        case load_error::invalid_file_format:
//            return "Invalid file format";
//        case load_error::invalid_file_size:
//            return "Invalid file size";
//        case load_error::invalid_sampling_rate:
//            return "Invalid sampling rate";
//        case load_error::invalid_channel_size:
//            return "Invalid channel size";
//        case load_error::invalid_buffer_size:
//            return "Invalid buffer size";
//        }
//
//        return "";
//    }
//
//    inline const char* error_to_string(save_error err) {
//        switch (err) {
//        case save_error::no_error:
//            return "";
//        case save_error::unable_to_open_file:
//            return "Unable to open file";
//        case save_error::invalid_sampling_rate:
//            return "Invalid sampling rate";
//        case save_error::invalid_channel_size:
//            return "Invalid channel size";
//        case save_error::invalid_buffer_size:
//            return "Invalid buffer size";
//        }
//
//        return "";
//    }
//
//    template <typename _Tp>
//    load_error load(const std::filesystem::path& file_path, audio_data<_Tp>& data) {
//        using value_type = _Tp;
//        using data_type = audio_data<_Tp>;
//
//        fst::mapped_file file;
//        if (!file.open(file_path)) {
//            return load_error::unable_to_open_file;
//        }
//
//        detail::load_data l_data;
//        fst::byte_view bv(file.content());
//
//        load_error i_load_error = detail::internal_load<value_type>(bv, l_data);
//        if (i_load_error != load_error::no_error) {
//            return i_load_error;
//        }
//
//        std::uint32_t sampling_rate = l_data.sampling_rate;
//        std::uint32_t channel_size = l_data.channel_size;
//        std::uint32_t buffer_size = l_data.buffer_size;
//        std::size_t read_index = l_data.read_index;
//
//        std::vector<std::vector<value_type>> buffers;
//        buffers.resize(channel_size);
//        for (auto& b : buffers) {
//            b.resize(buffer_size);
//        }
//
//        for (std::size_t i = 0; i < channel_size; i++) {
//            std::memmove(buffers[i].data(), bv.data(read_index), sizeof(value_type) * buffer_size);
//            read_index += sizeof(value_type) * buffer_size;
//        }
//
//        data = data_type(sampling_rate, std::move(buffers));
//
//        return load_error::no_error;
//    }
//
//    template <typename _Tp>
//    load_error load(const std::filesystem::path& file_path, audio_stream_data<_Tp>& s_data) {
//        using value_type = _Tp;
//        using data_type = audio_stream_data<_Tp>;
//        using channel_view = typename data_type::channel_view;
//
//        fst::mapped_file file;
//        if (!file.open(file_path)) {
//            return load_error::unable_to_open_file;
//        }
//
//        detail::load_data l_data;
//        fst::byte_view bv(file.content());
//
//        load_error i_load_error = detail::internal_load<value_type>(bv, l_data);
//        if (i_load_error != load_error::no_error) {
//            return i_load_error;
//        }
//
//        std::uint32_t sampling_rate = l_data.sampling_rate;
//        std::uint32_t channel_size = l_data.channel_size;
//        std::uint32_t buffer_size = l_data.buffer_size;
//        std::size_t read_index = l_data.read_index;
//
//        std::vector<channel_view> buffers;
//        buffers.resize(channel_size);
//        for (std::size_t i = 0; i < channel_size; i++) {
//            buffers[i] = channel_view((const value_type*)bv.data(read_index), buffer_size);
//            read_index += sizeof(value_type) * buffer_size;
//        }
//
//        s_data = data_type(std::move(file), audio_data_view<_Tp>(sampling_rate, std::move(buffers)));
//        return load_error::no_error;
//    }
//
//    template <typename _Tp>
//    load_error load(const std::filesystem::path& file_path, audio_stream_shared_data<_Tp>& s_data) {
//        using value_type = _Tp;
//        using data_type = audio_stream_shared_data<_Tp>;
//        using channel_view = typename data_type::channel_view;
//
//        fst::mapped_file file;
//        if (!file.open(file_path)) {
//            return load_error::unable_to_open_file;
//        }
//
//        detail::load_data l_data;
//        fst::byte_view bv(file.content());
//
//        load_error i_load_error = detail::internal_load<value_type>(bv, l_data);
//        if (i_load_error != load_error::no_error) {
//            return i_load_error;
//        }
//
//        std::uint32_t sampling_rate = l_data.sampling_rate;
//        std::uint32_t channel_size = l_data.channel_size;
//        std::uint32_t buffer_size = l_data.buffer_size;
//        std::size_t read_index = l_data.read_index;
//
//        std::vector<channel_view> buffers;
//        buffers.resize(channel_size);
//        for (std::size_t i = 0; i < channel_size; i++) {
//            buffers[i] = channel_view((const value_type*)bv.data(read_index), buffer_size);
//            read_index += sizeof(value_type) * buffer_size;
//        }
//
//        s_data = data_type(std::move(file), audio_data_view<_Tp>(sampling_rate, std::move(buffers)));
//        return load_error::no_error;
//    }
//
//    // Multi.
//    template <typename _Tp>
//    inline load_error load(const std::filesystem::path& file_path, audio_multi_stream_data<_Tp>& s_data) {
//        using value_type = _Tp;
//        using data_type = audio_multi_stream_data<_Tp>;
//        using view_type = audio_data_view<_Tp>;
//        using channel_view = typename view_type::channel_view;
//
//        fst::mapped_file file;
//        if (!file.open(file_path)) {
//            return load_error::unable_to_open_file;
//        }
//
//        std::size_t r_index = 0;
//        fst::byte_view bv(file.content());
//
//        std::vector<view_type> views;
//
//        do {
//            detail::load_data l_data;
//            fst::byte_view byte_view = bv.subspan(r_index);
//            load_error i_load_error = detail::internal_load<value_type>(byte_view, l_data);
//            if (i_load_error != load_error::no_error) {
//                return i_load_error;
//            }
//
//            std::uint32_t sampling_rate = l_data.sampling_rate;
//            std::uint32_t channel_size = l_data.channel_size;
//            std::uint32_t buffer_size = l_data.buffer_size;
//            std::size_t read_index = l_data.read_index;
//
//            std::vector<channel_view> buffers;
//            buffers.resize(channel_size);
//            for (std::size_t i = 0; i < channel_size; i++) {
//                buffers[i] = channel_view((const value_type*)byte_view.data(read_index), buffer_size);
//                read_index += sizeof(value_type) * buffer_size;
//            }
//
//            r_index += read_index;
//
//            views.push_back(view_type(sampling_rate, std::move(buffers)));
//
//        } while (r_index < bv.size());
//
//        s_data = data_type(std::move(file), std::move(views));
//
//        return load_error::no_error;
//    }
//
//    template <typename _Tp>
//    save_error internal_save(audio_data_view<_Tp> data_view, fst::byte_vector& data) {
//        if (data_view.sampling_rate() == 0) {
//            return save_error::invalid_sampling_rate;
//        }
//
//        if (data_view.channel_size() == 0) {
//            return save_error::invalid_channel_size;
//        }
//
//        if (data_view.buffer_size() == 0) {
//            return save_error::invalid_buffer_size;
//        }
//
//        const std::uint32_t sampling_rate = (std::uint32_t)data_view.sampling_rate();
//        const std::uint32_t channel_size = (std::uint32_t)data_view.channel_size();
//        const std::uint32_t buffer_size = (std::uint32_t)data_view.buffer_size();
//        const std::size_t channel_data_size = sizeof(_Tp) * buffer_size;
//
//        //    fst::byte_vector data;
//        std::size_t start_index = data.size();
//        data.resize(data.size() + detail::header_id.size() + sizeof(std::uint32_t) * 3 + channel_size * channel_data_size);
//        for (std::size_t i = 0; i < detail::header_id.size(); i++) {
//            data[start_index + i] = detail::header_id[i];
//        }
//
//        std::size_t write_index = start_index + detail::header_id.size();
//        data.as_ref<std::uint32_t>(write_index) = sampling_rate;
//        write_index += sizeof(std::uint32_t);
//        data.as_ref<std::uint32_t>(write_index) = channel_size;
//        write_index += sizeof(std::uint32_t);
//        data.as_ref<std::uint32_t>(write_index) = buffer_size;
//        write_index += sizeof(std::uint32_t);
//
//        for (std::size_t i = 0; i < channel_size; i++) {
//            std::memmove(data.data(write_index), data_view.get_buffer(i).data(), channel_data_size);
//            write_index += channel_data_size;
//        }
//
//        return save_error::no_error;
//    }
//
//    template <typename _Tp>
//    save_error save(const std::filesystem::path& file_path, audio_data_view<_Tp> data_view) {
//        if (data_view.sampling_rate() == 0) {
//            return save_error::invalid_sampling_rate;
//        }
//
//        if (data_view.channel_size() == 0) {
//            return save_error::invalid_channel_size;
//        }
//
//        if (data_view.buffer_size() == 0) {
//            return save_error::invalid_buffer_size;
//        }
//
//        const std::uint32_t sampling_rate = (std::uint32_t)data_view.sampling_rate();
//        const std::uint32_t channel_size = (std::uint32_t)data_view.channel_size();
//        const std::uint32_t buffer_size = (std::uint32_t)data_view.buffer_size();
//        const std::size_t channel_data_size = sizeof(_Tp) * buffer_size;
//
//        fst::byte_vector data;
//        data.resize(detail::header_id.size() + sizeof(std::uint32_t) * 3 + channel_size * channel_data_size);
//        for (std::size_t i = 0; i < detail::header_id.size(); i++) {
//            data[i] = detail::header_id[i];
//        }
//
//        std::size_t write_index = detail::header_id.size();
//        data.as_ref<std::uint32_t>(write_index) = sampling_rate;
//        write_index += sizeof(std::uint32_t);
//        data.as_ref<std::uint32_t>(write_index) = channel_size;
//        write_index += sizeof(std::uint32_t);
//        data.as_ref<std::uint32_t>(write_index) = buffer_size;
//        write_index += sizeof(std::uint32_t);
//
//        for (std::size_t i = 0; i < channel_size; i++) {
//            std::memmove(data.data(write_index), data_view.get_buffer(i).data(), channel_data_size);
//            write_index += channel_data_size;
//        }
//
//        if (!data.write_to_file(file_path)) {
//            return save_error::unable_to_open_file;
//        }
//
//        return save_error::no_error;
//    }
//
//    template <typename _Tp>
//    inline save_error save(const std::filesystem::path& file_path, const std::vector<audio_data_view<_Tp>>& data_views) {
//
//        fst::byte_vector data;
//        for (const auto& dv : data_views) {
//            save_error s_err = save(dv, data);
//            if (s_err != save_error::no_error) {
//                return s_err;
//            }
//        }
//
//        if (!data.write_to_file(file_path)) {
//            return save_error::unable_to_open_file;
//        }
//
//        return save_error::no_error;
//    }
//
//    template <typename _Tp>
//    inline save_error save(const std::filesystem::path& file_path, const std::vector<audio_data<_Tp>>& au_data) {
//
//        fst::byte_vector data;
//        for (const auto& dv : au_data) {
//            save_error s_err = internal_save(audio_data_view<_Tp>(dv), data);
//            if (s_err != save_error::no_error) {
//                return s_err;
//            }
//        }
//
//        if (!data.write_to_file(file_path)) {
//            return save_error::unable_to_open_file;
//        }
//
//        return save_error::no_error;
//    }
//} // namespace raw.
