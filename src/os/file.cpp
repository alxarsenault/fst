#include "fst/os/file.h"

#if __FST_WINDOWS__
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>

#include "fst/unicode.h"
#endif // __FST__WINDOWS__

FST_BEGIN_SUB_NAMESPACE(os)
    namespace
    {
        using wstring_type = __fst::basic_string<wchar_t, __fst::memory_zone_allocator<wchar_t, __fst::default_memory_zone, __fst::os_memory_category>>;
    }

    struct file::native
    {
        struct overlapped_content : OVERLAPPED
        {
            file::native* self;
        };

        HANDLE _handle = nullptr;
        HANDLE _event = nullptr;
        overlapped_content _overlap = {};

        __fst::status open(const __fst::os::path& fpath, __fst::os::file::flags flgs) noexcept
        {
            wstring_type wpath = __fst::utf_cvt(fpath.get_string());

            DWORD dwShareMode = 0;
            if ((flgs & __fst::os::file::flags::read) != 0) { dwShareMode |= GENERIC_READ; }
            if ((flgs & __fst::os::file::flags::write) != 0) { dwShareMode |= GENERIC_WRITE; }

            DWORD dwCreationDisposition = 0;
            if ((flgs & __fst::os::file::flags::create_always) != 0) { dwCreationDisposition |= CREATE_ALWAYS; }
            if ((flgs & __fst::os::file::flags::create_new) != 0) { dwCreationDisposition |= CREATE_NEW; }
            if ((flgs & __fst::os::file::flags::open_always) != 0) { dwCreationDisposition |= OPEN_ALWAYS; }
            if ((flgs & __fst::os::file::flags::open_existing) != 0) { dwCreationDisposition |= OPEN_EXISTING; }
            if ((flgs & __fst::os::file::flags::truncate_existing) != 0) { dwCreationDisposition |= TRUNCATE_EXISTING; }

            _handle = CreateFileW(wpath.c_str(), dwShareMode, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, dwCreationDisposition, FILE_FLAG_OVERLAPPED, 0);

            if (_handle == INVALID_HANDLE_VALUE)
            {
                printf("Unable to create file! Error: %u\n", GetLastError());
                return __fst::status_code::unknown;
            }

            _event = CreateEventW(nullptr, true, false, nullptr);
            if (!_event)
            {
                printf("Unable to create wait event! Error: %u\n", GetLastError());
                CloseHandle(_handle);
                DeleteFileW(wpath.c_str());
                return __fst::status_code::unknown;
            }

            _overlap.hEvent = _event;
            _overlap.self = this;

            printf("Writing to file (AddrOverlapped: %p)\n", &_overlap);

            if (!WriteFileEx(_handle, "ABCDEF", 6, &_overlap, &CompletionRoutine))
            {
                printf("Unable to write to file! Error: %u\n", GetLastError());
                CloseHandle(_handle);
                DeleteFileW(wpath.c_str());
                return __fst::status_code::unknown;
            }

            printf("Waiting for write to complete\n");

            DWORD dwResult;
            do
            {
                dwResult = WaitForSingleObjectEx(_overlap.hEvent, 5000, TRUE);

                if (dwResult == WAIT_OBJECT_0)
                {
                    printf("Write Completed\n");
                    break;
                }

                if (dwResult != WAIT_IO_COMPLETION)
                {
                    if (dwResult == WAIT_TIMEOUT)
                        printf("Timeout waiting for write to complete!\n");
                    else
                        printf("Unable to wait for write to complete!\n");

                    CancelIo(_handle);
                    break;
                }
            } while (true);

            printf("Finished waiting\n");

            CloseHandle(_handle);
            CloseHandle(_event);

            printf("Exit\n");

            return __fst::status();
        }

        __fst::status close() noexcept { return __fst::status_code::success; }

        static void WINAPI CompletionRoutine(DWORD u32_ErrorCode, DWORD u32_BytesTransfered, LPOVERLAPPED pk_Overlapped)
        {
            //native::overlapped_content* ct = (native::overlapped_content*) pk_Overlapped;

            if (u32_ErrorCode != 0)
                printf("CompletionRoutine: Unable to write to file! Error: %u, AddrOverlapped: %p\n", u32_ErrorCode, pk_Overlapped);
            else
                printf("CompletionRoutine: Transferred: %u Bytes, AddrOverlapped: %p\n", u32_BytesTransfered, pk_Overlapped);

            SetEvent(pk_Overlapped->hEvent);
        }
    };

    namespace
    {
        template <class... Args>
        inline file::native* allocate_native_file(Args&&... args)
        {
            file::native* ptr = (file::native*) __fst::allocate<__fst::os_memory_category>(sizeof(file::native));
            if (ptr)
            {
                fst_placement_new(ptr) file::native(__fst::forward<Args>(args)...);
                return ptr;
            }

            return nullptr;
        }

    } // namespace

    file::~file()
    {

        if (_native)
        {
            _native->~native();
            __fst::deallocate<__fst::os_memory_category>(_native);
        }
    }

    __fst::status file::open(const __fst::os::path& fpath, __fst::os::file::flags flgs) noexcept
    {
        if (!_native) { _native = allocate_native_file(); }
        if (!_native) { return __fst::status_code::unknown; }

        __fst::status st = _native->open(fpath, flgs);
        if (!st) { return st; }

        return __fst::status();
    }

    __fst::status file::close() noexcept
    {
        return _native ? _native->close() : __fst::status_code::unknown;
    }

FST_END_SUB_NAMESPACE
