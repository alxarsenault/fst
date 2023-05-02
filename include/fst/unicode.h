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
#include "fst/string.h"
#include "fst/utility.h"
#include "fst/stream.h"
FST_BEGIN_NAMESPACE

    // Forward declared.
    class utf_string_view;

FST_END_NAMESPACE

FST_BEGIN_SUB_NAMESPACE(utf)

    ///
    ///
    ///
    template <typename CharT, typename SType>
    inline auto iterate_as(const SType& str) noexcept;

    ///
    ///
    ///
    template <typename SType, __fst::enable_if_t<__fst::is_utf_string_type<SType>::value, __fst::nullptr_t> = nullptr>
    inline auto iterate(const SType& str) noexcept;

    ///
    ///
    ///
    template <class IteratorType>
    class iterator;

    ///
    ///
    ///
    template <class CharT, class SType, class IteratorType, __fst::enable_if_t<__fst::is_utf_string_type<SType>::value, __fst::nullptr_t> = nullptr>
    class basic_iterator;

    /// Converts any string type to a utf8 __fst::string.
    inline __fst::string to_utf8(__fst::utf_string_view s) noexcept;

    /// Converts any string type to a utf16 __fst::u16string.
    inline __fst::u16string to_utf16(__fst::utf_string_view s) noexcept;

    /// Converts any string type to a utf32 __fst::u32string.
    inline __fst::u32string to_utf32(__fst::utf_string_view s) noexcept;

    /// Converts any string type to a __fst::wstring.
    inline __fst::wstring to_wide(__fst::utf_string_view s) noexcept;

    namespace detail
    {
        template <char_encoding Encoding = char_encoding::utf8>
        struct iterator_sequence_length
        {
            template <typename T>
            static inline constexpr size_t length(T lead) noexcept
            {
                return __fst::utf::sequence_length(static_cast<uint8_t>(lead));
            }
        };

        template <>
        struct iterator_sequence_length<char_encoding::utf16>
        {
            template <typename T>
            static inline constexpr size_t length(T c) noexcept
            {
                return __fst::utf::u16_sequence_length(static_cast<char16_t>(c));
            }
        };

        template <>
        struct iterator_sequence_length<char_encoding::utf32>
        {
            template <typename T>
            static inline constexpr size_t length(T) noexcept
            {
                return 1;
            }
        };

        template <typename InputCharT, typename OutputChart, typename = void>
        struct base_iterator
        {};

        template <typename InputCharT, typename OutputCharT>
        struct base_iterator<InputCharT, OutputCharT, __fst::enable_if_t<__fst::utf_encoding_of<InputCharT>::value == __fst::utf_encoding_of<OutputCharT>::value>>
        {
            using input_char_type = InputCharT;
            using output_char_type = OutputCharT;

            using input_view_type = __fst::basic_string_view<input_char_type>;
            using output_view_type = __fst::basic_string_view<output_char_type>;

            using it_seq_length = detail::iterator_sequence_length<__fst::utf_encoding_of<InputCharT>::value>;

            template <typename Iterator>
            static inline output_view_type get(Iterator it) noexcept
            {
                return output_view_type(reinterpret_cast<const output_char_type*>(&(it[0])), it_seq_length::length(*it));
            }

            template <typename Iterator>
            static inline void advance(Iterator& it) noexcept
            {
                using it_diff_type = typename __fst::iterator_traits<Iterator>::difference_type;
                __fst::advance(it, static_cast<it_diff_type>(it_seq_length::length(*it)));
            }
        };

        template <typename InputCharT, typename OutputCharT>
        struct base_iterator<InputCharT, OutputCharT, __fst::enable_if_t<__fst::utf_encoding_of<InputCharT>::value != utf_encoding_of<OutputCharT>::value>>
        {

            using input_char_type = InputCharT;
            using output_char_type = OutputCharT;

            using input_view_type = __fst::basic_string_view<input_char_type>;
            using output_view_type = __fst::basic_string_view<output_char_type>;

            using it_seq_length = detail::iterator_sequence_length<utf_encoding_of<InputCharT>::value>;

            template <typename Iterator>
            inline output_view_type get(Iterator it) const noexcept
            {
                return output_view_type(_data.begin(),
                    static_cast<size_t>(__fst::distance(_data.begin(), __fst::utf_copy(input_view_type(&it[0], it_seq_length::length(*it)), _data.begin()))));
            }

            template <typename Iterator>
            inline void advance(Iterator& it) noexcept
            {
                using it_diff_type = typename __fst::iterator_traits<Iterator>::difference_type;
                __fst::advance(it, static_cast<it_diff_type>(it_seq_length::length(*it)));
            }

            mutable __fst::array<output_char_type, utf_encoding_to_max_char_count<__fst::utf_encoding_of<OutputCharT>::value>::value> _data;
        };

        template <class IteratorType>
        using iterator_value_type = __fst::remove_cvref_t<decltype(__fst::declval<IteratorType>()[0])>;

        template <class IteratorType>
        using iterator_base_type = basic_iterator<iterator_value_type<IteratorType>, __fst::basic_string_view<iterator_value_type<IteratorType>>, IteratorType>;

        template <typename IteratorT>
        class iterator_range
        {
            IteratorT _begin_iterator;
            IteratorT _end_iterator;

          public:
            template <typename Container>
            inline iterator_range(Container&& c) noexcept
                : _begin_iterator(c.begin())
                , _end_iterator(c.end())
            {}

            inline iterator_range(IteratorT begin_iterator, IteratorT end_iterator) noexcept
                : _begin_iterator(__fst::move(begin_iterator))
                , _end_iterator(__fst::move(end_iterator))
            {}

            inline IteratorT begin() const noexcept { return _begin_iterator; }
            inline IteratorT end() const noexcept { return _end_iterator; }
            inline bool empty() const noexcept { return _begin_iterator == _end_iterator; }
        };

    } // namespace detail.

    //NANO_UNICODE_CLANG_PUSH_WARNING("-Wpadded")

    template <class CharT, class SType, class IteratorType, __fst::enable_if_t<is_utf_string_type<SType>::value, __fst::nullptr_t>>
    class basic_iterator : private detail::base_iterator<__fst::string_char_type_t<SType>, CharT>
    {
        using base_type = detail::base_iterator<__fst::string_char_type_t<SType>, CharT>;
        using output_view_type = typename base_type::output_view_type;

        template <class T>
        friend class iterator;

      public:
        typedef ptrdiff_t difference_type;
        typedef __fst::forward_iterator_tag iterator_category;

        inline basic_iterator() noexcept = default;

        inline explicit basic_iterator(IteratorType it) noexcept
            : _it(it)
        {}

        inline IteratorType base() const noexcept { return _it; }

        inline output_view_type operator*() const noexcept { return base_type::get(_it); }

        inline bool operator==(const basic_iterator& rhs) const noexcept { return (_it == rhs._it); }

        inline bool operator!=(const basic_iterator& rhs) const noexcept { return !(operator==(rhs)); }

        inline basic_iterator& operator++() noexcept
        {
            base_type::advance(_it);
            return *this;
        }

        inline basic_iterator operator++(int) noexcept
        {
            basic_iterator temp = *this;
            base_type::advance(_it);
            return temp;
        }

      private:
        IteratorType _it;
    };
    //NANO_UNICODE_CLANG_POP_WARNING()

    template <typename CharT, typename SType>
    inline auto iterate_as(const SType& str) noexcept
    {
        return detail::iterator_range<basic_iterator<CharT, SType, typename SType::const_iterator>>(str);
    }

    template <typename SType, __fst::enable_if_t<is_utf_string_type<SType>::value, __fst::nullptr_t>>
    inline auto iterate(const SType& str) noexcept
    {
        return detail::iterator_range<basic_iterator<string_char_type_t<SType>, SType, typename SType::const_iterator>>(str);
    }

    template <class IteratorType>
    class iterator : private detail::iterator_base_type<IteratorType>
    {
        using base_type = detail::iterator_base_type<IteratorType>;

      public:
        typedef ptrdiff_t difference_type;
        typedef __fst::forward_iterator_tag iterator_category;

        inline iterator() noexcept = default;

        inline explicit iterator(IteratorType it) noexcept
            : _it(it)
        {}

        inline IteratorType base() const noexcept { return _it; }

        inline typename base_type::output_view_type operator*() const noexcept { return base_type::get(_it); }

        inline bool operator==(const iterator& rhs) const noexcept { return (_it == rhs._it); }

        inline bool operator!=(const iterator& rhs) const noexcept { return !(operator==(rhs)); }

        inline iterator& operator++() noexcept
        {
            base_type::advance(_it);
            return *this;
        }

        inline iterator operator++(int) noexcept
        {
            iterator temp = *this;
            base_type::advance(_it);
            return temp;
        }

      private:
        IteratorType _it;
    };

    template <class IteratorType>
    iterator(IteratorType) -> iterator<IteratorType>;
FST_END_SUB_NAMESPACE

FST_BEGIN_NAMESPACE

    /*!
 * @brief   Generalization of a __fst::basic_string_view that accepts any char type.
 *
 * @details The utf::string_view describes an object that can refer to a constant
 *          contiguous sequence of any char-like objects with the first element of
 *          the sequence at position zero.
 *
 *          This class is intended to be used when you need to support multiple
 *          character types and don't want to write a function for each of them.
 *
 *          It also prevents from creating unnecessary __fst::basic_string
 *          when passing a string literals or a __fst::basic_string_view to a function.
 *
 * @warning Keep in mind that the size of wchar_t is 2 bytes on Windows and 4 bytes
 *          on other platforms.
 *
 * Here's an example on how this could be used for drawing multiple string types (drawText)
 * or how the string_view could be utf_cvted to any string type and help to abstract a
 * cross-platform implementation (setText) :
 *
 * @code
 *   // In example.h
 *   void drawText(utf::string_view text);
 *   void setText(utf::string_view text);
 *
 *   // In example.cpp
 *   void drawText(utf::string_view text) {
 *     switch(text.encoding()) {
 *       case utf::string_view::utf8:
 *         drawUtf8Text(text.u8view());
 *         break;
 *       case utf::string_view::utf16:
 *         drawUtf16Text(text.u16view());
 *         break;
 *       case utf::string_view::utf32:
 *         drawUtf32Text(text.u32view());
 *         break;
 *     }
 *   }
 *
 *   #ifdef _WIN32
 *   void setTextWindows(const __fst::wstring&) { ... }
 *   #else
 *   void setTextMac(const __fst::string&) { ... }
 *   #endif
 *
 *   void setText(utf::string_view text) {
 *     #ifdef _WIN32
 *     setTextWindows(text.to_wide());
 *     #else
 *     setTextMac(text.to_utf8());
 *     #endif
 *   }
 *
 *   // In main.cpp
 *   #include "example.h"
 *
 *   int main() {
 *     __fst::string text = "Text";
 *     __fst::wstring wtext = L"Text";
 *
 *     drawText(text);
 *     drawText(wtext);
 *     drawText("Text");
 *     drawText(L"Text");
 *
 *     setText(text);
 *     setText(wtext);
 *     setText("Text");
 *     setText(L"Text");
 *     return 0;
 *   }
 * @endcode
 */
    class utf_string_view
    {
      public:
        /// Default constructor.
        /// Creates an empty string view of char type (utf8).
        inline utf_string_view() noexcept;

        /// Constructor.
        /// Takes any possible kind of string input:
        ///
        /// Strings:
        /// const __fst::string&
        /// const __fst::u16string&
        /// const __fst::u32string&
        /// const __fst::wstring&
        ///
        /// Views:
        /// __fst::string_view
        /// __fst::u16string_view
        /// __fst::u32string_view
        /// __fst::wstring_view
        ///
        /// Pointers:
        /// const char*
        /// const char16_t*
        /// const char32_t*
        /// const wchar_t*
        ///
        /// Literals:
        /// const char(&)[N]
        /// const char16_t(&)[N]
        /// const char32_t(&)[N]
        /// const wchar_t(&)[N]
        template <typename T, __fst::enable_if_t<is_utf_string_type<remove_cvref_t<T>>::value, __fst::nullptr_t> = nullptr>
        inline utf_string_view(T&& s);

        utf_string_view(utf_string_view&&) noexcept = default;
        utf_string_view(const utf_string_view&) noexcept = default;

        utf_string_view& operator=(utf_string_view&&) noexcept = default;
        utf_string_view& operator=(const utf_string_view&) noexcept = default;

        /// Indicates if the character array is null terminated.
        inline bool null_terminated() const noexcept { return (bool) m_info.null_terminated; }

        /// Returns true if the character array contains no characters.
        inline bool empty() const noexcept { return !size(); }

        /// Returns the number of characters.
        inline size_t size() const noexcept { return static_cast<size_t>(m_info.size); }

        /// Returns the size of a single char (i.e. sizeof(char_type)).
        inline size_t char_size() const noexcept { return static_cast<size_t>(m_info.c_size); }

        /// Returns the size of the string in bytes (i.e. size() * char_size()).
        inline size_t size_bytes() const noexcept { return size() * char_size(); }

        /// Returns the human readable character count.
        inline size_t count() const noexcept;

        /// Returns the encoding.
        inline enum char_encoding encoding() const noexcept;

        /// Returns a pointer to a null-terminated character array with data equivalent to those
        /// stored in the string. The pointer is such that the range [c_str(); c_str() + size()]
        /// is valid and the values in it correspond to the values stored in the string with an
        /// additional null character after the last position.
        ///
        /// @warning This can only be called with a CharT matching the current char_size().
        ///          A nullptr is returned if sizeof(CharT) != char_size().
        ///
        /// @warning This can only be called when null_terminated() returns true.
        ///          A nullptr is returned if null_terminated() == false.
        ///
        /// @see     data() or view() for non null-terminated strings.
        template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t> = nullptr>
        inline const CharT* c_str() const noexcept;

        /// Same as c_str<char>().
        inline const char* u8cstr() const noexcept;

        /// Same as c_str<char16_t>().
        inline const char16_t* u16cstr() const noexcept;

        /// Same as c_str<char32_t>().
        inline const char32_t* u32cstr() const noexcept;

        /// Same as c_str<wchar_t>().
        inline const wchar_t* wcstr() const noexcept;

        /// Returns a pointer to the character array.
        /// The pointer is such that the range [data(); data() + size()) is valid and the values
        /// in it correspond to the values of the view.
        ///
        /// @warning This can only be called with a CharT matching the current char_size().
        ///          A nullptr is returned if sizeof(CharT) != char_size().
        ///
        /// @warning Unlike __fst::basic_string::data() and string literals, data() may return a pointer
        ///          to a buffer that is not null-terminated. Therefore it is typically a mistake to
        ///          pass data() to a routine that takes just a const CharT* and expects a
        ///          null-terminated string.
        ///
        /// @see     null_terminated()
        template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t> = nullptr>
        inline const CharT* data() const noexcept;

        /// Same as data<char>().
        inline const char* u8data() const noexcept;

        /// Same as data<char16_t>().
        inline const char16_t* u16data() const noexcept;

        /// Same as data<char32_t>().
        inline const char32_t* u32data() const noexcept;

        /// Same as data<wchar_t>().
        inline const wchar_t* wdata() const noexcept;

        /// Returns a __fst::basic_string<CharT>.
        template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t> = nullptr>
        inline __fst::basic_string<CharT, __fst::default_allocator<CharT>> str() const;

        /// Same as str<char>()
        inline __fst::string u8str() const noexcept;

        /// Same as str<char16_t>()
        inline __fst::basic_string<char16_t> u16str() const noexcept;

        /// Same as str<char32_t>()
        inline __fst::basic_string<char32_t> u32str() const noexcept;

        /// Same as str<wchar_t>()
        inline __fst::wstring wstr() const noexcept;

        /// Returns a __fst::basic_string_view<CharT>.
        /// @warning This can only be called with a CharT matching the current char_size().
        ///          An empty __fst::basic_string_view<CharT> is returned if sizeof(CharT) != char_size().
        template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t> = nullptr>
        inline __fst::basic_string_view<CharT> view() const;

        /// Same as view<char>().
        inline __fst::string_view u8view() const noexcept;

        /// Same as view<char16_t>().
        inline __fst::basic_string_view<char16_t> u16view() const noexcept;

        /// Same as view<char32_t>().
        inline __fst::basic_string_view<char32_t> u32view() const noexcept;

        /// Same as view<wchar_t>().
        inline __fst::wstring_view wview() const noexcept;

        /// Converts the character array to a utf8 __fst::string.
        inline __fst::string to_utf8() const;

        /// Converts the character array to a utf16 __fst::u16string.
        inline __fst::u16string to_utf16() const;

        /// Converts the character array to a utf32 __fst::u32string.
        inline __fst::u32string to_utf32() const;

        /// Converts the character array to a __fst::wstring.
        inline __fst::wstring to_wide() const;

        inline operator __fst::string() const { return to_utf8(); }
        inline operator __fst::u16string() const { return to_utf16(); }
        inline operator __fst::u32string() const { return to_utf32(); }
        inline operator __fst::wstring() const { return to_wide(); }

      private:
        union content
        {
            inline content() noexcept;
            inline content(const char* s) noexcept;
            inline content(const char16_t* s) noexcept;
            inline content(const char32_t* s) noexcept;
            inline content(const wchar_t* s) noexcept;

            inline const wchar_t* cw() const noexcept;

            const char* c8;
            const char16_t* c16;
            const char32_t* c32;
        } m_data;

        struct
        {
            uint64_t size;
            uint8_t c_size;
            uint8_t null_terminated;
        } m_info;

        struct normal_tag
        {};

        struct null_terminated_tag
        {};

        template <typename T>
        static inline auto make(T&& s);

        inline utf_string_view(const __fst::string& str, normal_tag) noexcept;
        inline utf_string_view(const __fst::u16string& str, normal_tag) noexcept;
        inline utf_string_view(const __fst::u32string& str, normal_tag) noexcept;
        inline utf_string_view(const __fst::wstring& str, normal_tag) noexcept;
        inline utf_string_view(__fst::string_view str, normal_tag) noexcept;
        inline utf_string_view(__fst::u16string_view str, normal_tag) noexcept;
        inline utf_string_view(__fst::u32string_view str, normal_tag) noexcept;
        inline utf_string_view(__fst::wstring_view str, normal_tag) noexcept;
        inline utf_string_view(__fst::string_view str, null_terminated_tag) noexcept;
        inline utf_string_view(__fst::u16string_view str, null_terminated_tag) noexcept;
        inline utf_string_view(__fst::u32string_view str, null_terminated_tag) noexcept;
        inline utf_string_view(__fst::wstring_view str, null_terminated_tag) noexcept;
    };

    //
    //
    //
    template <typename T, __fst::enable_if_t<is_utf_string_type<remove_cvref_t<T>>::value, __fst::nullptr_t>>
    utf_string_view::utf_string_view(T && s)
        : utf_string_view(make<T>(__fst::forward<T>(s)))
    {
        //  (void)m_reserved;
    }

    utf_string_view::utf_string_view() noexcept
        : m_data()
        , m_info{ 0, sizeof(char), false }
    {}

    utf_string_view::utf_string_view(const __fst::string& str, normal_tag) noexcept
        : m_data{ str.c_str() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char), (uint8_t) true }
    {}

    utf_string_view::utf_string_view(const __fst::u16string& str, normal_tag) noexcept
        : m_data{ str.c_str() }
        , m_info{ static_cast<uint32_t>(str.size()), (uint8_t) sizeof(char16_t), (uint8_t) true }
    {}

    utf_string_view::utf_string_view(const __fst::u32string& str, normal_tag) noexcept
        : m_data{ str.c_str() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char32_t), (uint8_t) true }
    {}

    utf_string_view::utf_string_view(const __fst::wstring& str, normal_tag) noexcept
        : m_data{ str.c_str() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(wchar_t), (uint8_t) true }
    {}

    utf_string_view::utf_string_view(__fst::string_view str, normal_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char), (uint8_t) false }
    {}

    utf_string_view::utf_string_view(__fst::u16string_view str, normal_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char16_t), (uint8_t) false }
    {}

    utf_string_view::utf_string_view(__fst::u32string_view str, normal_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char32_t), (uint8_t) false }
    {}

    utf_string_view::utf_string_view(__fst::wstring_view str, normal_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(wchar_t), (uint8_t) false }
    {}

    utf_string_view::utf_string_view(__fst::string_view str, null_terminated_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char), (uint8_t) true }
    {}

    utf_string_view::utf_string_view(__fst::u16string_view str, null_terminated_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char16_t), (uint8_t) true }
    {}

    utf_string_view::utf_string_view(__fst::u32string_view str, null_terminated_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(char32_t), (uint8_t) true }
    {}

    utf_string_view::utf_string_view(__fst::wstring_view str, null_terminated_tag) noexcept
        : m_data{ str.data() }
        , m_info{ static_cast<uint64_t>(str.size()), (uint8_t) sizeof(wchar_t), (uint8_t) true }
    {}

    utf_string_view::content::content() noexcept
        : c8(nullptr)
    {}

    utf_string_view::content::content(const char* s) noexcept
        : c8(s)
    {}

    utf_string_view::content::content(const char16_t* s) noexcept
        : c16(s)
    {}

    utf_string_view::content::content(const char32_t* s) noexcept
        : c32(s)
    {}

    utf_string_view::content::content(const wchar_t* s) noexcept
        : content(
            [](const wchar_t* ws)
            {
                if constexpr (sizeof(wchar_t) == sizeof(char16_t)) { return content(reinterpret_cast<const char16_t*>(ws)); }
                else { return content(reinterpret_cast<const char32_t*>(ws)); }
            }(s))
    {}

    const wchar_t* utf_string_view::content::cw() const noexcept
    {
        if constexpr (sizeof(wchar_t) == sizeof(char16_t)) { return reinterpret_cast<const wchar_t*>(c16); }
        else { return reinterpret_cast<const wchar_t*>(c32); }
    }

    enum char_encoding utf_string_view::encoding() const noexcept
    {
        switch (char_size())
        {
        case sizeof(char): return char_encoding::utf8;
        case sizeof(char16_t): return char_encoding::utf16;
        case sizeof(char32_t): return char_encoding::utf32;
        default: return char_encoding::utf8;
        }
    }

    template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t>>
    const CharT* utf_string_view::c_str() const noexcept
    {
// #define ERROR_RESULT (fst_error("wrong encoding or not null terminated"), nullptr)
#define ERROR_RESULT nullptr

        if constexpr (__fst::is_same_v<CharT, char>) { return (char_size() == sizeof(char) && null_terminated()) ? m_data.c8 : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, char16_t>) { return (char_size() == sizeof(char16_t) && null_terminated()) ? m_data.c16 : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, char32_t>) { return (char_size() == sizeof(char32_t) && null_terminated()) ? m_data.c32 : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, wchar_t>) { return (char_size() == sizeof(wchar_t) && null_terminated()) ? m_data.cw() : ERROR_RESULT; }

        else
        {
            // Should never happen.
            return nullptr;
        }

#undef ERROR_RESULT
    }

    template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t>>
    const CharT* utf_string_view::data() const noexcept
    {
//#define ERROR_RESULT (fst_error("wrong encoding"), nullptr)
#define ERROR_RESULT nullptr

        if constexpr (__fst::is_same_v<CharT, char>) { return (char_size() == sizeof(char)) ? m_data.c8 : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, char16_t>) { return (char_size() == sizeof(char16_t)) ? m_data.c16 : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, char32_t>) { return (char_size() == sizeof(char32_t)) ? m_data.c32 : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, wchar_t>) { return (char_size() == sizeof(wchar_t)) ? m_data.cw() : ERROR_RESULT; }

        else
        {
            // Should never happen.
            fst_error("wrong encoding");
            return nullptr;
        }

#undef ERROR_RESULT
    }

    template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t>>
    __fst::basic_string<CharT, __fst::default_allocator<CharT>> utf_string_view::str() const
    {
        using stype = __fst::basic_string<CharT, __fst::default_allocator<CharT>>;

        if constexpr (__fst::is_same_v<CharT, char>) { return (char_size() == sizeof(char)) ? stype(m_data.c8, size()) : to_utf8(); }

        else if constexpr (__fst::is_same_v<CharT, char16_t>) { return (char_size() == sizeof(char16_t)) ? stype(m_data.c16, size()) : to_utf16(); }

        else if constexpr (__fst::is_same_v<CharT, char32_t>) { return (char_size() == sizeof(char32_t)) ? stype(m_data.c32, size()) : to_utf32(); }

        else if constexpr (__fst::is_same_v<CharT, wchar_t>) { return (char_size() == sizeof(wchar_t)) ? stype(m_data.cw(), size()) : to_wide(); }

        else
        {
            // Should never happen.
            fst_error("wrong encoding");
            return stype();
        }
    }

    template <typename CharT, __fst::enable_if_t<is_utf_char_type<CharT>::value, __fst::nullptr_t>>
    __fst::basic_string_view<CharT> utf_string_view::view() const
    {
        using svtype = __fst::basic_string_view<CharT>;

// #define ERROR_RESULT (fst_error("wrong encoding"), svtype())
#define ERROR_RESULT svtype()

        if constexpr (__fst::is_same_v<CharT, char>) { return (char_size() == sizeof(char)) ? svtype(m_data.c8, size()) : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, char16_t>) { return (char_size() == sizeof(char16_t)) ? svtype(m_data.c16, size()) : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, char32_t>) { return (char_size() == sizeof(char32_t)) ? svtype(m_data.c32, size()) : ERROR_RESULT; }

        else if constexpr (__fst::is_same_v<CharT, wchar_t>) { return (char_size() == sizeof(wchar_t)) ? svtype(m_data.cw(), size()) : ERROR_RESULT; }

        else
        {
            // Should never happen.
            fst_error("wrong encoding");
            return svtype();
        }

#undef ERROR_RESULT
    }

    template <typename T>
    auto utf_string_view::make(T && s)
    {
        using type = __fst::remove_cv_t<__fst::remove_reference_t<T>>;

        constexpr bool is_any_string_or_string_view = __fst::is_same_v<type, __fst::string> //
                                                      || __fst::is_same_v<type, __fst::u16string> //
                                                      || __fst::is_same_v<type, __fst::u32string> //
                                                      || __fst::is_same_v<type, __fst::wstring> //
                                                      || __fst::is_same_v<type, __fst::string_view> //
                                                      || __fst::is_same_v<type, __fst::u16string_view> //
                                                      || __fst::is_same_v<type, __fst::u32string_view> //
                                                      || __fst::is_same_v<type, __fst::wstring_view>;

        static_assert(is_any_string_or_string_view || is_utf_string_type<T>::value, "");

        if constexpr (is_any_string_or_string_view) { return utf_string_view(__fst::forward<T>(s), normal_tag{}); }

        else if constexpr (__fst::is_constructible_v<__fst::string_view, T>) { return utf_string_view(__fst::string_view(s), null_terminated_tag{}); }

        else if constexpr (__fst::is_constructible_v<__fst::u16string_view, T>) { return utf_string_view(__fst::u16string_view(s), null_terminated_tag{}); }

        else if constexpr (__fst::is_constructible_v<__fst::u32string_view, T>) { return utf_string_view(__fst::u32string_view(s), null_terminated_tag{}); }

        else if constexpr (__fst::is_constructible_v<__fst::wstring_view, T>) { return utf_string_view(__fst::wstring_view(s), null_terminated_tag{}); }

        else
        {
            // Should never happen.
            return utf_string_view();
        }
    }

    __fst::string utf_string_view::to_utf8() const
    {
        if (empty()) { return {}; }

        switch (encoding())
        {
        case char_encoding::utf8: return __fst::string(data<char>(), size());

        case char_encoding::utf16: return utf_cvt_as<__fst::string>(view<char16_t>());

        case char_encoding::utf32: return utf_cvt_as<__fst::string>(view<char32_t>());
        }

        return {};
    }

    __fst::u16string utf_string_view::to_utf16() const
    {
        if (empty()) { return {}; }

        switch (encoding())
        {
        case char_encoding::utf8: return utf_cvt_as<__fst::u16string>(view<char>());

        case char_encoding::utf16: return __fst::u16string(data<char16_t>(), size());

        case char_encoding::utf32: return utf_cvt_as<__fst::u16string>(view<char32_t>());
        }

        return {};
    }

    __fst::u32string utf_string_view::to_utf32() const
    {
        if (empty()) { return {}; }

        switch (encoding())
        {
        case char_encoding::utf8: return utf_cvt_as<__fst::u32string>(view<char>());

        case char_encoding::utf16: return utf_cvt_as<u32string>(view<char16_t>());

        case char_encoding::utf32: return __fst::u32string(data<char32_t>(), size());
        }

        return {};
    }

    __fst::wstring utf_string_view::to_wide() const
    {
        if (empty()) { return {}; }

        switch (encoding())
        {
        case char_encoding::utf8: return utf_cvt_as<__fst::wstring>(view<char>());

        case char_encoding::utf16: return utf_cvt_as<__fst::wstring>(view<char16_t>());

        case char_encoding::utf32: return utf_cvt_as<__fst::wstring>(view<char32_t>());
        }

        return {};
    }

    size_t utf_string_view::count() const noexcept
    {
        switch (encoding())
        {
        case char_encoding::utf8: return __fst::utf_length(view<char>());

        case char_encoding::utf16: return __fst::utf_length(view<char16_t>());

        case char_encoding::utf32: return size();
        }
        return 0;
    }

    inline const char* utf_string_view::u8cstr() const noexcept
    {
        return c_str<char>();
    }

    /// Same as c_str<char16_t>().
    inline const char16_t* utf_string_view::u16cstr() const noexcept
    {
        return c_str<char16_t>();
    }

    /// Same as c_str<char32_t>().
    inline const char32_t* utf_string_view::u32cstr() const noexcept
    {
        return c_str<char32_t>();
    }

    /// Same as c_str<wchar_t>().
    inline const wchar_t* utf_string_view::wcstr() const noexcept
    {
        return c_str<wchar_t>();
    }

    /// Same as data<char>().
    inline const char* utf_string_view::u8data() const noexcept
    {
        return data<char>();
    }

    /// Same as data<char16_t>().
    inline const char16_t* utf_string_view::u16data() const noexcept
    {
        return data<char16_t>();
    }

    /// Same as data<char32_t>().
    inline const char32_t* utf_string_view::u32data() const noexcept
    {
        return data<char32_t>();
    }

    /// Same as data<wchar_t>().
    inline const wchar_t* utf_string_view::wdata() const noexcept
    {
        return data<wchar_t>();
    }

    /// Same as str<char>()
    inline __fst::string utf_string_view::u8str() const noexcept
    {
        return str<char>();
    }

    /// Same as str<char16_t>()
    inline __fst::basic_string<char16_t> utf_string_view::u16str() const noexcept
    {
        return str<char16_t>();
    }

    /// Same as str<char32_t>()
    inline __fst::basic_string<char32_t> utf_string_view::u32str() const noexcept
    {
        return str<char32_t>();
    }

    /// Same as str<wchar_t>()
    inline __fst::wstring utf_string_view::wstr() const noexcept
    {
        return str<wchar_t>();
    }

    /// Same as view<char>().
    inline __fst::string_view utf_string_view::u8view() const noexcept
    {
        return view<char>();
    }

    /// Same as view<char16_t>().
    inline __fst::basic_string_view<char16_t> utf_string_view::u16view() const noexcept
    {
        return view<char16_t>();
    }

    /// Same as view<char32_t>().
    inline __fst::basic_string_view<char32_t> utf_string_view::u32view() const noexcept
    {
        return view<char32_t>();
    }

    /// Same as view<wchar_t>().
    inline __fst::wstring_view utf_string_view::wview() const noexcept
    {
        return view<wchar_t>();
    }

    inline __fst::output_stream<char>& operator<<(__fst::output_stream<char>& stream, const utf_string_view& s)
    {
        auto ss = s.str<char>();
        stream.write(ss.data(), ss.size());
        return stream;
    }

    inline __fst::output_stream<wchar_t>& operator<<(__fst::output_stream<wchar_t>& stream, const utf_string_view& s)
    {
        auto ss = s.str<wchar_t>();
        stream.write(ss.data(), ss.size());
        return stream;
    }

    namespace utf
    {
        __fst::string to_utf8(__fst::utf_string_view s) noexcept
        {
            return s.to_utf8();
        }
        __fst::u16string to_utf16(__fst::utf_string_view s) noexcept
        {
            return s.to_utf16();
        }
        __fst::u32string to_utf32(__fst::utf_string_view s) noexcept
        {
            return s.to_utf32();
        }
        __fst::wstring to_wide(__fst::utf_string_view s) noexcept
        {
            return s.to_wide();
        }
    } // namespace utf
FST_END_NAMESPACE
