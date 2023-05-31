#include "fst/file.h"

#if __FST_WINDOWS__
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include "fst/unicode.h"

#else
#include <stdio.h>
#endif // __FST__WINDOWS__

FST_BEGIN_NAMESPACE
#if __FST_WINDOWS__

    file::~file() noexcept
    {
        this->close();
    }

    FST_NODISCARD FST_ALWAYS_INLINE bool file::is_open() const noexcept
    {
        return _native != nullptr;
    }

    FST_NODISCARD __fst::status file::open(const char* filepath, __fst::open_mode flags) noexcept
    {
        if (is_open()) { this->close(); }

        __fst::wstring wpath = __fst::utf_cvt(filepath);

        DWORD dwShareMode = 0;
        if ((flags & __fst::open_mode::read) != 0) { dwShareMode |= GENERIC_READ; }
        if ((flags & __fst::open_mode::write) != 0) { dwShareMode |= GENERIC_WRITE; }

        DWORD dwCreationDisposition = 0;
        if ((flags & __fst::open_mode::create_always) != 0) { dwCreationDisposition |= CREATE_ALWAYS; }
        if ((flags & __fst::open_mode::create_new) != 0) { dwCreationDisposition |= CREATE_NEW; }
        if ((flags & __fst::open_mode::open_always) != 0) { dwCreationDisposition |= OPEN_ALWAYS; }
        if ((flags & __fst::open_mode::open_existing) != 0) { dwCreationDisposition |= OPEN_EXISTING; }
        if ((flags & __fst::open_mode::truncate_existing) != 0) { dwCreationDisposition |= TRUNCATE_EXISTING; }

        HANDLE _handle = CreateFileW(wpath.c_str(), dwShareMode, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, dwCreationDisposition, 0, 0);

        if (_handle == INVALID_HANDLE_VALUE)
        {
            printf("Unable to create file! Error: %u\n", GetLastError());
            return __fst::status_code::unknown;
        }

        _native = (native*) _handle;

        return __fst::status_code::success;
    }

    __fst::status file::close() noexcept
    {
        if (is_open())
        {
            CloseHandle((HANDLE) _native);
            _native = nullptr;
        }

        return __fst::status_code::success;
    }

    FST_NODISCARD size_t file::write(const void* buffer, size_t buffer_size) const noexcept
    {
        DWORD numberOfBytesWritten = 0;

        // If the function succeeds, the return value is nonzero (TRUE).
        if (WriteFile((HANDLE) _native, buffer, (DWORD) buffer_size, &numberOfBytesWritten, nullptr)) { return (size_t) numberOfBytesWritten; }
        return 0;
    }

    FST_NODISCARD size_t file::read(void* buffer, size_t buffer_size) const noexcept
    {
        DWORD numberOfBytesRead = 0;

        // If the function succeeds, the return value is nonzero (TRUE).
        if (ReadFile((HANDLE) _native, buffer, (DWORD) buffer_size, &numberOfBytesRead, nullptr)) { return (size_t) numberOfBytesRead; }
        return 0;
    }

    FST_NODISCARD size_t file::size() const noexcept
    {
        DWORD s = GetFileSize((HANDLE) _native, nullptr);
        return (size_t) s;
    }

#else
    //
    // Default C FILE implementation.
    //

    file::~file() noexcept
    {
        this->close();
    }

    FST_NODISCARD bool file::is_open() const noexcept
    {
        return _native != nullptr;
    }

    FST_NODISCARD __fst::status file::open(const char* filepath, __fst::open_mode flags) noexcept
    {
        if (is_open()) { this->close(); }

        const char* mode = "";

        if ((flags & __fst::open_mode::read) != 0 && (flags & __fst::open_mode::write) != 0)
        {
            if ((flags & __fst::open_mode::create_always) != 0) { mode = "w+"; }
            else if ((flags & __fst::open_mode::create_new) != 0) { mode = "w+"; }
            else if ((flags & __fst::open_mode::open_existing) != 0) { mode = "r+"; }
            else if ((flags & __fst::open_mode::open_always) != 0) { mode = "r+"; }
            else if ((flags & __fst::open_mode::truncate_existing) != 0) { mode = "a+"; }
        }
        else if ((flags & __fst::open_mode::read) != 0)
        {
            if ((flags & __fst::open_mode::create_always) != 0) { mode = "r"; }
            else if ((flags & __fst::open_mode::create_new) != 0) { mode = "r"; }
            else if ((flags & __fst::open_mode::open_existing) != 0) { mode = "r+"; }
            else if ((flags & __fst::open_mode::open_always) != 0) { mode = "r+"; }
            else if ((flags & __fst::open_mode::truncate_existing) != 0) { mode = "a+"; }
        }
        else if ((flags & __fst::open_mode::write) != 0)
        {
            if ((flags & __fst::open_mode::create_always) != 0) { mode = "w+"; }
            else if ((flags & __fst::open_mode::create_new) != 0) { mode = "w+"; }
            else if ((flags & __fst::open_mode::open_existing) != 0) { mode = "+"; }
            else if ((flags & __fst::open_mode::open_always) != 0) { mode = "r+"; }
            else if ((flags & __fst::open_mode::truncate_existing) != 0) { mode = "a+"; }
        }

        FILE* f = ::fopen(filepath, mode);
        if (!f) { return __fst::status_code::unknown; }

        _native = (native*) f;

        return __fst::status_code::success;
    }

    __fst::status file::close() noexcept
    {
        if (is_open())
        {
            ::fclose((FILE*) _native);
            _native = nullptr;
        }

        return __fst::status_code::success;
    }

    FST_NODISCARD size_t file::write(const void* buffer, size_t buffer_size) const noexcept
    {
        size_t wbytes = ::fwrite(buffer, 1, buffer_size, (FILE*) _native);
        return wbytes;
    }

    FST_NODISCARD size_t file::read(void* buffer, size_t buffer_size) const noexcept
    {
        size_t rbytes = ::fread(buffer, 1, buffer_size, (FILE*) _native);
        return rbytes;
    }

    FST_NODISCARD size_t file::size() const noexcept
    {
        long prev = ftell((FILE*) _native);
        fseek((FILE*) _native, 0L, SEEK_END);

        long sz = ftell((FILE*) _native);
        fseek((FILE*) _native, prev, SEEK_SET);
        return (size_t) sz;
    }
#endif // __FST__WINDOWS__

FST_END_NAMESPACE
