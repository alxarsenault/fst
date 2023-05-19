#include "fst/trace.h"
#include "fst/time.h"
#include "fst/string.h"
#include <stdio.h>

FST_BEGIN_NAMESPACE

    FST_PRAGMA_PUSH()
    FST_PRAGMA_DISABLE_WARNING_CLANG("-Wexit-time-destructors")
    __fst::output_stream<char>& trace_stream(size_t level, const char* section, const __fst::source_location& loc, size_t has_msg) noexcept
    {
        struct file_tracer
        {
            file_tracer() noexcept
                : _stream{ this,
                    [](void* data, const char* str, size_t size, stream_modifier) noexcept -> size_t
                    {
                        return ::fwrite(str, size, 1, ((file_tracer*) data)->_file);
                    } }

            {
                char trace_path[] = FST_TRACE_DIRECTORY "/trace_HH_MM_SS_MMM.log\0";

                time_to_string2(__fst::memory_range<char, 12>(trace_path + sizeof(trace_path) - 18), __fst::system_clock::now(), '_');
                _file = ::fopen(trace_path, "a+");
            }

            ~file_tracer() noexcept { ::fclose(_file); }

            FILE* _file;
            __fst::output_stream<char> _stream;
        };
        static file_tracer _tracer;

        static size_t count = 0;
        __fst::output_stream<char>& stream = _tracer._stream << "- item :";

        constexpr auto print_label = [](__fst::output_stream<char>& stream, const char* label) -> __fst::output_stream<char>&
        {
            return stream << "\n    " << __fst::padded<8>(label) << ": ";
        };
        //<< " - time    : " << __fst::system_clock::now();

        print_label(stream, "id") << __fst::zero_padded_right<4>(++count);
        print_label(stream, "time") << "'" << __fst::system_clock::now() << "'";

        print_label(stream, "level") << __fst::zero_padded_right<2>(level);
        print_label(stream, "section") << section;
        print_label(stream, "file") << "'" << loc.file_name() << "'";
        print_label(stream, "function") << "'" << loc.function_name() << "'";
        print_label(stream, "line") << loc.line();

        if (has_msg) { print_label(stream, "message"); }
        return stream;
    }
    FST_PRAGMA_POP()

FST_END_NAMESPACE
