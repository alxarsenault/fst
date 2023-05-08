#include "utest.h"
#include "fst/int24_t.h"
#include "fst/media/audio_format.h"
#include "fst/media/interchange_file_format.h"
#include "fst/media/audio_buffer.h"
#include "fst/media/audio_bus.h"
#include "fst/media/audio_file.h"

namespace
{
    TEST_CASE("fst::media", "[int24_t]")
    {
        //fst::audio_buffer<float> buffer(2, 1024);
        //REQUIRE_EQ(buffer.channel_size(), 2);
        //REQUIRE_EQ(buffer.buffer_size(), 1024);

        fst::audio_buffer<float> buffer;
        uint32_t sampling_rate = 0;
        fst::audio_format_type format;
        //AudioBufferType& output_buffer, uint32_t& sampling_rate, __fst::audio_format_type& format
        fst::read_wave_file(FST_TEST_RESOURCES_DIRECTORY "/wav_2_pcm_16_48000_384000.wav", buffer, sampling_rate, format);
        //REQUIRE(sampling_rate == 48000);
    }
} // namespace
