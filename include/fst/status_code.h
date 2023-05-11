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
#include "fst/traits.h"

FST_BEGIN_NAMESPACE

    enum class status_code {
        success = 0,
        unknown = -1,

        operation_not_permitted = 1, // EPERM
        no_such_file_or_directory = 2, // ENOENT
        no_such_process = 3, // ESRCH
        interrupted = 4, // EINTR
        io_error = 5, // EIO
        no_such_device_or_address = 6, // ENXIO
        argument_list_too_long = 7, // E2BIG
        executable_format_error = 8, // ENOEXEC
        bad_file_descriptor = 9, // EBADF
        no_child_process = 10, // ECHILD
        resource_unavailable_try_again = 11, // EAGAIN
        not_enough_memory = 12, // ENOMEM
        permission_denied = 13, // EACCES
        bad_address = 14, // EFAULT
        device_or_resource_busy = 16, // EBUSY
        file_exists = 17, // EEXIST
        cross_device_link = 18, // EXDEV
        no_such_device = 19, // ENODEV
        not_a_directory = 20, // ENOTDIR
        is_a_directory = 21, // EISDIR
        invalid_argument = 22, // EINVAL
        too_many_files_open_in_system = 23, // ENFILE
        too_many_files_open = 24, // EMFILE
        inappropriate_io_control_operation = 25, // ENOTTY
        file_too_large = 27, // EFBIG
        no_space_on_device = 28, // ENOSPC
        invalid_seek = 29, // ESPIPE
        read_only_file_system = 30, // EROFS
        too_many_links = 31, // EMLINK
        broken_pipe = 32, // EPIPE
        argument_out_of_domain = 33, // EDOM
        result_out_of_range = 34, // ERANGE
        resource_deadlock_would_occur = 36, // EDEADLK
        filename_too_long = 38, // ENAMETOOLONG
        no_lock_available = 39, // ENOLCK
        function_not_supported = 40, // ENOSYS
        directory_not_empty = 41, // ENOTEMPTY
        illegal_byte_sequence = 42, // EILSEQ
        address_in_use = 100, // EADDRINUSE
        address_not_available = 101, // EADDRNOTAVAIL
        address_family_not_supported = 102, // EAFNOSUPPORT
        connection_already_in_progress = 103, // EALREADY
        bad_message = 104, // EBADMSG
        operation_canceled = 105, // ECANCELED
        connection_aborted = 106, // ECONNABORTED
        connection_refused = 107, // ECONNREFUSED
        connection_reset = 108, // ECONNRESET
        destination_address_required = 109, // EDESTADDRREQ
        host_unreachable = 110, // EHOSTUNREACH
        identifier_removed = 111, // EIDRM
        operation_in_progress = 112, // EINPROGRESS
        already_connected = 113, // EISCONN
        too_many_symbolic_link_levels = 114, // ELOOP
        message_size = 115, // EMSGSIZE
        network_down = 116, // ENETDOWN
        network_reset = 117, // ENETRESET
        network_unreachable = 118, // ENETUNREACH
        no_buffer_space = 119, // ENOBUFS
        no_message_available = 120, // ENODATA
        no_link = 121, // ENOLINK
        no_message = 122, // ENOMSG
        no_protocol_option = 123, // ENOPROTOOPT
        no_stream_resources = 124, // ENOSR
        not_a_stream = 125, // ENOSTR
        not_connected = 126, // ENOTCONN
        state_not_recoverable = 127, // ENOTRECOVERABLE
        not_a_socket = 128, // ENOTSOCK
        not_supported = 129, // ENOTSUP
        operation_not_supported = 130, // EOPNOTSUPP
        value_too_large = 132, // EOVERFLOW
        owner_dead = 133, // EOWNERDEAD
        protocol_error = 134, // EPROTO
        protocol_not_supported = 135, // EPROTONOSUPPORT
        wrong_protocol_type = 136, // EPROTOTYPE
        stream_timeout = 137, // ETIME
        timed_out = 138, // ETIMEDOUT
        text_file_busy = 139, // ETXTBSY
        operation_would_block = 140, // EWOULDBLOCK

        already_created,
        invalid_file_format,
        invalid_file_content,
        invalid_channel_size,
        invalid_audio_format,

        invalid
    };

    struct error_result;

    /// @struct status
    struct status
    {
        status_code code = status_code::success;

        inline constexpr status(status_code c) noexcept
            : code(c)
        {}

        constexpr status() noexcept = default;
        constexpr status(const status&) noexcept = default;
        constexpr status(status&&) noexcept = default;

        ~status() noexcept = default;

        constexpr status& operator=(const status&) noexcept = default;
        constexpr status& operator=(status&&) noexcept = default;

        /// Returns false on error.
        FST_NODISCARD inline constexpr explicit operator bool() const noexcept { return code == status_code::success; }

        ///
        FST_NODISCARD inline constexpr bool valid() const noexcept { return code == status_code::success; }

        FST_NODISCARD inline constexpr bool operator==(status c) const noexcept { return code == c.code; }
        FST_NODISCARD inline constexpr bool operator!=(status c) const noexcept { return code != c.code; }
        FST_NODISCARD inline constexpr bool operator==(error_result er) const noexcept;
        FST_NODISCARD inline constexpr bool operator!=(error_result er) const noexcept;

        ///
        FST_NODISCARD inline constexpr const char* message() const noexcept
        {
            switch (code)
            {
            case status_code::success: return "success";
            case status_code::unknown: return "unknown";
            case status_code::invalid: return "invalid";

            case status_code::address_family_not_supported: return "address_family_not_supported";
            case status_code::address_in_use: return "address_in_use";
            case status_code::address_not_available: return "address_not_available";
            case status_code::already_connected: return "already_connected";
            case status_code::argument_list_too_long: return "argument_list_too_long";
            case status_code::argument_out_of_domain: return "argument_out_of_domain";
            case status_code::bad_address: return "bad_address";
            case status_code::bad_file_descriptor: return "bad_file_descriptor";
            case status_code::bad_message: return "bad_message";
            case status_code::broken_pipe: return "broken_pipe";
            case status_code::connection_aborted: return "connection_aborted";
            case status_code::connection_already_in_progress: return "connection_already_in_progress";
            case status_code::connection_refused: return "connection_refused";
            case status_code::connection_reset: return "connection_reset";
            case status_code::cross_device_link: return "cross_device_link";
            case status_code::destination_address_required: return "destination_address_required";
            case status_code::device_or_resource_busy: return "device_or_resource_busy";
            case status_code::directory_not_empty: return "directory_not_empty";
            case status_code::executable_format_error: return "executable_format_error";
            case status_code::file_exists: return "file_exists";
            case status_code::file_too_large: return "file_too_large";
            case status_code::filename_too_long: return "filename_too_long";
            case status_code::function_not_supported: return "function_not_supported";
            case status_code::host_unreachable: return "host_unreachable";
            case status_code::identifier_removed: return "identifier_removed";
            case status_code::illegal_byte_sequence: return "illegal_byte_sequence";
            case status_code::inappropriate_io_control_operation: return "inappropriate_io_control_operation";
            case status_code::interrupted: return "interrupted";
            case status_code::invalid_argument: return "invalid_argument";
            case status_code::invalid_seek: return "invalid_seek";
            case status_code::io_error: return "io_error";
            case status_code::is_a_directory: return "is_a_directory";
            case status_code::message_size: return "message_size";
            case status_code::network_down: return "network_down";
            case status_code::network_reset: return "network_reset";
            case status_code::network_unreachable: return "network_unreachable";
            case status_code::no_buffer_space: return "no_buffer_space";
            case status_code::no_child_process: return "no_child_process";
            case status_code::no_link: return "no_link";
            case status_code::no_lock_available: return "no_lock_available";
            case status_code::no_message_available: return "no_message_available";
            case status_code::no_message: return "no_message";
            case status_code::no_protocol_option: return "no_protocol_option";
            case status_code::no_space_on_device: return "no_space_on_device";
            case status_code::no_stream_resources: return "no_stream_resources";
            case status_code::no_such_device_or_address: return "no_such_device_or_address";
            case status_code::no_such_device: return "no_such_device";
            case status_code::no_such_file_or_directory: return "no_such_file_or_directory";
            case status_code::no_such_process: return "no_such_process";
            case status_code::not_a_directory: return "not_a_directory";
            case status_code::not_a_socket: return "not_a_socket";
            case status_code::not_a_stream: return "not_a_stream";
            case status_code::not_connected: return "not_connected";
            case status_code::not_enough_memory: return "not_enough_memory";
            case status_code::not_supported: return "not_supported";
            case status_code::operation_canceled: return "operation_canceled";
            case status_code::operation_in_progress: return "operation_in_progress";
            case status_code::operation_not_permitted: return "operation_not_permitted";
            case status_code::operation_not_supported: return "operation_not_supported";
            case status_code::operation_would_block: return "operation_would_block";
            case status_code::owner_dead: return "owner_dead";
            case status_code::permission_denied: return "permission_denied";
            case status_code::protocol_error: return "protocol_error";
            case status_code::protocol_not_supported: return "protocol_not_supported";
            case status_code::read_only_file_system: return "read_only_file_system";
            case status_code::resource_deadlock_would_occur: return "resource_deadlock_would_occur";
            case status_code::resource_unavailable_try_again: return "resource_unavailable_try_again";
            case status_code::result_out_of_range: return "result_out_of_range";
            case status_code::state_not_recoverable: return "state_not_recoverable";
            case status_code::stream_timeout: return "stream_timeout";
            case status_code::text_file_busy: return "text_file_busy";
            case status_code::timed_out: return "timed_out";
            case status_code::too_many_files_open_in_system: return "too_many_files_open_in_system";
            case status_code::too_many_files_open: return "too_many_files_open";
            case status_code::too_many_links: return "too_many_links";
            case status_code::too_many_symbolic_link_levels: return "too_many_symbolic_link_levels";
            case status_code::value_too_large: return "value_too_large";
            case status_code::wrong_protocol_type: return "wrong_protocol_type";

                            case status_code::already_created: return "already_created";
            case status_code::invalid_file_format: return "invalid_file_format";
            case status_code::invalid_file_content: return "invalid_file_content";
            case status_code::invalid_channel_size: return "invalid_channel_size";
            case status_code::invalid_audio_format: return "invalid_audio_format";
            }

            return "unknown";
        }
    };

    /// @struct error_result
    struct error_result
    {
        status_code code = status_code::success;

        inline constexpr error_result(status_code c) noexcept
            : code(c)
        {}

        inline constexpr error_result(status st) noexcept
            : code(st.code)
        {}

        constexpr error_result() noexcept = default;
        constexpr error_result(const error_result&) noexcept = default;
        constexpr error_result(error_result&&) noexcept = default;

        ~error_result() noexcept = default;

        constexpr error_result& operator=(const error_result&) noexcept = default;
        constexpr error_result& operator=(error_result&&) noexcept = default;

        /// Returns true on error.
        FST_NODISCARD inline constexpr explicit operator bool() const noexcept { return code != status_code::success; }

        ///
        FST_NODISCARD inline constexpr bool valid() const noexcept { return code == status_code::success; }

        FST_NODISCARD inline constexpr bool operator==(error_result c) const noexcept { return code == c.code; }
        FST_NODISCARD inline constexpr bool operator!=(error_result c) const noexcept { return code != c.code; }
        FST_NODISCARD inline constexpr bool operator==(status c) const noexcept { return code == c.code; }
        FST_NODISCARD inline constexpr bool operator!=(status c) const noexcept { return code != c.code; }

        ///
        FST_NODISCARD inline constexpr const char* message() const noexcept { return status(code).message(); }
    };

    /// status_ref
    class status_ref
    {
        status_code* _code = nullptr;
        constexpr status_ref() noexcept = default;

      public:
        static inline constexpr status_ref empty() noexcept { return status_ref(); }

        inline constexpr status_ref(status_code& c) noexcept
            : _code(&c)
        {}

        inline constexpr status_ref(status& st) noexcept
            : _code(&st.code)
        {}

        inline constexpr status_ref(error_result& er) noexcept
            : _code(&er.code)
        {}

        constexpr status_ref(const status_ref&) noexcept = default;
        constexpr status_ref(status_ref&&) noexcept = default;

        ~status_ref() noexcept = default;

        constexpr status_ref& operator=(const status_ref&) noexcept = default;
        constexpr status_ref& operator=(status_ref&&) noexcept = default;

        constexpr status_ref& operator=(status_code c) noexcept
        {
            if (_code) *_code = c;
            return *this;
        }

        constexpr status_ref& operator=(status st) noexcept
        {
            if (_code) *_code = st.code;
            return *this;
        }

        constexpr status_ref& operator=(error_result er) noexcept
        {
            if (_code) *_code = er.code;
            return *this;
        }

        FST_NODISCARD inline constexpr status_code code() const noexcept { return _code ? *_code : status_code::invalid; }

        /// Returns true on success.
        FST_NODISCARD inline constexpr explicit operator bool() const noexcept { return _code && *_code == status_code::success; }

        ///
        FST_NODISCARD inline constexpr bool valid() const noexcept { return _code && *_code == status_code::success; }

        FST_NODISCARD inline constexpr bool operator==(status_ref c) const noexcept { return _code && c._code && *_code == *c._code; }
        FST_NODISCARD inline constexpr bool operator!=(status_ref c) const noexcept { return _code && c._code && *_code != *c._code; }
        FST_NODISCARD inline constexpr bool operator==(error_result c) const noexcept { return _code && *_code == c.code; }
        FST_NODISCARD inline constexpr bool operator!=(error_result c) const noexcept { return _code && *_code != c.code; }
        FST_NODISCARD inline constexpr bool operator==(status c) const noexcept { return _code && *_code == c.code; }
        FST_NODISCARD inline constexpr bool operator!=(status c) const noexcept { return _code && *_code != c.code; }

        ///
        FST_NODISCARD inline constexpr const char* message() const noexcept { return status(code()).message(); }
    };

    FST_NODISCARD inline constexpr bool status::operator==(error_result er) const noexcept
    {
        return code == er.code;
    }

    FST_NODISCARD inline constexpr bool status::operator!=(error_result er) const noexcept
    {
        return code != er.code;
    }

    template <class charT>
    inline __fst::output_stream<charT>& operator<<(__fst::output_stream<charT>& stream, __fst::status_code sc)
    {
        return stream << __fst::status(sc).message();
    }

    template <class charT>
    inline __fst::output_stream<charT>& operator<<(__fst::output_stream<charT>& stream, __fst::status st)
    {
        return stream << st.message();
    }

    template <class charT>
    inline __fst::output_stream<charT>& operator<<(__fst::output_stream<charT>& stream, __fst::error_result er)
    {
        return stream << er.message();
    }

    template <class charT>
    inline __fst::output_stream<charT>& operator<<(__fst::output_stream<charT>& stream, __fst::status_ref st)
    {
        return stream << st.message();
    }

FST_END_NAMESPACE

#if FST_USE_STL
#include <ostream>

template <class _CharT, class _Traits>
inline std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& stream, __fst::status_code sc)
{
    return stream << __fst::status(sc).message();
}

template <class _CharT, class _Traits>
inline std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& stream, __fst::status st)
{
    return stream << st.message();
}
#endif
