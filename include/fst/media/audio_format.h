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
#include "fst/int24_t.h"
#include "fst/status_code.h"
#include "fst/vector.h"
#include "fst/media/audio_buffer.h"

FST_BEGIN_NAMESPACE

    enum class audio_format_type {
        unknown,
        pcm_8s, // Signed 8 bit
        pcm_8u, // Unsigned 8 bit
        pcm_16s, // Signed 16 bit
        pcm_16u, // Unsigned 16 bit
        pcm_24s, // Signed 24 bit
        pcm_24u, // Unsigned 24 bit
        pcm_32s, // Signed 32 bit
        pcm_32u, // Unsigned 32 bit
        ieee_32, // Float 32 bit
        ieee_64, // Float 64 bit

        pcm_8s_be, // Big endian signed 8 bit.
        pcm_8u_be, // Big endian unsigned 8 bit.
        pcm_16s_be, // Big endian signed 16 bit.
        pcm_16u_be, // Big endian unsigned 16 bit.
        pcm_24s_be, // Big endian signed 24 bit.
        pcm_24u_be, // Big endian unsigned 24 bit.
        pcm_32s_be, // Big endian signed 32 bit.
        pcm_32u_be, // Big endian unsigned 32 bit.
        ieee_32_be, // Big endian float 32 bit
        ieee_64_be, // Big endian float 64 bit
    };

    template <audio_format_type Type>
    inline constexpr bool is_audio_format_little_endian()
    {
        return Type >= audio_format_type::pcm_8s && Type <= audio_format_type::ieee_64;
    }

    template <audio_format_type Type>
    inline constexpr bool is_audio_format_big_endian()
    {
        return Type >= audio_format_type::pcm_8s_be && Type <= audio_format_type::ieee_64_be;
    }

    inline constexpr bool is_audio_format_little_endian(audio_format_type type)
    {
        return type >= audio_format_type::pcm_8s && type <= audio_format_type::ieee_64;
    }

    inline constexpr bool is_audio_format_big_endian(audio_format_type type)
    {
        return type >= audio_format_type::pcm_8s_be && type <= audio_format_type::ieee_64_be;
    }

    template <audio_format_type Type>
    inline constexpr bool is_audio_format_pcm()
    {
        return (Type >= audio_format_type::pcm_8s && Type <= audio_format_type::pcm_32u)
               || (Type >= audio_format_type::pcm_8s_be && Type <= audio_format_type::pcm_32u_be);
    }

    inline constexpr bool is_audio_format_pcm(audio_format_type type)
    {
        return (type >= audio_format_type::pcm_8s && type <= audio_format_type::pcm_32u)
               || (type >= audio_format_type::pcm_8s_be && type <= audio_format_type::pcm_32u_be);
    }

    inline constexpr bool is_floating_point_audio_format(audio_format_type f)
    {

        FST_PRAGMA_PUSH()
        FST_PRAGMA_DISABLE_WARNING_MSVC(4061)
        FST_PRAGMA_DISABLE_WARNING_MSVC(4062)
        switch (f)
        {
        case audio_format_type::ieee_32:
        case audio_format_type::ieee_64:
        case audio_format_type::ieee_32_be:
        case audio_format_type::ieee_64_be: return true;
        }
        FST_PRAGMA_POP()

        return false;
    }

    template <audio_format_type Type>
    struct audio_format_to_type;

    template <>
    struct audio_format_to_type<audio_format_type::pcm_8s>
    {
        using type = int8_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_8u>
    {
        using type = uint8_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_16s>
    {
        using type = int16_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_16u>
    {
        using type = uint16_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_24s>
    {
        using type = __fst::int24_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_24u>
    {
        using type = __fst::uint24_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_32s>
    {
        using type = int32_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_32u>
    {
        using type = uint32_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::ieee_32>
    {
        using type = float;
    };
    template <>
    struct audio_format_to_type<audio_format_type::ieee_64>
    {
        using type = double;
    };

    template <>
    struct audio_format_to_type<audio_format_type::pcm_8s_be>
    {
        using type = int8_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_8u_be>
    {
        using type = uint8_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_16s_be>
    {
        using type = int16_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_16u_be>
    {
        using type = uint16_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_24s_be>
    {
        using type = __fst::int24_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_24u_be>
    {
        using type = __fst::uint24_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_32s_be>
    {
        using type = int32_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::pcm_32u_be>
    {
        using type = uint32_t;
    };
    template <>
    struct audio_format_to_type<audio_format_type::ieee_32_be>
    {
        using type = float;
    };
    template <>
    struct audio_format_to_type<audio_format_type::ieee_64_be>
    {
        using type = double;
    };

    template <audio_format_type Type>
    using audio_format_type_t = typename audio_format_to_type<Type>::type;

    template <class T>
    struct to_audio_format;

    template <>
    struct to_audio_format<int8_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_8s;
    };
    template <>
    struct to_audio_format<uint8_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_8u;
    };
    template <>
    struct to_audio_format<int16_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_16s;
    };
    template <>
    struct to_audio_format<uint16_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_16u;
    };
    template <>
    struct to_audio_format<__fst::int24_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_24s;
    };
    template <>
    struct to_audio_format<__fst::uint24_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_24u;
    };
    template <>
    struct to_audio_format<int32_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_32s;
    };
    template <>
    struct to_audio_format<uint32_t>
    {
        static constexpr audio_format_type value = audio_format_type::pcm_32u;
    };
    template <>
    struct to_audio_format<float>
    {
        static constexpr audio_format_type value = audio_format_type::ieee_32;
    };
    template <>
    struct to_audio_format<double>
    {
        static constexpr audio_format_type value = audio_format_type::ieee_64;
    };

    template <class T>
    static constexpr audio_format_type to_audio_format_v = to_audio_format<T>::value;

    inline constexpr int audio_format_to_bit_depth(audio_format_type f)
    {
        switch (f)
        {
        case audio_format_type::unknown: return 0;
        case audio_format_type::pcm_8s: return 8;
        case audio_format_type::pcm_8s_be: return 8;
        case audio_format_type::pcm_8u: return 8;
        case audio_format_type::pcm_8u_be: return 8;
        case audio_format_type::pcm_16s: return 16;
        case audio_format_type::pcm_16s_be: return 16;
        case audio_format_type::pcm_16u: return 16;
        case audio_format_type::pcm_16u_be: return 16;
        case audio_format_type::pcm_24s: return 24;
        case audio_format_type::pcm_24s_be: return 24;
        case audio_format_type::pcm_24u: return 24;
        case audio_format_type::pcm_24u_be: return 24;
        case audio_format_type::pcm_32s: return 32;
        case audio_format_type::pcm_32s_be: return 32;
        case audio_format_type::pcm_32u: return 32;
        case audio_format_type::pcm_32u_be: return 32;
        case audio_format_type::ieee_32: return 32;
        case audio_format_type::ieee_32_be: return 32;
        case audio_format_type::ieee_64: return 64;
        case audio_format_type::ieee_64_be: return 64;
        }

        return 0;
    }

    inline constexpr audio_format_type bit_depth_to_audio_format(int depth)
    {
        switch (depth)
        {
        case 8: return audio_format_type::pcm_8s;
        case 16: return audio_format_type::pcm_16s;
        case 24: return audio_format_type::pcm_24s;
        case 32: return audio_format_type::ieee_32;
        }

        return audio_format_type::pcm_24s;
    }

    inline constexpr const char* audio_format_to_str(audio_format_type f)
    {
        switch (f)
        {
        case audio_format_type::unknown: return "unknown";
        case audio_format_type::pcm_8s: return "pcm_8s";
        case audio_format_type::pcm_8s_be: return "pcm_8s_be";
        case audio_format_type::pcm_8u: return "pcm_8u";
        case audio_format_type::pcm_8u_be: return "pcm_8u_be";
        case audio_format_type::pcm_16s: return "pcm_16s";
        case audio_format_type::pcm_16s_be: return "pcm_16s_be";
        case audio_format_type::pcm_16u: return "pcm_16u";
        case audio_format_type::pcm_16u_be: return "pcm_16u_be";
        case audio_format_type::pcm_24s: return "pcm_24s";
        case audio_format_type::pcm_24s_be: return "pcm_24s_be";
        case audio_format_type::pcm_24u: return "pcm_24u";
        case audio_format_type::pcm_24u_be: return "pcm_24u_be";
        case audio_format_type::pcm_32s: return "pcm_32s";
        case audio_format_type::pcm_32s_be: return "pcm_32s_be";
        case audio_format_type::pcm_32u: return "pcm_32u";
        case audio_format_type::pcm_32u_be: return "pcm_32u_be";
        case audio_format_type::ieee_32: return "ieee_32";
        case audio_format_type::ieee_32_be: return "ieee_32_be";
        case audio_format_type::ieee_64: return "ieee_64";
        case audio_format_type::ieee_64_be: return "ieee_64_be";
        }

        return "unknown";
    }

    namespace detail
    {
        template <class T>
        struct audio_format_data_type_base
        {
            using type = T;
        };

        template <>
        struct audio_format_data_type_base<int24_t>
        {
            using type = int24_t::int_type;
        };

        template <>
        struct audio_format_data_type_base<uint24_t>
        {
            using type = uint24_t::int_type;
        };

        template <class T>
        using audio_format_data_type = typename audio_format_data_type_base<T>::type;

        template <class T, class Enable = void>
        struct audio_format_range
        {
            static inline constexpr T min() { return __fst::numeric_limits<T>::min(); }
            static inline constexpr T max() { return __fst::numeric_limits<T>::max(); }
        };

        template <class T>
        struct audio_format_range<T, __fst::enable_if_t<__fst::is_floating_point<T>::value>>
        {
            static inline constexpr T min() { return (T) -1; }
            static inline constexpr T max() { return (T) 1; }
        };

        template <>
        struct audio_format_range<int24_t>
        {
            static inline constexpr int24_t::int_type min() { return -8388608; }
            static inline constexpr int24_t::int_type max() { return 8388607; }
        };

        template <>
        struct audio_format_range<uint24_t>
        {
            static inline constexpr uint24_t::int_type min() { return 0; }
            static inline constexpr uint24_t::int_type max() { return 16777215; }
        };

        template <class _Ty, class DstType>
        static inline constexpr DstType clamp(_Ty value)
        {
            return value > audio_format_range<DstType>::max()   ? audio_format_range<DstType>::max()
                   : value < audio_format_range<DstType>::min() ? audio_format_range<DstType>::min()
                                                                : value;
        }

        template <class T>
        using IsIntegral = __fst::bool_t<__fst::is_integral<T>::value || __fst::is_same<T, int24_t>::value>;

        template <class T>
        using IsSigned = __fst::bool_t<(__fst::is_integral<T>::value && __fst::is_signed<T>::value) || __fst::is_same<T, int24_t>::value>;

        template <class T>
        using IsUnsigned = __fst::bool_t<__fst::is_integral<T>::value && __fst::is_unsigned<T>::value>;

        template <class SrcType, class DstType, bool Cond>
        using EnableIfDiff = __fst::enable_if_t<!__fst::is_same<SrcType, DstType>::value && Cond>;

        template <class SrcType, class DstType, class Enable = void>
        struct audio_format_converter
        {
            static inline DstType convert(SrcType value) { return (DstType) value; }
        };

        template <class T>
        struct audio_format_converter<T, T>
        {
            static inline T convert(T value) { return value; }
        };

        // signed to unsigned
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, IsSigned<SrcType>::value && IsUnsigned<DstType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                constexpr double ratio = (double(audio_format_range<__fst::make_signed<DstType>::type>::max()) + 1.0) / double(audio_format_range<SrcType>::max());
                return (DstType) clamp<double, DstType>(double(value) * ratio + audio_format_range<__fst::make_signed<DstType>::type>::max());
            }
        };

        // unsigned to signed
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, IsUnsigned<SrcType>::value && IsSigned<DstType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                constexpr double ratio = 1.0 / (double(audio_format_range<SrcType>::max()));
                return (DstType) clamp<double, DstType>((((double(value) * ratio) - 0.5) * 2.0) * (double(audio_format_range<DstType>::max()) + 1));
            }
        };

        // unsigned to unsigned
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, IsUnsigned<SrcType>::value && IsUnsigned<DstType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                constexpr double ratio = 1.0 / (double(audio_format_range<SrcType>::max()));
                return (DstType) clamp<double, DstType>((double(value) * ratio) * (double(audio_format_range<DstType>::max())));
            }
        };

        // signed to signed
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, IsSigned<SrcType>::value && IsSigned<DstType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                constexpr double ratio = (double(audio_format_range<DstType>::max()) + 1.0) / double(audio_format_range<SrcType>::max());
                return (DstType) clamp<double, DstType>(double(value) * ratio);
            }
        };

        // signed to float
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, __fst::is_floating_point<DstType>::value && IsSigned<SrcType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                constexpr DstType ratio = (DstType) (1.0 / (audio_format_range<SrcType>::max() + 1.0));
                return (DstType) (value) *ratio;
            }
        };

        // unsigned to float
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, __fst::is_floating_point<DstType>::value && IsUnsigned<SrcType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                constexpr DstType m = DstType(audio_format_range<__fst::make_signed_t<SrcType>>::max()) + 1;
                return (DstType(value) - m) / m;

                //constexpr DstType ratio = (DstType)1.0 / FormatRange<SrcType>::max();
                //return ((DstType(value) * ratio) - (DstType)0.5) * (DstType)2.0;
            }
        };

        // float to signed
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, __fst::is_floating_point<SrcType>::value && IsSigned<DstType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                //if (__fst::is_same<int8_t, DstType>::value)
                //return static_cast<DstType> (clamp<SrcType>(value, -1., 1.) * (SrcType)0x7F);
                return (DstType) clamp<double, DstType>(double(value) * (double(audio_format_range<DstType>::max()) + 1.0));
            }
        };

        // float to unsigned
        template <class SrcType, class DstType>
        struct audio_format_converter<SrcType, DstType, EnableIfDiff<SrcType, DstType, __fst::is_floating_point<SrcType>::value && IsUnsigned<DstType>::value>>
        {
            static inline DstType convert(SrcType value)
            {
                //sample = clamp<T>(sample, -1., 1.);
                //return static_cast<int8_t> (sample * (T)0x7F);
                return (DstType) ((double(value) * 0.5 + 0.5) * audio_format_range<DstType>::max());
            }
        };
    } // namespace detail

    template <class DstType, class SrcType>
    inline DstType convert_audio_sample(SrcType value)
    {
        return detail::audio_format_converter<SrcType, DstType>::convert(value);
    }

    template <audio_format_type DstType, audio_format_type SrcType>
    FST_NODISCARD inline __fst::status_code convert_audio_buffer(const uint8_t* data, __fst::audio_buffer<audio_format_type_t<DstType>>& buffer, size_t numSamples,
        size_t numChannels, size_t numBytesPerBlock, size_t numBytesPerSample)
    {
        using dst_type = audio_format_type_t<DstType>;
        using src_type = audio_format_type_t<SrcType>;

        if (__fst::status_code sc = buffer.resize((uint32_t) numChannels, (uint32_t) numSamples); sc != __fst::status_code::success) { return sc; }

        if constexpr (__fst::is_audio_format_little_endian<SrcType>())
        {
            if (numBytesPerBlock == sizeof(src_type) * numChannels && numBytesPerSample == sizeof(src_type))
            {
                if constexpr (__fst::is_same<dst_type, src_type>::value)
                {
                    if (numChannels == 1)
                    {
                        __fst::memcpy(buffer.channel(0), data, sizeof(dst_type) * numSamples);
                        return __fst::status_code::success;
                    }
                }

                const src_type* dataPtr = (const src_type*) data;
                for (size_t i = 0; i < numSamples; i++)
                {
                    for (size_t channel = 0; channel < numChannels; channel++)
                    {
                        buffer.channel((uint32_t) channel)[i] = __fst::convert_audio_sample<dst_type>(*dataPtr++);
                    }
                }

                return __fst::status_code::success;
            }

            for (size_t i = 0, offset = 0; i < numSamples; i++, offset += numBytesPerBlock)
            {
                for (size_t channel = 0, delta = offset; channel < numChannels; channel++, delta += numBytesPerSample)
                {
                    buffer.channel((uint32_t) channel)[i] = __fst::convert_audio_sample<dst_type>(*(const src_type*) (data + delta));
                }
            }
        }
        else
        {
            if (numBytesPerBlock == sizeof(src_type) * numChannels && numBytesPerSample == sizeof(src_type))
            {
                const src_type* dataPtr = (const src_type*) data;
                for (size_t i = 0; i < numSamples; i++)
                {
                    for (size_t channel = 0; channel < numChannels; channel++)
                    {
                        buffer.channel((uint32_t) channel)[i] = __fst::convert_audio_sample<dst_type>(__fst::byte_swap(*dataPtr++));
                    }
                }

                return __fst::status_code::success;
            }

            for (size_t i = 0, offset = 0; i < numSamples; i++, offset += numBytesPerBlock)
            {
                for (size_t channel = 0, delta = offset; channel < numChannels; channel++, delta += numBytesPerSample)
                {
                    buffer.channel((uint32_t) channel)[i] = __fst::convert_audio_sample<dst_type>(__fst::byte_swap(*(const src_type*) (data + delta)));
                }
            }
        }

        return __fst::status_code::success;
    }

    template <audio_format_type DstType, audio_format_type SrcType>
    static inline void serialize_audio_buffer(__fst::vector<uint8_t> & data, const __fst::audio_buffer<audio_format_type_t<SrcType>>& input_buffer)
    {
        using dst_type = audio_format_type_t<DstType>;

        const uint32_t channel_size = input_buffer.channel_size();
        const uint32_t buffer_size = input_buffer.buffer_size();
        constexpr int16_t bit_depth = __fst::audio_format_to_bit_depth(DstType);

        const size_t sampleStartIndex = data.size();
        data.resize(sampleStartIndex + buffer_size * channel_size * bit_depth / 8);

        dst_type* output = (dst_type*) (data.data() + sampleStartIndex);
        for (uint32_t i = 0; i < buffer_size; i++)
        {
            for (uint32_t channel = 0; channel < channel_size; channel++)
            {
                *output++ = convert_audio_sample<dst_type>(input_buffer[channel][i]);
            }
        }
    }

    template <audio_format_type Type>
    static inline void serialize_audio_buffer(__fst::vector<uint8_t> & data, const __fst::audio_buffer<audio_format_type_t<Type>>& input_buffer)
    {
        using dst_type = audio_format_type_t<Type>;

        const size_t channel_size = input_buffer.channel_size();
        const size_t buffer_size = input_buffer.buffer_size();
        const int16_t bit_depth = __fst::audio_format_to_bit_depth(Type);

        const size_t sampleStartIndex = data.size();
        data.resize(sampleStartIndex + buffer_size * channel_size * bit_depth / 8);

        dst_type* output = (dst_type*) (data.data() + sampleStartIndex);

        if (channel_size == 1) { __fst::memcpy(output, input_buffer[0], buffer_size * sizeof(dst_type)); }
        else if (channel_size == 2)
        {
            const dst_type* inputBufferLeft = input_buffer[0];
            const dst_type* inputBufferRight = input_buffer[1];

            for (size_t i = 0; i < buffer_size; i++)
            {
                *output++ = *inputBufferLeft++;
                *output++ = *inputBufferRight++;
            }
        }
        else
        {
            for (size_t i = 0; i < buffer_size; i++)
            {
                for (size_t channel = 0; channel < channel_size; channel++)
                {
                    *output++ = input_buffer[channel][i];
                }
            }
        }
    }

FST_END_NAMESPACE
