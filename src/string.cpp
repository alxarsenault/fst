#include "fst/string.h"

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>

FST_BEGIN_NAMESPACE

    //__fst::wstring widen_string(__fst::string_view input) noexcept
    //{
    //    if (input.empty()) { return __fst::wstring(); }

    //    int required_length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(), (int) input.size(), nullptr, 0);
    //    if (required_length > 0)
    //    {
    //        __fst::wstring output(static_cast<size_t>(required_length), L'\0');

    //        if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input.data(), (int) input.size(), &output[0], required_length) > 0) { return output; }
    //    }

    //    // Failed to convert string from UTF-8 to UTF-16
    //    return __fst::wstring();
    //}

    //// Converts a wide (UTF-16-encoded) string into a narrow (UTF-8-encoded) string.
    //__fst::string narrow_string(__fst::wstring_view input) noexcept
    //{
    //    if (input.empty()) { return __fst::string(); }

    //    const UINT cp = CP_UTF8;
    //    const DWORD flags = WC_ERR_INVALID_CHARS;

    //    const int required_length = WideCharToMultiByte(cp, flags, input.data(), static_cast<int>(input.size()), nullptr, 0, nullptr, nullptr);
    //    if (required_length > 0)
    //    {
    //        __fst::string output(static_cast<size_t>(required_length), '\0');

    //        if (WideCharToMultiByte(cp, flags, input.data(), static_cast<int>(input.size()), &output[0], required_length, nullptr, nullptr) > 0) { return output; }
    //    }

    //    // Failed to convert string from UTF-16 to UTF-8
    //    return __fst::string();
    //}

FST_END_NAMESPACE
