#include "fst/path.h"
#include "fst/string.h"
#include "fst/unicode.h"

#if __FST_WINDOWS__ && __FST_DEBUG_BUILD__
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // __FST_WINDOWS__

FST_BEGIN_NAMESPACE
    //
    namespace filesystem
    {
        namespace
        {

#if __FST_WINDOWS__
            namespace win32
            {
                using string_type = __fst::basic_small_string<wchar_t, 1024>;
                inline string_type create_wstring(__fst::string_view name) noexcept
                {
                    return __fst::utf_cvt(name);
                }

                bool is_directory(__fst::wstring_range p) noexcept
                {
                    __fst::wstring_range::scoped_null ns = p.null_scope();

                    DWORD dwAttrib = GetFileAttributesW(p.data());
                    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
                }

                __fst::wstring_range get_parent_directory(__fst::wstring_range p)
                {
                    size_t last = __fst::wstring_range::npos;
                    for (__fst::wstring_view it : __fst::utf::iterate(p))
                    {
                        if (it == (wchar_t) separator) { last = (size_t) (it.data() - p.data()); }
                    }

                    if (last != __fst::wstring_range::npos) return p.substr(0, last);

                    return p;
                }

                __fst::status create_directory(__fst::wstring_range name, bool exist_is_success) noexcept
                {
                    // If the function succeeds, the return value is nonzero.
                    //wchar_t pp = name.null_last();
                    __fst::wstring_range::scoped_null ns = name.null_scope();

                    if (CreateDirectoryW(name.data(), nullptr)) { return __fst::status_code::success; }

                    DWORD err = GetLastError();
                    if (err == ERROR_ALREADY_EXISTS) { return exist_is_success ? __fst::status_code::success : __fst::status_code::already_created; }
                    else if (err == ERROR_PATH_NOT_FOUND) { return __fst::status_code::no_such_file_or_directory; }
                    // TODO: convert error
                    return __fst::status_code::unknown;
                }

                __fst::status delete_file(__fst::wstring_range name) noexcept
                {
                    __fst::wstring_range::scoped_null ns = name.null_scope();

                    // If the function succeeds, the return value is nonzero.
                    if (DeleteFileW(name.data())) { return __fst::status_code::success; }

                    DWORD err = GetLastError();
                    if (err == ERROR_PATH_NOT_FOUND) { return __fst::status_code::no_such_file_or_directory; }
                    else if (err == ERROR_ACCESS_DENIED) { return __fst::status_code::operation_not_permitted; }
                    // TODO: convert error
                    //
                    // TODO: convert error
                    return __fst::status_code::unknown;
                }

                __fst::status delete_directory(__fst::wstring_range name) noexcept
                {
                    __fst::wstring_range::scoped_null ns = name.null_scope();

                    // If the function succeeds, the return value is nonzero.
                    if (RemoveDirectoryW(name.data())) { return __fst::status_code::success; }

                    DWORD err = GetLastError();
                    if (err == ERROR_PATH_NOT_FOUND) { return __fst::status_code::no_such_file_or_directory; }
                    else if (err == ERROR_ACCESS_DENIED) { return __fst::status_code::operation_not_permitted; }
                    // TODO: convert error
                    //
                    // TODO: convert error
                    return __fst::status_code::unknown;
                }

                __fst::status create_directories(__fst::wstring_range name) noexcept
                {
                    if (win32::is_directory(name)) { return __fst::status_code::success; }

                    __fst::error_result err = win32::create_directories(win32::get_parent_directory(name));
                    return err ? err.code : win32::create_directory(name, true);
                }
            } // namespace win32

#endif // __FST_WINDOWS__
        } // namespace
        //using wstring_type = __fst::basic_string<wchar_t, __fst::memory_zone_allocator<wchar_t, __fst::default_memory_category, __fst::default_memory_zone>>;

        __fst::string_view get_parent_directory(__fst::string_view p) noexcept
        {
            size_t last = __fst::string_view::npos;
            for (__fst::string_view it : __fst::utf::iterate(p))
            {
                if (it == separator) { last = (size_t) (it.data() - p.data()); }
            }

            if (last != __fst::string_view::npos) return p.substr(0, last);

            return p;
        }

        bool is_directory(__fst::string_view name) noexcept
        {
#if __FST_WINDOWS__
            win32::string_type wname = win32::create_wstring(name);
            return win32::is_directory(wname);
#else
            return false;
#endif
        }

        __fst::status create_directory(__fst::string_view name, bool exist_is_success) noexcept
        {
#if __FST_WINDOWS__
            win32::string_type wname = win32::create_wstring(name);
            return win32::create_directory(wname, exist_is_success);
#else
            return __fst::status_code::not_supported;
#endif
        }

        __fst::status create_directories(__fst::string_view name) noexcept
        {
#if __FST_WINDOWS__
            win32::string_type wname = win32::create_wstring(name);
            return win32::create_directories(wname);
#else
            return __fst::status_code::not_supported;
#endif
        }

        __fst::status delete_file(__fst::string_view name) noexcept
        {
#if __FST_WINDOWS__
            win32::string_type wname = win32::create_wstring(name);
            return win32::delete_file(wname);
#else
            return __fst::status_code::not_supported;
#endif
        }

        __fst::status delete_directory(__fst::string_view name) noexcept
        {
#if __FST_WINDOWS__
            win32::string_type wname = win32::create_wstring(name);
            return win32::delete_directory(wname);
#else
            return __fst::status_code::not_supported;
#endif
        }
    } // namespace filesystem

FST_END_NAMESPACE
