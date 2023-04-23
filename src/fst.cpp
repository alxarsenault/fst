#include "fst/common.h"
#include <stdio.h>
#include <stdarg.h>

FST_BEGIN_NAMESPACE
    int printf(const char* format, ...) noexcept
    {
        va_list arglist;
        va_start(arglist, format);
        int ret = vprintf(format, arglist);
        va_end(arglist);
        return ret;
    }

    int warnprintf(const char* format, ...) noexcept
    {
        va_list arglist;
        va_start(arglist, format);
        int ret = vfprintf(stderr, format, arglist);
        va_end(arglist);
        return ret;
    }

    int fprintf(void* const file, const char* format, ...) noexcept
    {

        va_list arglist;
        va_start(arglist, format);
        int ret = vfprintf((FILE*) file, format, arglist);
        va_end(arglist);
        return ret;
    }

    int snprintf(char* buffer, size_t bufsz, const char* format, ...) noexcept
    {
        va_list arglist;
        va_start(arglist, format);
        int ret = vsnprintf(buffer, bufsz, format, arglist);
        va_end(arglist);
        return ret;
    }

    size_t write_stdout(const char* data, size_t size) noexcept
    {
        return ::fwrite(data, size, 1, stdout);
    }

    size_t write_wstdout(const wchar_t* data, size_t size) noexcept
    {

        for (size_t i = 0; i < size; i++)
        {
            fputwc(data[i], stdout);
        }

        return size;
    }

    void print_version() noexcept
    {
        printf("fst version %s (C++%d)\n", __FST_LONG_VERSION_STR__, __FST_CPP_VERSION__);
    }
FST_END_NAMESPACE
