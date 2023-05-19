#include "fst/time.h"
#include "fst/utility.h"
#include "fst/stream.h"
#include <stdio.h>
#include <inttypes.h>

FST_BEGIN_NAMESPACE

    time_point system_clock::now() noexcept
    {
        struct timespec ts;

#if __FST_GCC__

        if (clock_gettime(CLOCK_REALTIME, &ts) == 0) { return time_point(); }
        return time_point(ts.tv_sec, ts.tv_nsec);
#else

        if (timespec_get(&ts, TIME_UTC) == 0) { return time_point(); }
        return time_point(ts.tv_sec, ts.tv_nsec);
#endif

        //return time_point(::time(0));
    }

    __fst::output_stream<char>& operator<<(__fst::output_stream<char>& stream, time_point now)
    {
        // https://stackoverflow.com/a/35157784
        constexpr size_t time_size = __fst::char_traits<char>::length("HH:MM:SS");
        constexpr size_t ms_size = __fst::char_traits<char>::length(":MMM");
        constexpr size_t total_size = time_size + ms_size;
        char str[total_size + 1];

        ::time_t t = (::time_t) now.time_since_epoch();
        struct tm tms;

#if __FST_WINDOWS__
        if (FST_ATTRIBUTE_UNUSED errno_t err = ::localtime_s(&tms, &t))
        {
            __fst::unused(err);
            fst_error("time convertion error");
            return stream;
        }
#else
        if (struct tm* tt = ::localtime(&t)) { tms = *tt; }
        else
        {
            fst_error("time convertion error");
            return stream;
        }
#endif // __FST_WINDOWS__

        //if (::strftime(str, sizeof(str), "%%T", &tms) != time_size) { return stream; }
        if (::strftime(str, sizeof(str), "%T", &tms) != time_size) { return stream; }

        // If a call to sprintf or snprintf causes copying to take place between
        // objects that overlap, the behavior is undefined (e.g. sprintf(buf, "%s
        // text", buf);).
        int r = ::snprintf(str + time_size, ms_size + 1, ":%03" PRIu64, now.milliseconds());
        if (r == ms_size) { stream << str; }

        return stream;
    }

    size_t time_to_string(__fst::memory_range<char, 13> buffer, time_point now, char sep) noexcept
    {
        // https://stackoverflow.com/a/35157784
        constexpr size_t time_size = __fst::char_traits<char>::length("HH:MM:SS");
        constexpr size_t ms_size = __fst::char_traits<char>::length(":MMM");
        constexpr size_t total_size = time_size + ms_size;

        ::time_t t = (::time_t) now.time_since_epoch();
        struct tm tms;

#if __FST_WINDOWS__
        if (FST_ATTRIBUTE_UNUSED errno_t err = ::localtime_s(&tms, &t))
        {
            __fst::unused(err);
            fst_error("time convertion error");
            return 0;
        }
#else
        if (struct tm* tt = ::localtime(&t)) { tms = *tt; }
        else
        {
            fst_error("time convertion error");
            return 0;
        }
#endif // __FST_WINDOWS__

        char fmt_buffer[9] = { '%', 'I', sep, '%', 'M', sep, '%', 'S', 0 };
        if (::strftime(buffer.data(), total_size + 1, fmt_buffer, &tms) != time_size) { return 0; }

        // If a call to sprintf or snprintf causes copying to take place between
        // objects that overlap, the behavior is undefined (e.g. sprintf(buf, "%s
        // text", buf);).
        int r = ::snprintf(buffer.data() + time_size, ms_size + 1, "%c%03" PRIu64, sep, now.milliseconds());
        if (r == ms_size) { return total_size; }

        return 0;
    }

    size_t time_to_string2(__fst::memory_range<char, 12> buffer, time_point now, char sep) noexcept
    {
        // https://stackoverflow.com/a/35157784
        constexpr size_t time_size = __fst::char_traits<char>::length("HH:MM:SS");
        constexpr size_t ms_size = __fst::char_traits<char>::length(":MMM");
        constexpr size_t total_size = time_size + ms_size;

        ::time_t t = (::time_t) now.time_since_epoch();
        struct tm tms;

#if __FST_WINDOWS__
        if (FST_ATTRIBUTE_UNUSED errno_t err = ::localtime_s(&tms, &t))
        {
            __fst::unused(err);
            fst_error("time convertion error");
            return 0;
        }
#else
        if (struct tm* tt = ::localtime(&t)) { tms = *tt; }
        else
        {
            fst_error("time convertion error");
            return 0;
        }
#endif // __FST_WINDOWS__

        char buf[total_size + 1];
        char fmt_buffer[9] = { '%', 'I', sep, '%', 'M', sep, '%', 'S', 0 };
        if (::strftime(buf, total_size + 1, fmt_buffer, &tms) != time_size) { return 0; }

        // If a call to sprintf or snprintf causes copying to take place between
        // objects that overlap, the behavior is undefined (e.g. sprintf(buf, "%s
        // text", buf);).
        int r = ::snprintf(buf + time_size, ms_size + 1, "%c%03" PRIu64, sep, now.milliseconds());
        if (r == ms_size)
        {
            __fst::memcpy(buffer.data(), buf, total_size);
            return total_size;
        }

        return 0;
    }

FST_END_NAMESPACE
