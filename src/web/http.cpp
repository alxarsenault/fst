#include "fst/web/http.h"
#include "fst/unicode.h"

#if __FST_WINDOWS__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winhttp.h>
//#include <stdio.h>

//#pragma comment(lib, "winhttp.lib")

FST_BEGIN_SUB_NAMESPACE(http)

    void print_win_error()
    {
        char buffer[4096];
        DWORD len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer, sizeof(buffer) / sizeof(char), nullptr);

        if (len) { _FST::print(::GetLastError(), "Error", buffer); }
        else { _FST::print(::GetLastError(), "Error--------"); }
    }

    using web_wstring = _FST::basic_string<wchar_t, _FST::allocator<wchar_t, _FST::web_memory_category>>;
    using web_string = _FST::basic_string<char, _FST::allocator<char, _FST::web_memory_category>>;

    //
    void get_response_headers(HINTERNET hRequest, _FST::http::response::header_field_vector & headers)
    {
        DWORD header_size = 0;
        WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, nullptr, &header_size, WINHTTP_NO_HEADER_INDEX);

        // Allocate memory for the buffer.
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            _FST::basic_string<wchar_t, _FST::allocator<wchar_t, _FST::web_memory_category>> wheader;
            wheader.resize(header_size / sizeof(wchar_t));

            // Now, use WinHttpQueryHeaders to retrieve the header.
            if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, wheader.data(), &header_size, WINHTTP_NO_HEADER_INDEX))
            {
                print_win_error();
                return;
            }

            using string_type = _FST::http::header_field::string_type;
            string_type header_str = _FST::utf_cvt(wheader);

            size_t line_count = _FST::count_lines(header_str.c_str());
            if (line_count > 3)
            {
                line_count -= 3;
                headers.reserve(line_count);
            }

            for (_FST::string_view line : _FST::line_range(_FST::strsplit(header_str, '\n')[1]))
            {
                auto c = _FST::strsplit(line, ':');

                if (c[1].empty()) { return; }
                headers.push_back({ string_type(c[0]), string_type(c[1].substr(1)) });
            }

            //fst::print(__LINE__, header_str);
            //auto c = _FST::strsplit(a, ':');

            /*if (const char* begin = ::strchr(header_str.data(), '\n'))
            {
                const char* end = header_str.cend();

                while (++begin < end)
                {
                    const char* name_split = ::strchr(begin, ':');
                    if (!name_split) { return; }

                    const char* line_end = ::strchr(name_split, '\n');
                    if (!line_end) { return; }

                    headers.push_back({ string_type(begin, name_split - begin), string_type(name_split + 2, line_end - name_split - 3) });
                    begin = line_end;
                }
            }*/
        }
    }

    _FST::http::response url::send(bool with_header) const noexcept
    {

        // WinHTTP Sessions Overview | https://msdn.microsoft.com/en-us/library/windows/desktop/aa384270(v=vs.85).aspx

        // Use WinHttpOpen to obtain a session handle.
        HINTERNET hSession = WinHttpOpen(nullptr, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession)
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        _FST::final_action _close_session([=]() { WinHttpCloseHandle(hSession); });

        // Specify an HTTP server.
        web_wstring domain = _FST::utf_cvt(get_domain());

        HINTERNET hConnect = WinHttpConnect(hSession, domain.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect)
        {

            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        _FST::final_action _close_connect([=]() { WinHttpCloseHandle(hConnect); });

        // Create an HTTP request handle.
        web_wstring route = _FST::utf_cvt(get_route(true));

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", route.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest)
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        _FST::final_action _close_request([=]() { WinHttpCloseHandle(hRequest); });

        //BOOL bResults = FALSE;

        // Send a request.
        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
        {

            switch (::GetLastError())
            {
            case ERROR_WINHTTP_CANNOT_CONNECT: return _FST::http::response(_FST::http::error_code::cannot_connect_to_host);
            case ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED: return _FST::http::response(_FST::http::error_code::client_certificate_required);
            case ERROR_WINHTTP_CONNECTION_ERROR: return _FST::http::response(_FST::http::error_code::cannot_connect_to_host);
            case ERROR_WINHTTP_INCORRECT_HANDLE_STATE: return _FST::http::response(_FST::http::error_code::unknown);
            case ERROR_WINHTTP_INCORRECT_HANDLE_TYPE: return _FST::http::response(_FST::http::error_code::unknown);
            case ERROR_WINHTTP_INTERNAL_ERROR: return _FST::http::response(_FST::http::error_code::unknown);
            case ERROR_WINHTTP_INVALID_URL: return _FST::http::response(_FST::http::error_code::bad_url);
            case ERROR_WINHTTP_LOGIN_FAILURE: return _FST::http::response(_FST::http::error_code::unknown);
            case ERROR_WINHTTP_NAME_NOT_RESOLVED: return _FST::http::response(_FST::http::error_code::unresolved_name);
            case ERROR_WINHTTP_OPERATION_CANCELLED: return _FST::http::response(_FST::http::error_code::cancelled);
            case ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW: return _FST::http::response(_FST::http::error_code::cancelled);
            case ERROR_WINHTTP_SECURE_FAILURE: return _FST::http::response(_FST::http::error_code::secure_connection_failed);
            case ERROR_WINHTTP_SHUTDOWN: return _FST::http::response(_FST::http::error_code::cancelled);
            case ERROR_WINHTTP_TIMEOUT: return _FST::http::response(_FST::http::error_code::timed_out);
            case ERROR_WINHTTP_UNRECOGNIZED_SCHEME: return _FST::http::response(_FST::http::error_code::unknown);
            case ERROR_WINHTTP_RESEND_REQUEST: return _FST::http::response(_FST::http::error_code::unknown);
            }

            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        if (!WinHttpReceiveResponse(hRequest, nullptr))
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        DWORD dwStatusCode = 0;

        // Get status code.
        {
            DWORD dwStatusSize = sizeof(dwStatusCode);
            if (!WinHttpQueryHeaders(
                    hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwStatusSize, WINHTTP_NO_HEADER_INDEX))
            {
                print_win_error();
                return _FST::http::response(_FST::http::error_code::cancelled);
            }
        }

        _FST::http::response::header_field_vector headers;
        if (with_header) { get_response_headers(hRequest, headers); }

        _FST::http::response::data_vector data;

        // Keep checking for data until there is nothing left.
        {
            DWORD dwSize = 0;

            do
            {
                // Check for available data.
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                {
                    print_win_error();
                    //printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
                    return _FST::http::response(_FST::http::error_code::cancelled);
                }

                //
                const size_t data_begin_index = data.size();
                data.resize(data_begin_index + dwSize, 0);
                char* data_ptr = data.data() + data_begin_index;

                {
                    // Read the data.
                    DWORD dwDownloaded = 0;

                    if (!WinHttpReadData(hRequest, (LPVOID) data_ptr, dwSize, &dwDownloaded))
                    {
                        print_win_error();
                        //printf("Error %u in WinHttpReadData.\n", GetLastError());
                        return _FST::http::response(_FST::http::error_code::cancelled);
                    }
                }
            } while (dwSize > 0);
        }

        return _FST::http::response(_FST::move(data), (int) dwStatusCode, _FST::move(headers));
    }

    response request::send(bool with_header) const noexcept
    {
        // Use WinHttpOpen to obtain a session handle.
        HINTERNET hSession = WinHttpOpen(nullptr, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession)
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        _FST::final_action _close_session([=]() { WinHttpCloseHandle(hSession); });

        // Specify an HTTP server.
        web_wstring domain = _FST::utf_cvt(get_domain());

        HINTERNET hConnect = WinHttpConnect(hSession, domain.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect)
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        _FST::final_action _close_connect([=]() { WinHttpCloseHandle(hConnect); });

        // Create an HTTP request handle.
        web_wstring route = _FST::utf_cvt(get_route(true));

        constexpr const wchar_t* method_str[] = { L"GET", L"POST", L"PUT", L"PATCH", L"DELETE" };

        HINTERNET hRequest
            = WinHttpOpenRequest(hConnect, method_str[(int) _method], route.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest)
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        _FST::final_action _close_request([=]() { WinHttpCloseHandle(hRequest); });

        // Headers.
        wchar_t* header_ptr = nullptr;
        web_wstring header_wstr;
        DWORD header_size = 0;

        // Add headers.
        if (!_headers.empty())
        {
            web_string header_str;
            header_str.reserve(1024);

            for (size_t i = 0; i < _headers.size(); i++)
            {
                header_str += _headers[i].name + ": " + _headers[i].value + "\r\n";
            }

            header_wstr = _FST::utf_cvt(header_str);
            header_ptr = header_wstr.data();
            header_size = (DWORD) header_wstr.size();
        }

        // Data.
        web_string data_str;
        void* data_ptr = nullptr;
        DWORD data_size = 0;

        if (!_data.empty())
        {
            const size_t data_str_size = _data.size() + 2;
            data_str.resize(data_str_size);
            data_str[0] = '\r';
            data_str[1] = '\n';
            _FST::memcpy(data_str.data() + 2, _data.data(), _data.size());
            data_size = (DWORD) data_str_size;
            data_ptr = data_str.data();
        }

        // Send a request.
        if (!WinHttpSendRequest(hRequest, header_ptr, header_size, data_ptr, data_size, data_size, 0))
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        if (!WinHttpReceiveResponse(hRequest, nullptr))
        {
            print_win_error();
            return _FST::http::response(_FST::http::error_code::cancelled);
        }

        DWORD dwStatusCode = 0;

        // Get status code.
        {
            DWORD dwStatusSize = sizeof(dwStatusCode);
            if (!WinHttpQueryHeaders(
                    hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwStatusSize, WINHTTP_NO_HEADER_INDEX))
            {
                print_win_error();
                return _FST::http::response(_FST::http::error_code::cancelled);
            }
        }

        _FST::http::response::header_field_vector headers;
        if (with_header) { get_response_headers(hRequest, headers); }

        _FST::http::response::data_vector data;

        // Keep checking for data until there is nothing left.
        {
            DWORD dwSize = 0;

            do
            {
                // Check for available data.
                dwSize = 0;
                if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                {
                    //printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
                    print_win_error();
                    return _FST::http::response(_FST::http::error_code::cancelled);
                }

                //
                const size_t data_begin_index = data.size();
                data.resize(data_begin_index + dwSize, 0);
                char* resp_data_ptr = data.data() + data_begin_index;

                {
                    // Read the data.
                    DWORD dwDownloaded = 0;

                    if (!WinHttpReadData(hRequest, (LPVOID) resp_data_ptr, dwSize, &dwDownloaded))
                    {
                        //printf("Error %u in WinHttpReadData.\n", GetLastError());
                        print_win_error();
                        return _FST::http::response(_FST::http::error_code::cancelled);
                    }
                }
            } while (dwSize > 0);
        }

        return _FST::http::response(_FST::move(data), (int) dwStatusCode, _FST::move(headers));
    }
FST_END_SUB_NAMESPACE

#else
FST_BEGIN_SUB_NAMESPACE(http)

    _FST::http::response url::send(bool with_header) const noexcept
    {
        _FST::unused(with_header);
        return _FST::http::response(_FST::http::error_code::unknown);
    }
    _FST::http::response request::send(bool with_header) const noexcept
    {
        _FST::unused(with_header);
        return _FST::http::response(_FST::http::error_code::unknown);
    }
FST_END_SUB_NAMESPACE

#endif //
