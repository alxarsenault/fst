//
// MIT License
//
// Copyright (c) 2023 Alexandre Arsenault
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "fst/common.h"
#include "fst/memory.h"
#include "fst/string.h"
#include "fst/utility.h"
#include "fst/small_vector.h"
#include "fst/vector.h"
#include "fst/memory_range.h"
#include "fst/status_code.h"

FST_BEGIN_NAMESPACE

    FST_DECLARE_MEMORY_CATEGORY(network_memory_category, "network");
    using http_string = __fst::basic_string<char, __fst::allocator<char, __fst::network_memory_category>>;
    using http_wstring = __fst::basic_string<wchar_t, __fst::allocator<wchar_t, __fst::network_memory_category>>;

    // http status
    enum class http_status_code {
        invalid = 0,

        // informational.
        scontinue = 100,
        switching_protocols = 101,
        processing = 102,
        early_hints = 103,

        // sucess.
        ok = 200,
        created = 201,
        accepted = 202,
        non_authoritative_information = 203,
        no_content = 204,
        reset_content = 205,
        partial_content = 206,
        multi_status = 207,
        already_reported = 208,
        im_used = 226,

        // redirection.
        multiple_choices = 300,
        moved_permanently = 301,
        found = 302,
        see_other = 303,
        not_modified = 304,
        use_proxy = 305,
        temporary_redirect = 307,
        permanent_redirect = 308,

        // client errors.
        bad_request = 400,
        unauthorized = 401,
        payment_required = 402,
        forbidden = 403,
        not_found = 404,
        method_not_allowed = 405,
        not_acceptable = 406,
        proxy_authentication_required = 407,
        request_timeout = 408,
        conflict = 409,
        gone = 410,
        length_required = 411,
        precondition_failed = 412,
        payload_too_large = 413,
        uri_too_long = 414,
        unsupported_media_type = 415,
        range_not_satisfiable = 416,
        expectation_failed = 417,
        im_a_teapot = 418,
        misdirected_request = 421,
        unprocessable_entity = 422,
        locked = 423,
        failed_dependency = 424,
        too_early = 425,
        upgrade_required = 426,
        precondition_required = 428,
        too_many_requests = 429,
        request_header_fields_too_large = 431,
        unavailable_for_legal_reasons = 451,

        // server errors.
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503,
        gateway_timeout = 504,
        http_version_not_supported = 505,
        variant_also_negotiates = 506,
        insufficient_storage = 507,
        loop_detected = 508,
        not_extended = 510,
        network_authentication_required = 511
    };

    ///
    ///
    ///
    struct http_status
    {
        http_status_code code = http_status_code::invalid;

        http_status() noexcept = default;
        inline http_status(int http_status) noexcept;
        inline http_status(http_status_code c) noexcept;

        ~http_status() noexcept = default;

        http_status(const http_status&) noexcept = default;
        http_status(http_status&&) noexcept = default;

        http_status& operator=(const http_status&) noexcept = default;
        http_status& operator=(http_status&&) noexcept = default;

        ///
        inline explicit operator bool() const noexcept { return !is_error(); }

        ///
        inline bool operator==(http_status_code c) const noexcept { return code == c; }

        ///
        inline bool operator!=(http_status_code c) const noexcept { return code != c; }

        ///
        inline bool is_informational() const noexcept;

        ///
        inline bool is_success() const noexcept;

        ///
        inline bool is_redirection() const noexcept;

        ///
        inline bool is_client_error() const noexcept;

        ///
        inline bool is_server_error() const noexcept;

        ///
        inline bool is_error() const noexcept;

        ///
        inline const char* message() const noexcept;
    };

    //
    // MARK: Status implementation
    //

    http_status::http_status(int status) noexcept
        : code(static_cast<http_status_code>(status))
    {}

    http_status::http_status(http_status_code c) noexcept
        : code(c)
    {}

    bool http_status::is_informational() const noexcept
    {
        return code >= http_status_code::scontinue && code < http_status_code::ok;
    }

    bool http_status::is_success() const noexcept
    {
        return code >= http_status_code::ok && code < http_status_code::multiple_choices;
    }

    bool http_status::is_redirection() const noexcept
    {
        return code >= http_status_code::multiple_choices && code < http_status_code::bad_request;
    }

    bool http_status::is_client_error() const noexcept
    {
        return code >= http_status_code::bad_request && code < http_status_code::internal_server_error;
    }

    bool http_status::is_server_error() const noexcept
    {
        return code >= http_status_code::internal_server_error;
    }

    bool http_status::is_error() const noexcept
    {
        return code >= http_status_code::bad_request || code == http_status_code::invalid;
    }

    const char* http_status::message() const noexcept
    {
        switch (code)
        {
        case http_status_code::invalid: return "Invalid status code";

        // Informational.
        case http_status_code::scontinue: return "Continue";
        case http_status_code::switching_protocols: return "Switching protocols";
        case http_status_code::processing: return "Processing";
        case http_status_code::early_hints: return "Early hints";

        // Success.
        case http_status_code::ok: return "OK";
        case http_status_code::created: return "Created";
        case http_status_code::accepted: return "Accepted";
        case http_status_code::non_authoritative_information: return "Non-Authoritative information";
        case http_status_code::no_content: return "No content";
        case http_status_code::reset_content: return "Reset content";
        case http_status_code::partial_content: return "Partial content";
        case http_status_code::multi_status: return "Multi-Status";
        case http_status_code::already_reported: return "Already reported";
        case http_status_code::im_used: return "IM used";

        // Redirection.
        case http_status_code::multiple_choices: return "Multiple choices";
        case http_status_code::moved_permanently: return "Moved permanently";
        case http_status_code::found: return "Found";
        case http_status_code::see_other: return "See other";
        case http_status_code::not_modified: return "Not modified";
        case http_status_code::use_proxy: return "Use proxy";
        case http_status_code::temporary_redirect: return "Temporary redirect";
        case http_status_code::permanent_redirect: return "Permanent redirect";

        // Client errors.
        case http_status_code::bad_request: return "Bad request";
        case http_status_code::unauthorized: return "Unauthorized";
        case http_status_code::payment_required: return "Payment required";
        case http_status_code::forbidden: return "Forbidden";
        case http_status_code::not_found: return "Not found";
        case http_status_code::method_not_allowed: return "Method not allowed";
        case http_status_code::not_acceptable: return "Not acceptable";
        case http_status_code::proxy_authentication_required: return "Proxy authentication required";
        case http_status_code::request_timeout: return "Request timeout";
        case http_status_code::conflict: return "Conflict";
        case http_status_code::gone: return "Gone";
        case http_status_code::length_required: return "Length required";
        case http_status_code::precondition_failed: return "Precondition failed";
        case http_status_code::payload_too_large: return "Payload too large";
        case http_status_code::uri_too_long: return "URI too long";
        case http_status_code::unsupported_media_type: return "Unsupported media type";
        case http_status_code::range_not_satisfiable: return "Range not satisfiable";
        case http_status_code::expectation_failed: return "Expectation failed";
        case http_status_code::im_a_teapot: return "I'm a teapot";
        case http_status_code::misdirected_request: return "Misdirected request";
        case http_status_code::unprocessable_entity: return "Unprocessable entity";
        case http_status_code::locked: return "Locked";
        case http_status_code::failed_dependency: return "Failed dependency";
        case http_status_code::too_early: return "Too early";
        case http_status_code::upgrade_required: return "Upgrade required";
        case http_status_code::precondition_required: return "Precondition required";
        case http_status_code::too_many_requests: return "Too many requests";
        case http_status_code::request_header_fields_too_large: return "Request header fields too large";
        case http_status_code::unavailable_for_legal_reasons: return "Unavailable for legal reasons";

        // Server errors.
        case http_status_code::internal_server_error: return "Internal server error";
        case http_status_code::not_implemented: return "Not implemented";
        case http_status_code::bad_gateway: return "Bad gateway";
        case http_status_code::service_unavailable: return "Service unavailable";
        case http_status_code::gateway_timeout: return "Gateway timeout";
        case http_status_code::http_version_not_supported: return "HTTP version not supported";
        case http_status_code::variant_also_negotiates: return "Variant also negotiates";
        case http_status_code::insufficient_storage: return "Insufficient storage";
        case http_status_code::loop_detected: return "Loop detected";
        case http_status_code::not_extended: return "Not extended";
        case http_status_code::network_authentication_required: return "Network authentication required";

        default: return "Invalid status code";
        }
    }

    template <class charT>
    inline __fst::output_stream<charT>& operator<<(__fst::output_stream<charT>& stream, http_status s)
    {
        return stream << s.message();
    }

    // Types

    class http_response;

    enum class http_method {
        GET,
        POST,
        PUT,
        PATCH,
        DELETE
    };

    ///
    enum class http_scheme {
        http,
        https
    };

    ///
    struct http_parameter
    {
        using string_type = http_string;

        string_type name;
        string_type value;
    };

    struct http_header_field
    {
        using string_type = http_string;

        string_type name;
        string_type value;
    };

    // URL

    class http_url
    {
      public:
        static constexpr const char* k_http_scheme = "http://";
        static constexpr const char* k_https_scheme = "https://";

        using string_type = typename http_parameter::string_type;
        using parameter_vector = __fst::small_vector<http_parameter, 4, alignof(http_parameter), __fst::default_memory_zone, __fst::network_memory_category>;

        inline http_url(const char* uri)
            : _uri(uri)
        {}

        inline http_url(__fst::string_view uri)
            : _uri(uri)
        {}

        inline http_url(string_type&& uri) noexcept
            : _uri(__fst::move(uri))
        {}

        inline http_url(const string_type& uri)
            : _uri(uri)
        {}

        // MARK: URL

        inline string_type get_string(bool withScheme, bool withParameters) const
        {
            string_type u = withScheme ? _uri : _uri.substr(get_scheme().size());

            if (withParameters) { return u + get_parameters_string(); }

            return u;
        }

        inline __fst::string_view get_string(bool withScheme) const
        {
            return withScheme ? __fst::string_view(_uri) : __fst::string_view(_uri).substr(get_scheme().size());
        }

        /// Returns the domain uri.
        /// This 'https://www.abc.com/info' would return 'www.abc.com'.
        inline __fst::string_view get_domain() const
        {
            __fst::string_view domain = get_string(false);

            __fst::string_view::size_type pos = domain.find_first_of('/');
            if (pos == __fst::string_view::npos) { return domain; }

            return domain.substr(0, pos);
        }

        /// Returns the uri route.
        /// This 'https://www.abc.com/info' would return '/info'.
        inline __fst::string_view get_route() const
        {
            __fst::string_view u = _uri.view().substr(get_scheme().size());
            __fst::string_view::size_type pos = u.find_first_of('/');
            return pos == __fst::string_view::npos ? __fst::string_view() : u.substr(pos);
        }

        /// Returns the uri route.
        /// This 'https://www.abc.com/info' would return '/info'.
        inline string_type get_route(bool withParameters) const { return withParameters ? string_type(get_route()) + get_parameters_string() : string_type(get_route()); }

        // MARK: Scheme

        inline bool has_scheme() const noexcept { return find_end_of_scheme(_uri.view()) != 0; }

        /// Get the uri scheme.
        /// This 'https://www.abc.com/info' would return 'https://'.
        inline __fst::string_view get_scheme() const noexcept { return _uri.view().substr(0, find_end_of_scheme(_uri.view())); }

        inline http_url& with_scheme(http_scheme sc)
        {
            set_scheme(sc);
            return *this;
        }

        inline void set_scheme(http_scheme sc) { _uri = (sc == http_scheme::http ? k_http_scheme : k_https_scheme) + get_string(false, false); }

        inline bool is_http() const noexcept { return _uri.find(k_http_scheme) == 0; }

        inline bool is_https() const noexcept { return _uri.find(k_https_scheme) == 0; }

        // MARK: Parameters

        inline http_url& with_parameter(http_parameter&& param)
        {
            _parameters.push_back(__fst::move(param));
            return *this;
        }

        inline http_url& with_parameter(const http_parameter& param)
        {
            _parameters.push_back(param);
            return *this;
        }

        inline void add_parameter(http_parameter&& param, bool allowDuplicates = true)
        {

            if (allowDuplicates)
            {
                _parameters.push_back(__fst::move(param));
                return;
            }

            for (http_parameter& p : _parameters)
            {
                if (p.name == param.name)
                {
                    p = __fst::move(param);
                    return;
                }
            }

            _parameters.push_back(__fst::move(param));
        }

        inline void add_parameter(const http_parameter& param, bool allowDuplicates = true)
        {
            if (allowDuplicates)
            {
                _parameters.push_back(param);
                return;
            }

            for (http_parameter& p : _parameters)
            {
                if (p.name == param.name)
                {
                    p = param;
                    return;
                }
            }

            _parameters.push_back(param);
        }

        inline bool has_parameters() const noexcept { return !_parameters.empty(); }

        inline size_t parameter_count() const noexcept { return _parameters.size(); }

        inline bool has_parameter(__fst::string_view name) const noexcept
        {
            for (const http_parameter& p : _parameters)
            {
                if (p.name == name) { return true; }
            }

            return false;
        }

        inline bool remove_parameter(__fst::string_view name)
        {
            for (size_t i = 0; i < _parameters.size(); i++)
            {
                if (_parameters[i].name == name)
                {
                    _parameters.erase(_parameters.begin() + i);
                    return true;
                }
            }

            return false;
        }

        inline const http_parameter* get_parameter(__fst::string_view name) const noexcept
        {
            for (const http_parameter& p : _parameters)
            {
                if (p.name == name) { return &p; }
            }

            return nullptr;
        }

        inline string_type get_parameters_string() const
        {
            if (_parameters.empty()) { return ""; }

            size_t reserve_length = 0;
            for (const http_parameter& p : _parameters)
            {
                reserve_length += p.name.size() + p.value.size() + 2;
            }

            string_type params;
            params.reserve(reserve_length);
            params.push_back('?');
            params.append(_parameters[0].name);
            params.push_back('=');
            params.append(_parameters[0].value);

            for (size_t i = 1; i < _parameters.size(); i++)
            {
                params.push_back('&');
                params.append(_parameters[i].name);
                params.push_back('=');
                params.append(_parameters[i].value);
            }

            return params;
        }

        inline parameter_vector& get_parameters() noexcept { return _parameters; }

        inline const parameter_vector& get_parameters() const noexcept { return _parameters; }

        http_response send(bool with_header = true) const noexcept;

      private:
        string_type _uri;
        parameter_vector _parameters;

        static inline size_t find_end_of_scheme(__fst::string_view uri)
        {
            size_t i = 0;

            while (::isalnum(static_cast<int>(uri[i])) || uri[i] == '+' || uri[i] == '-' || uri[i] == '.')
            {
                i++;
            }

            const __fst::string_view key = "://";
            return ((uri).substr(i).substr(0, key.size()) == key) ? i + 3 : 0;
        }
    };

    template <class charT>
    inline __fst::output_stream<charT>& operator<<(__fst::output_stream<charT>& stream, const http_url& u)
    {
        return stream << u.get_string(true, true);
    }

    /// @class request
    class http_request
    {
      public:
        using string_type = typename http_parameter::string_type;
        using header_field_vector = __fst::vector<http_header_field, alignof(http_header_field), __fst::network_memory_category>;
        using data_vector = __fst::vector<char, alignof(char), __fst::network_memory_category>;

        inline http_request(const http_url& u)
            : _url(u)
        {}

        // MARK: URL

        inline http_request& with_url(http_url&& u)
        {
            _url = __fst::move(u);
            return *this;
        }

        inline http_request& with_url(const http_url& u)
        {
            _url = u;
            return *this;
        }

        inline void set_url(http_url&& u) { _url = __fst::move(u); }

        inline void set_url(const http_url& u) { _url = u; }

        inline http_url& get_url() noexcept { return _url; }

        inline const http_url& get_url() const noexcept { return _url; }

        // MARK: Method

        ///
        inline http_request& with_method(http_method m) noexcept
        {
            _method = m;
            return *this;
        }

        ///
        inline void set_method(http_method m) noexcept { _method = m; }

        ///
        inline http_method get_method() const noexcept { return _method; }

        // MARK: Header

        ///
        inline http_request& with_header(http_header_field&& field)
        {
            _headers.push_back(__fst::move(field));
            return *this;
        }

        ///
        inline http_request& with_header(const http_header_field& field)
        {
            _headers.push_back(field);
            return *this;
        }

        ///
        inline void add_header(http_header_field&& field) { _headers.push_back(__fst::move(field)); }

        ///
        inline void add_header(const http_header_field& field) { _headers.push_back(field); }

        ///
        inline bool has_headers() const noexcept { return !_headers.empty(); }

        inline bool has_header(__fst::string_view name) const noexcept
        {
            for (const http_header_field& h : _headers)
            {
                if (h.name == name) { return true; }
            }

            return false;
        }

        inline bool remove_header(__fst::string_view name)
        {
            for (size_t i = 0; i < _headers.size(); i++)
            {
                if (_headers[i].name == name)
                {
                    _headers.erase(_headers.begin() + i);
                    return true;
                }
            }

            return false;
        }

        inline const http_header_field* get_header(__fst::string_view name) const noexcept
        {
            for (const http_header_field& h : _headers)
            {
                if (h.name == name) { return &h; }
            }

            return nullptr;
        }

        ///
        inline header_field_vector& get_headers() noexcept { return _headers; }

        ///
        inline const header_field_vector& get_headers() const noexcept { return _headers; }

        /// Returns the domain uri.
        /// This 'https://www.abc.com/info' would return 'www.abc.com'.
        inline __fst::string_view get_domain() const { return _url.get_domain(); }

        /// Returns the uri route.
        /// This 'https://www.abc.com/info' would return '/info'.
        inline __fst::string_view get_route() const { return _url.get_route(); }

        /// Returns the uri route.
        /// This 'https://www.abc.com/info' would return '/info'.
        inline string_type get_route(bool withParameters) const { return _url.get_route(withParameters); }

        // MARK: Scheme

        inline bool has_scheme() const noexcept { return _url.has_scheme(); }

        /// Get the uri scheme.
        /// This 'https://www.abc.com/info' would return 'https://'.
        inline __fst::string_view get_scheme() const noexcept { return _url.get_scheme(); }

        inline http_request& with_scheme(http_scheme sc)
        {
            _url.set_scheme(sc);
            return *this;
        }

        inline void set_scheme(http_scheme sc) { _url.set_scheme(sc); }

        inline bool is_http() const noexcept { return _url.is_http(); }

        inline bool is_https() const noexcept { return _url.is_https(); }

        // MARK: Parameters

        inline http_request& with_parameter(http_parameter&& param)
        {
            _url.add_parameter(__fst::move(param));
            return *this;
        }

        inline http_request& with_parameter(const http_parameter& param)
        {
            _url.add_parameter(param);
            return *this;
        }

        inline void add_parameter(http_parameter&& param, bool allowDuplicates = true) { _url.add_parameter(__fst::move(param), allowDuplicates); }

        inline void add_parameter(const http_parameter& param, bool allowDuplicates = true) { _url.add_parameter(param, allowDuplicates); }

        inline bool has_parameters() const noexcept { return _url.has_parameters(); }

        inline size_t parameter_count() const noexcept { return _url.parameter_count(); }

        inline bool has_parameter(__fst::string_view name) const noexcept { return _url.has_parameter(name); }

        inline bool remove_parameter(__fst::string_view name) { return _url.remove_parameter(name); }

        inline const http_parameter* get_parameter(__fst::string_view name) const noexcept { return _url.get_parameter(name); }

        inline string_type get_parameters_string() const { return _url.get_parameters_string(); }

        inline __fst::http_url::parameter_vector& get_parameters() noexcept { return _url.get_parameters(); }

        inline const __fst::http_url::parameter_vector& get_parameters() const noexcept { return _url.get_parameters(); }

        inline void set_body(const data_vector& __data) noexcept { _data = __data; }

        inline void set_body(data_vector&& __data) noexcept { _data = __fst::move(__data); }

        inline void set_body(__fst::string_view __data) noexcept
        {
            _data.resize(__data.size());
            __fst::memcpy(_data.data(), __data.data(), __data.size());
        }

        ///
        inline bool has_body() const noexcept { return !_data.empty(); }

        ///
        inline data_vector& body() noexcept { return _data; }

        ///
        inline const data_vector& body() const noexcept { return _data; }

        ///
        inline __fst::string_view body_as_string() const noexcept { return __fst::string_view(_data.data(), _data.size()); }

        ///
        __fst::http_response send(bool with_header = true) const noexcept;

      private:
        __fst::http_url _url;
        header_field_vector _headers;
        data_vector _data;
        __fst::http_method _method = __fst::http_method::GET;
    };

    class http_response
    {
      public:
        using data_vector = __fst::vector<char, alignof(char), __fst::network_memory_category>;
        using header_field_vector = __fst::vector<http_header_field, alignof(http_header_field), __fst::network_memory_category>;

        http_response() = default;
        http_response(const http_response&) = default;
        http_response(http_response&&) = default;

        inline http_response(__fst::status_code err)
            : _error(err)
        {}

        inline http_response(int statusCode, __fst::status_code err)
            : _status(statusCode)
            , _error(err)
        {}

        inline http_response(data_vector&& data, int statusCode, __fst::status_code err)
            : _data(__fst::move(data))
            , _status(statusCode)
            , _error(err)
        {}

        inline http_response(const data_vector& data, int statusCode, __fst::status_code err)
            : _data(data)
            , _status(statusCode)
            , _error(err)
        {}

        inline http_response(const data_vector& data, int statusCode, header_field_vector&& headers)
            : _data(data)
            , _status(statusCode)
            , _error(__fst::status_code::success)
            , _headers(__fst::move(headers))
        {}

        ~http_response() = default;

        http_response& operator=(const http_response&) = default;
        http_response& operator=(http_response&&) = default;

        ///
        inline http_status status() const noexcept { return _status; }

        ///
        inline __fst::error_result error() const noexcept { return _error; }

        ///
        inline bool valid() const noexcept { return _error.valid(); }

        ///
        inline explicit operator bool() const noexcept { return valid(); }

        // MARK: Data

        ///
        inline bool has_data() const noexcept { return !_data.empty(); }

        ///
        inline data_vector& data() noexcept { return _data; }

        ///
        inline const data_vector& data() const noexcept { return _data; }

        //
        inline __fst::byte_range bytes() noexcept { return __fst::byte_range((__fst::byte*) _data.data(), _data.size()); }

        //
        inline __fst::byte_view bytes() const noexcept { return __fst::byte_view((const __fst::byte*) _data.data(), _data.size()); }

        ///
        inline __fst::string_view as_string() const noexcept { return __fst::string_view(_data.data(), _data.size()); }

        inline bool has_header(__fst::string_view name) const noexcept
        {
            for (const http_header_field& h : _headers)
            {
                if (h.name == name) { return true; }
            }

            return false;
        }

        inline const http_header_field* get_header(__fst::string_view name) const noexcept
        {
            for (const http_header_field& h : _headers)
            {
                if (h.name == name) { return &h; }
            }

            return nullptr;
        }

        inline const __fst::string_view get_header_value(__fst::string_view name) const noexcept
        {
            for (const http_header_field& h : _headers)
            {
                if (h.name == name) { return h.value.view(); }
            }

            return __fst::string_view();
        }

        ///
        inline const header_field_vector& get_headers() const noexcept { return _headers; }

      private:
        data_vector _data;
        http_status _status;
        __fst::error_result _error;
        header_field_vector _headers;
    };
FST_END_NAMESPACE
