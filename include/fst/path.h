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

#include "fst/string_view.h"
#include "fst/memory.h"
#include "fst/string.h"
#include "fst/small_string.h"

FST_BEGIN_NAMESPACE
    namespace filesystem
    {

#if __FST_WINDOWS__
        static constexpr char separator = (char) '\\';
#else
        static constexpr char separator = (char) '/';
#endif

        __fst::status create_directory(__fst::string_view name, bool exist_is_success = true) noexcept;
        __fst::status create_directories(__fst::string_view name) noexcept;

        __fst::string_view get_parent_directory(__fst::string_view p) noexcept;

        bool is_directory(__fst::string_view p) noexcept;

        __fst::status delete_file(__fst::string_view name) noexcept;
        __fst::status delete_directory(__fst::string_view name) noexcept;

        template <class _StringType>
        inline _StringType join(__fst::string_view a, __fst::string_view b) noexcept
        {
            _StringType s;
            size_t sz = a.size() + b.size() + 1;
            s.resize(sz);
            __fst::memcpy(s.data(), a.data(), a.size());
            s[a.size()] = separator;
            __fst::memcpy(s.data() + a.size() + 1, b.data(), b.size());

            return s;
        }

        template <class _StringType, typename... _Ts, __fst::enable_if_t<__fst::conjunction_v<__fst::is_string_view_convertible<_Ts>...>, int> = 0>
        inline constexpr _StringType join(_Ts&&... vs) noexcept
        {
            static_assert(sizeof...(_Ts) > 1, "DD");
            __fst::array<__fst::string_view, sizeof...(_Ts)> views(__fst::string_view(__fst::forward<_Ts>(vs))...);
            size_t sz = views.size() - 1;
            for (const auto& s : views)
            {
                sz += s.size();
            }

            _StringType sout;
            sout.resize(sz);

            char* data = sout.data();
            for (size_t i = 0; i < views.size() - 1; i++)
            {
                __fst::memcpy(data, views[i].data(), views[i].size());
                data += views[i].size();
                *data++ = separator;
            }

            __fst::memcpy(data, views.back().data(), views.back().size());

            return sout;
        }

        //
        struct path_range;

        //
        class path_iterator
        {
          public:
            using difference_type = ptrdiff_t;
            using iterator_category = __fst::forward_iterator_tag;
            using pointer = const char*;

            inline path_iterator() noexcept = default;

            inline path_iterator(pointer en) noexcept
                : _begin(en)
                , _line_end(en)
                , _end(en)
            {}

            inline path_iterator(pointer b, pointer en) noexcept
                : _begin(b)
                , _line_end(nullptr)
                , _end(en)
            {

                _line_end = __fst::char_traits<char>::find(b, __fst::pdistance(b, _end), separator);
                if (!_line_end) { _line_end = _end; }
            }

            inline __fst::basic_string_view<char> operator*() const noexcept { return __fst::basic_string_view<char>(_begin, __fst::pdistance(_begin, _line_end)); }

            inline bool operator==(const path_iterator& rhs) const noexcept { return (_begin == rhs._begin) && (_line_end == rhs._line_end) && (_end == rhs._end); }

            inline bool operator!=(const path_iterator& rhs) const noexcept { return !(operator==(rhs)); }

            inline path_iterator& operator++() noexcept
            {
                _begin = ++_line_end;
                if (_line_end < _end)
                {
                    if (pointer line_end = __fst::char_traits<char>::find(_line_end, __fst::pdistance(_line_end, _end), separator)) { _line_end = line_end; }

                    else { _line_end = _end; }
                    return *this;
                }

                _begin = _line_end = _end;
                return *this;
            }

            inline path_iterator operator++(int) noexcept
            {
                path_iterator temp = *this;
                _begin = ++_line_end;
                if (_line_end < _end)
                {
                    if (pointer line_end = ::__fst::char_traits<char>::find(_line_end, __fst::pdistance(_line_end, _end), separator)) { _line_end = line_end; }
                    else { _line_end = _end; }
                    return temp;
                }

                _begin = _line_end = _end;
                return temp;
            }

          private:
            pointer _begin = nullptr;
            pointer _line_end = nullptr;
            pointer _end = nullptr;

            friend struct path_range;

            inline path_iterator end() const noexcept { return path_iterator(_end); }
        };
        //
        struct path_range
        {
            using iterator_type = path_iterator;
            iterator_type _begin_iterator;

            inline path_range(__fst::basic_string_view<char> c) noexcept
                : _begin_iterator(c.begin(), c.end())
            {}

            template <class _Container, __fst::enable_if_t<__fst::is_container_v<_Container>, int> = 0>
            inline path_range(const _Container& c) noexcept
                : path_range(__fst::basic_string_view<char>(c))
            {}

            inline iterator_type begin() const noexcept { return _begin_iterator; }
            inline iterator_type end() const noexcept { return _begin_iterator.end(); }
            inline bool empty() const noexcept { return _begin_iterator == _begin_iterator.end(); }
        };

        //
    } // namespace filesystem

    template <class _CharT>
    class basic_path_view
    {
      public:
        using view_type = __fst::basic_string_view<_CharT>;
        using size_type = typename view_type::size_type;

        basic_path_view() noexcept = default;
        basic_path_view(view_type p) noexcept
            : _path(p)
        {}

        view_type path() const { return _path; }

      private:
        view_type _path;
    };
FST_END_NAMESPACE
