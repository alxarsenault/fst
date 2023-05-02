///
/// MIT License
///
/// Copyright (c) 2023 Alexandre Arsenault
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#pragma once

#include "fst/common.h"
#include "fst/memory.h"
#include "fst/string_view.h"
#include "fst/tuple.h"

#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    namespace
    {
        namespace stream_detail
        {
            struct stream_space_t
            {};
            struct stream_endl_t
            {};
            struct tab_t
            {};
            struct dash_t
            {};
            struct comma_t
            {};
            struct equal_t
            {};
            struct sep_t
            {};
            struct sep_endl_t
            {};

            template <class _CharT, class _UIntT>
            inline _CharT* uint_to_buff(_CharT* ptr, _UIntT value)
            {
                static_assert(_FST::is_unsigned_v<_UIntT>, "_UIntT must be unsigned");

#if __FST_64_BIT__
                auto value_trunc = value;
#else // ^^^ _WIN64 ^^^ // vvv !_WIN64 vvv

                // For 64-bit numbers, work in chunks to avoid 64-bit divisions.
                if constexpr (sizeof(_UIntT) > 4)
                {
                    while (value > 0xFFFFFFFFU)
                    {
                        auto value_chunk = static_cast<unsigned long>(value % 1000000000);
                        value /= 1000000000;

                        for (int _Idx = 0; _Idx != 9; ++_Idx)
                        {
                            *--ptr = static_cast<_CharT>('0' + value_chunk % 10);
                            value_chunk /= 10;
                        }
                    }
                }

                auto value_trunc = static_cast<unsigned long>(value);
#endif // __FST_64_BIT__

                do
                {
                    *--ptr = static_cast<_CharT>('0' + value_trunc % 10);
                    value_trunc /= 10;
                } while (value_trunc != 0);
                return ptr;
            }

            template <class _CharT, class _UIntT>
            inline _CharT* uint_to_buff_hex(_CharT* ptr, _UIntT value)
            {
                static_assert(_FST::is_unsigned_v<_UIntT>, "_UIntT must be unsigned");

                uint64_t value_trunc = value;
                do
                {
                    auto vt = value_trunc % 16;

                    if (vt < 10) { *--ptr = static_cast<_CharT>('0' + vt); }
                    else { *--ptr = static_cast<_CharT>('A' + vt - 10); }
                    value_trunc /= 16;
                } while (value_trunc != 0);
                *--ptr = 'x';
                *--ptr = '0';
                return ptr;
            }
        } // namespace stream_detail
    } // namespace

    FST_INLINE_VAR constexpr stream_detail::stream_endl_t endl = stream_detail::stream_endl_t{};
    FST_INLINE_VAR constexpr stream_detail::stream_space_t space = stream_detail::stream_space_t{};
    FST_INLINE_VAR constexpr stream_detail::sep_t separator = stream_detail::sep_t{};
    FST_INLINE_VAR constexpr stream_detail::sep_endl_t separator_endl = stream_detail::sep_endl_t{};
    FST_INLINE_VAR constexpr stream_detail::tab_t tab = stream_detail::tab_t{};
    FST_INLINE_VAR constexpr stream_detail::comma_t comma = stream_detail::comma_t{};
    FST_INLINE_VAR constexpr stream_detail::dash_t dash = stream_detail::dash_t{};

    enum class term_color {
        reset = 0,
        bold = 1,
        underline = 4,
        inverse = 7,
        bold_off = 21,
        underline_off = 24,
        inverse_off = 27,

        black = 30,
        red = 31,
        green = 32,
        yellow = 33,
        blue = 34,
        magenta = 35,
        cyan = 36,
        white = 37,

        bg_black = 40,
        bg_red = 41,
        bg_green = 42,
        bg_yellow = 43,
        bg_blue = 44,
        bg_magenta = 45,
        bg_cyan = 46,
        bg_white = 47
    };

    FST_INLINE_VAR constexpr term_color bold = term_color::bold;
    FST_INLINE_VAR constexpr term_color underline = term_color::underline;
    FST_INLINE_VAR constexpr term_color reset = term_color::reset;

    FST_INLINE_VAR constexpr term_color black = term_color::black;
    FST_INLINE_VAR constexpr term_color red = term_color::red;
    FST_INLINE_VAR constexpr term_color green = term_color::green;
    FST_INLINE_VAR constexpr term_color yellow = term_color::yellow;
    FST_INLINE_VAR constexpr term_color blue = term_color::blue;
    FST_INLINE_VAR constexpr term_color magenta = term_color::magenta;
    FST_INLINE_VAR constexpr term_color cyan = term_color::cyan;
    FST_INLINE_VAR constexpr term_color white = term_color::white;

    enum class stream_modifier {

        reset = 0,
        bold = 1,
        underline = 4,
        inverse = 7,
        bold_off = 21,
        underline_off = 24,
        inverse_off = 27,

        black = 30,
        red = 31,
        green = 32,
        yellow = 33,
        blue = 34,
        magenta = 35,
        cyan = 36,
        white = 37,

        bg_black = 40,
        bg_red = 41,
        bg_green = 42,
        bg_yellow = 43,
        bg_blue = 44,
        bg_magenta = 45,
        bg_cyan = 46,
        bg_white = 47,

        endl = 800,
        tab,
        space,
        empty,
        equal,
        comma,
        dash,
        sep,
        normal
    };

    template <class _T>
    struct is_stream_modifier : _FST::false_t
    {};
    template <>
    struct is_stream_modifier<_FST::term_color> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::stream_endl_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::stream_space_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::tab_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::comma_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::equal_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::sep_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::sep_endl_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::empty_t> : _FST::true_t
    {};
    template <>
    struct is_stream_modifier<_FST::stream_detail::dash_t> : _FST::true_t
    {};

    template <class _CharT>
    class output_stream
    {
      public:
        using write_fct = void (*)(void*, const _CharT*, size_t, stream_modifier);

        inline constexpr output_stream(void* data, write_fct w) noexcept
            : _write(w)
            , _data(data)
        {}

        write_fct _write;
        void* _data;

        inline void write(const _CharT* str, size_t size, stream_modifier mod = stream_modifier::normal) const noexcept { _write(_data, str, size, mod); }

        inline output_stream& operator<<(bool _Val) noexcept
        {
            const _CharT t[4] = { (_CharT) 't', (_CharT) 'r', (_CharT) 'u', (_CharT) 'e' };
            const _CharT f[5] = { (_CharT) 'f', (_CharT) 'a', (_CharT) 'l', (_CharT) 's', (_CharT) 'e' };
            write(_Val ? t : f, (size_t) (5 - _Val, stream_modifier::normal));
            return *this;
        }

        template <class T, _FST::enable_if_t<_FST::is_function_v<T> || _FST::is_pointer_v<T>, int> = 0>
        inline output_stream& operator<<(const T& v) noexcept
        {
            _CharT buffer[21];
            const _CharT* begin = stream_detail::uint_to_buff_hex(&buffer[0] + 21, (uintptr_t) v);
            write(begin, size_t(&buffer[0] + 21 - &buffer[0]), stream_modifier::normal);
            return *this;
        }
        inline output_stream& operator<<(term_color c) noexcept
        {
            _CharT s[] = { '\033', '[', '0', '0', 'm' };
            stream_detail::uint_to_buff(&s[4], (uint16_t) c);
            write(s, sizeof(s), stream_modifier(c));
            return *this;
        }

        inline output_stream& operator<<(stream_detail::stream_endl_t) noexcept
        {
            _CharT c = (_CharT) '\n';
            write(&c, 1, stream_modifier::endl);

            return *this;
        }

        inline output_stream& operator<<(stream_detail::stream_space_t) noexcept
        {
            _CharT c = (_CharT) ' ';
            write(&c, 1, stream_modifier::space);
            return *this;
        }

        inline output_stream& operator<<(stream_detail::tab_t) noexcept
        {
            _CharT c = (_CharT) '\t';
            write(&c, 1, stream_modifier::tab);
            return *this;
        }

        inline output_stream& operator<<(stream_detail::comma_t) noexcept
        {
            const _CharT s[] = { (_CharT) ',', (_CharT) ' ' };
            write(s, sizeof(s), stream_modifier::comma);
            return *this;
        }

        inline output_stream& operator<<(stream_detail::equal_t) noexcept
        {
            const _CharT s[] = { (_CharT) ' ', (_CharT) '=', (_CharT) ' ' };
            write(s, sizeof(s), stream_modifier::equal);
            return *this;
        }

        inline output_stream& operator<<(stream_detail::dash_t) noexcept
        {
            const _CharT s[] = { (_CharT) ' ', (_CharT) '-', (_CharT) ' ' };
            write(s, sizeof(s), stream_modifier::dash);
            return *this;
        }
        inline output_stream& operator<<(stream_detail::sep_t) noexcept
        {
            const _CharT s[] = { (_CharT) ' ', (_CharT) '|', (_CharT) ' ' };
            write(s, sizeof(s), stream_modifier::sep);
            return *this;
        }

        inline output_stream& operator<<(stream_detail::sep_endl_t) noexcept
        {
            const _CharT s[] = { (_CharT) ' ', (_CharT) '|', (_CharT) '\n' };
            write(s, sizeof(s), stream_modifier::sep);
            return *this;
        }

        inline output_stream& operator<<(empty_t) noexcept { return *this; }

        inline output_stream& operator<<(const _CharT* str) noexcept
        {
            fst_assert(str);
            write(str, _FST::char_traits<_CharT>::length(str), stream_modifier::normal);
            return *this;
        }

        inline output_stream& operator<<(_CharT* str) noexcept
        {
            fst_assert(str);
            write(str, _FST::char_traits<_CharT>::length(str), stream_modifier::normal);
            return *this;
        }

        output_stream& operator<<(_FST::nullptr_t) noexcept
        {
            const _CharT s[] = { 'n', 'u', 'l', 'l' };
            write(s, sizeof(s), stream_modifier::normal);
            return *this;
        }

        template <class C = char, _FST::enable_if_t<!_FST::is_same_v<C, _CharT> && _FST::is_same_v<_CharT, wchar_t>, int> = 0>
        inline output_stream& operator<<(const char* str) noexcept
        {
            fst_assert(str);
            const size_t size = _FST::strlen(str);
            write(str, size);
            return *this;
        }

        template <class C = char, _FST::enable_if_t<!_FST::is_same_v<C, _CharT> && _FST::is_same_v<_CharT, wchar_t>, int> = 0>
        inline output_stream& operator<<(char* str) noexcept
        {
            fst_assert(str);
            const size_t size = _FST::strlen(str);
            write(str, size);
            return *this;
        }

        inline output_stream& operator<<(char c) noexcept
        {
            _CharT cc = (_CharT) c;
            write(&cc, 1, stream_modifier::normal);
            return *this;
        }

        template <class C = _CharT, _FST::enable_if_t<!_FST::is_same_v<C, char>, int> = 0>
        inline output_stream& operator<<(_CharT c) noexcept
        {
            write(&c, 1, stream_modifier::normal);
            return *this;
        }

        template <class _T, _FST::enable_if_t<_FST::is_integral_v<_T>, int> = 0>
        output_stream& operator<<(_T value) noexcept
        {
            _CharT buffer[21];

            if constexpr (_FST::is_unsigned_v<_T>)
            {
                const _CharT* begin = stream_detail::uint_to_buff(&buffer[0] + 21, value);
                write(begin, size_t(&buffer[0] + 21 - begin), stream_modifier::normal);
            }
            else
            {
                if (value < 0)
                {
                    _CharT* begin = stream_detail::uint_to_buff(&buffer[0] + 21, 0 - static_cast<_FST::make_unsigned_t<_T>>(value));
                    *--begin = (_CharT) '-';
                    write(begin, size_t(&buffer[0] + 21 - begin), stream_modifier::normal);
                }
                else
                {
                    _CharT* begin = stream_detail::uint_to_buff(&buffer[0] + 21, static_cast<_FST::make_unsigned_t<_T>>(value));
                    write(begin, size_t(&buffer[0] + 21 - begin), stream_modifier::normal);
                }
            }

            return *this;
        }

        template <size_t I = 0, typename... _Ts>
        inline void print_tuple(const _FST::tuple<_Ts...>& t)
        {

            *this << t.template get<I>();

            if constexpr (I < sizeof...(_Ts) - 1) { print_tuple<I + 1, _Ts...>(*this << ", ", t); }
        }

        template <class _T, _FST::enable_if_t<_FST::is_floating_point_v<_T>, int> = 0>
        output_stream& operator<<(_T value) noexcept
        {
            _CharT buffer[33];
            int len = _FST::snprintf(&buffer[0], 32, "%g", (double) value);
            write(&buffer[0], (size_t) len, stream_modifier::normal);
            return *this;
        }

        template <class _T, _FST::enable_if_t<is_c_array_v<_T>, int> = 0>
        output_stream& operator<<(const _T& value) noexcept
        {
            const size_t size = _FST::strlen(value);
            write(value, size);
            return *this;
        }

        template <class _T, _FST::enable_if_t<is_iterable<_T>::value && !_FST::is_c_array_v<_T> && !_FST::is_string_view_convertible<_T>::value, int> = 0>
        output_stream& operator<<(const _T& value) noexcept
        {
            *this << "{";
            const size_t size = value.size();
            size_t count = 0;
            for (auto n = value.begin(); n != value.end(); ++n, ++count)
            {
                *this << *n;
                *this << (count == size - 1 ? "" : ", ");
            }
            *this << "}";
            return *this;
        }
    };

    template <class _CharT>
    class output_stream_imp
    {
      public:
        bool has_color = false;
        inline output_stream_imp() {}

        inline output_stream<_CharT> get_stream()
        {
            return output_stream<_CharT>(
                this, [](void* data, const _CharT* str, size_t size, stream_modifier mod) { ((output_stream_imp*) data)->write(str, size, mod); });
        }

        inline void write(const _CharT* str, size_t size, stream_modifier mod)
        {
            if (mod < stream_modifier::endl)
            {
                has_color = mod != stream_modifier::reset;
                _FST::write_stdout(str, size);
            }
            else if (mod == stream_modifier::endl && has_color)
            {
                has_color = false;
                const _CharT s[] = { '\033', '[', '0', 'm', '\n' };
                _FST::write_stdout(s, sizeof(s));
            }
            else { _FST::write_stdout(str, size); }
        }
    };

    FST_INLINE_VAR output_stream<char> cout = { nullptr, [](void*, const char* str, size_t size, stream_modifier mod)
        {
            static bool has_color = false;

            if (mod < stream_modifier::endl) { has_color = mod != stream_modifier::reset; }
            else if (mod == stream_modifier::endl && has_color)
            {
                has_color = false;
                str = "\033[0m\n";
                size = 5;
            }

            _FST::write_stdout(str, size);
        } };

    FST_INLINE_VAR output_stream<wchar_t> wcout = { nullptr, [](void*, const wchar_t* str, size_t size, stream_modifier)
        {
            _FST::write_wstdout(str, size);
        } };

    template <typename T, typename _Stream>
    inline void print_element(_Stream & stream, const T& t);

    template <size_t I = 0, typename... _Ts, typename _Stream>
    inline void print_tuple(_Stream & stream, const _FST::tuple<_Ts...>& t)
    {
        print_element(stream, t.template get<I>());

        if constexpr (I < sizeof...(_Ts) - 1) { print_tuple<I + 1, _Ts...>(stream << ", ", t); }
    }

    template <typename T, typename _Stream>
    inline void print_element(_Stream & stream, const T& t)
    {
        if constexpr (has_stream_operator<_Stream, T>::value) { stream << t; }
        else if constexpr (is_iterable<T>::value)
        {
            stream << "{";
            const size_t size = t.size();
            size_t count = 0;
            for (auto n = t.begin(); n != t.end(); ++n, ++count)
            {
                print_element(stream, *n);
                stream << (count == size - 1 ? "" : ", ");
            }
            stream << "}";
        }
        else if constexpr (is_pair<T>::value)
        {
            stream << "{";
            print_element(stream, t.first());
            stream << ", ";
            print_element(stream, t.second());
            stream << "}";
        }
        else if constexpr (_FST::is_tuple<T>::value)
        {
            print_tuple(stream << "{", t);
            stream << "}";
        }
        else
        {
            if constexpr (_FST::is_class_v<T>) { stream << "unknown class"; }
            else if constexpr (_FST::is_object_v<T>) { stream << "unknown object"; }
            else { stream << "unknown"; }
        }
    }

    template <class _Stream, typename D = stream_detail::comma_t, class E = _FST::stream_detail::stream_endl_t, typename T, typename... Ts>
    inline void basic_print(_Stream & stream, const T& t, const Ts&... ts)
    {
        if constexpr (sizeof...(ts) > 0)
        {
            print_element(stream, t);
            if constexpr (_FST::is_stream_modifier<T>::value) { basic_print<_Stream, D, E>(stream, ts...); }
            else { basic_print<_Stream, D, E>(stream << D{}, ts...); }
        }
        else
        {
            print_element(stream, t);
            stream << E{};
        }
    }

    template <typename D = _FST::stream_detail::comma_t, class E = _FST::stream_detail::stream_endl_t, typename T, typename... Ts>
    inline void print(const T& t, const Ts&... ts)
    {
        _FST::output_stream<char>& stream = _FST::cout;
        if constexpr (sizeof...(ts) > 0)
        {
            print_element(stream, t);

            if constexpr (_FST::is_stream_modifier<T>::value) { basic_print<_FST::output_stream<char>, D, E>(stream, ts...); }
            else { basic_print<_FST::output_stream<char>, D, E>(stream << D{}, ts...); }
        }
        else
        {
            print_element(stream, t);
            stream << E{};
        }
    }

    template <class T>
    struct space_padding
    {
        inline constexpr space_padding(size_t size, T value) noexcept
            : _size(size)
            , _value(value)
        {}

        friend inline _FST::output_stream<char>& operator<<(_FST::output_stream<char>& stream, const space_padding& t)
        {
            struct content
            {
                char buffer[1024];
                size_t size;
            };

            content _content;
            _FST::output_stream<char> s{ &_content, [](void* data, const char* str, size_t size, stream_modifier)
                {
                    content* c = (content*) data;
                    c->size = size;
                    _FST::memcpy(c->buffer, str, size);
                } };

            s << t._value;

            stream.write(_content.buffer, _content.size);
            if (t._size > _content.size)
            {
                const char* space_buffer = "                                                       ";
                stream.write(space_buffer, t._size - _content.size);
            }
            return stream;
        }

        size_t _size;
        T _value;
    };

    template <class T>
    space_padding(size_t, T) -> space_padding<T>;

    template <size_t _Size, class T>
    struct fspace_padding
    {
        fspace_padding() = delete;
        fspace_padding(const fspace_padding&) = delete;
        fspace_padding(fspace_padding&&) = delete;

        fspace_padding& operator=(const fspace_padding&) = delete;
        fspace_padding& operator=(fspace_padding&&) = delete;

        inline constexpr fspace_padding(T value) noexcept
            : _value(value)
        {}

        friend inline _FST::output_stream<char>& operator<<(_FST::output_stream<char>& stream, const fspace_padding& t)
        {
            struct content
            {

                char buffer[1024];
                size_t size;
            };

            content _content;
            _FST::output_stream<char> s{ &_content, [](void* data, const char* str, size_t size, stream_modifier)
                {
                    content* c = (content*) data;
                    c->size = size;
                    _FST::memcpy(c->buffer, str, size);
                } };

            s << t._value;

            stream.write(_content.buffer, _content.size);
            if (_Size > _content.size)
            {
                const char* space_buffer = "                                                       ";
                stream.write(space_buffer, _Size - _content.size);
            }
            return stream;
        }

        T _value;
    };

    template <size_t _Size, class T>
    struct fspace_padding_right
    {
        fspace_padding_right() = delete;
        fspace_padding_right(const fspace_padding_right&) = delete;
        fspace_padding_right(fspace_padding_right&&) = delete;

        fspace_padding_right& operator=(const fspace_padding_right&) = delete;
        fspace_padding_right& operator=(fspace_padding_right&&) = delete;

        inline constexpr fspace_padding_right(T value) noexcept
            : _value(value)
        {}

        friend inline _FST::output_stream<char>& operator<<(_FST::output_stream<char>& stream, const fspace_padding_right& t)
        {
            struct content
            {

                char buffer[1024];
                size_t size;
            };

            content _content;
            _FST::output_stream<char> s{ &_content, [](void* data, const char* str, size_t size, stream_modifier)
                {
                    content* c = (content*) data;
                    c->size = size;
                    _FST::memcpy(c->buffer, str, size);
                } };

            s << t._value;

            if (_Size > _content.size)
            {
                const char* space_buffer = "                                                       ";
                stream.write(space_buffer, _Size - _content.size);
            }

            stream.write(_content.buffer, _content.size);

            return stream;
        }

        T _value;
    };

    template <size_t _Size, class T>
    inline fspace_padding<_Size, T> padded(T && arg) noexcept
    {
        return fspace_padding<_Size, T>(_FST::forward<T>(arg));
    }

    template <size_t _Size, class T>
    inline fspace_padding_right<_Size, T> padded_right(T && arg) noexcept
    {
        return fspace_padding_right<_Size, T>(_FST::forward<T>(arg));
    }

FST_END_NAMESPACE