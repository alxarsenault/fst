#include "fst/file_view.h"
#include "fst/memory.h"

#include <errno.h>
#include <stdio.h>

FST_PRAGMA_PUSH()
FST_PRAGMA_DISABLE_WARNING_CLANG("-Wunused-macros")

#undef __FST_FILE_VIEW_USE_WINDOWS_MEMORY_MAP
#undef __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP

#if defined(_WIN32)
#define __FST_FILE_VIEW_USE_WINDOWS_MEMORY_MAP 1
#define __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP 0

#elif __FST_UNISTD__
#define __FST_FILE_VIEW_USE_WINDOWS_MEMORY_MAP 0
#include <unistd.h>

#ifdef HAVE_MMAP
#define __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP HAVE_MMAP
#elif _POSIX_VERSION >= 199506L
#define __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP 1
#else
#define __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP 0
#endif

// Copy to memory.
#else
#define __FST_FILE_VIEW_USE_WINDOWS_MEMORY_MAP 0
#define __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP 0
#endif

#if __FST_FILE_VIEW_USE_WINDOWS_MEMORY_MAP
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#elif __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#endif

FST_BEGIN_NAMESPACE

    namespace
    {
        struct file_view_impl
        {

#if __FST_FILE_VIEW_USE_WINDOWS_MEMORY_MAP
            static __fst::status open(const char* file_path, uint8_t*& _data, size_t& _size) noexcept
            {
                HANDLE hFile = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

                if (hFile == INVALID_HANDLE_VALUE)
                {
                    switch (GetLastError())
                    {
                    case ERROR_FILE_NOT_FOUND: return __fst::status_code::no_such_file_or_directory;
                    case ERROR_PATH_NOT_FOUND: return __fst::status_code::no_such_file_or_directory;
                    case ERROR_ACCESS_DENIED: return __fst::status_code::permission_denied;
                    default: return __fst::status_code::bad_file_descriptor;
                    }
                }

                DWORD file_size = GetFileSize(hFile, nullptr);
                if (file_size == INVALID_FILE_SIZE || file_size == 0)
                {
                    CloseHandle(hFile);
                    return __fst::status_code::unknown;
                }

                HANDLE hMap = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, file_size, nullptr);
                if (!hMap)
                {
                    CloseHandle(hFile);
                    return __fst::status_code::unknown;
                }

                uint8_t* ptr = (uint8_t*) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, file_size);

                // We can call CloseHandle here, but it will not be closed until we unmap the view.
                CloseHandle(hMap);
                CloseHandle(hFile);
                _data = ptr;
                _size = (size_t) file_size;

                return __fst::status_code::success;
            }

            static inline void close(uint8_t*& _data, size_t& _size) noexcept
            {
                __fst::unused(_size);
                UnmapViewOfFile(_data);
            }

//
// mmap
//
#elif __FST_FILE_VIEW_USE_POSIX_MEMORY_MAP
            static __fst::status open(const char* file_path, uint8_t*& _data, size_t& _size) noexcept
            {
                int fd = ::open(file_path, O_RDONLY);
                if (fd < 0) { return static_cast<__fst::status_code>(errno); }

                // Get file size.
                off_t size = lseek(fd, 0, SEEK_END);
                if (size <= 0)
                {
                    __fst::status_code ec = static_cast<__fst::status_code>(errno);
                    ::close(fd);
                    return ec;
                }

                // Create file map.
                uint8_t* data = (uint8_t*) mmap(nullptr, (size_t) size, PROT_READ, MAP_PRIVATE, fd, 0);

                if (data == MAP_FAILED)
                {
                    __fst::status_code ec = static_cast<__fst::status_code>(errno);
                    ::close(fd);
                    return ec;
                }

                ::close(fd);
                _data = data;
                _size = (size_t) size;
                return __fst::status_code::success;
                ;
            }

            static inline void close(uint8_t*& _data, size_t& _size) noexcept { munmap(_data, _size); }

//
// Using c FILE*
//
#else
            static __fst::status open(const char* file_path, uint8_t*& _data, size_t& _size) noexcept
            {
                FILE* fd = nullptr;

#ifdef _WIN32
                {
                    errno_t err;
                    if ((err = fopen_s(&fd, file_path, "rb")) != 0) { return static_cast<__fst::status_code>(err); }
                }
#else
                fd = ::fopen(file_path, "rb");
                if (!fd) { return static_cast<__fst::status_code>(errno); }
#endif // _WIN32

                // Get file size.
                ::fseek(fd, 0, SEEK_END);
                ptrdiff_t __size = ::ftell(fd);
                if (__size <= 0)
                {
                    __fst::status_code ec = static_cast<__fst::status_code>(errno);
                    ::fclose(fd);
                    return ec;
                }

                ::rewind(fd);
                uint8_t* __data = (uint8_t*) __fst::allocate(__size);

                if (!__data)
                {
                    __fst::status_code ec = static_cast<__fst::status_code>(errno);
                    ::fclose(fd);
                    return ec;
                }

                // Copy content into data.
                // std::fread returns the number of objects read successfully.
                if (::fread(__data, __size, 1, fd) != 1)
                {
                    __fst::status_code ec = static_cast<__fst::status_code>(errno);
                    __fst::deallocate(__data);

                    ::fclose(fd);
                    return ec;
                }

                _data = __data;
                _size = (size_t) __size;
                return __fst::status_code::success;
            }

            static inline void close(uint8_t*& _data, size_t&) noexcept { __fst::deallocate(_data); }
#endif
        };
    } // namespace

    FST_NODISCARD __fst::status file_view::open(const char* file_path) noexcept
    {
        close();
        return file_view_impl::open(file_path, _data, _size);
    }

    void file_view::close() noexcept
    {
        if (_data) { file_view_impl::close(_data, _size); }

        _data = nullptr;
        _size = 0;
    }
FST_END_NAMESPACE
FST_PRAGMA_POP()
