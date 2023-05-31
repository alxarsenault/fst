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
#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    ///
    template <typename T>
    struct range;

    ///
    template <typename T>
    struct point;

    ///
    template <typename T>
    struct rect;

    template <class T>
    struct is_rect : __fst::false_t
    {};

    template <class T>
    struct is_rect<__fst::rect<T>> : __fst::true_t
    {};

    namespace meta
    {
#define FST_USING_TYPE(x) \
    template <class T>    \
    using x = decltype(T::x)

        FST_USING_TYPE(x);
        FST_USING_TYPE(y);
        FST_USING_TYPE(width);
        FST_USING_TYPE(height);
        FST_USING_TYPE(X);
        FST_USING_TYPE(Y);
        FST_USING_TYPE(Width);
        FST_USING_TYPE(Height);
        FST_USING_TYPE(origin);
        FST_USING_TYPE(size);
        FST_USING_TYPE(left);
        FST_USING_TYPE(right);
        FST_USING_TYPE(top);
        FST_USING_TYPE(bottom);
        FST_USING_TYPE(start);
        FST_USING_TYPE(end);

#undef FST_USING_TYPE
    } // namespace meta.

    //
    // MARK: - Geometry -
    //

    /// A general-purpose range object, that simply represents any linear range with
    /// a start and end value.
    template <typename T>
    struct range
    {
        using value_type = T;
        value_type start, end;

        /// No custom default/copy/move constructor, copy/move assignment and destructor
        /// to remain a trivial type.
        range() noexcept = default;
        range(range&&) noexcept = default;
        range(const range&) noexcept = default;

        template <typename U>
        inline constexpr range(const range<U>& r) noexcept;

        /// Constructor.
        inline constexpr range(value_type _start, value_type _end) noexcept;

        /// Creates a range with and start and length.
        FST_NODISCARD static inline constexpr range with_length(value_type start, value_type len) noexcept;

        ~range() noexcept = default;

        range& operator=(range&&) noexcept = default;
        range& operator=(const range&) noexcept = default;

        template <typename U>
        inline constexpr range& operator=(const range<U>& r) noexcept;

        /// Create a new range with given start position.
        ///
        /// @see set_start
        FST_NODISCARD inline constexpr range with_start(value_type s) const noexcept;

        /// Create a new range with given end position.
        ///
        /// @see set_end
        FST_NODISCARD inline constexpr range with_end(value_type e) const noexcept;

        /// Create a new range with shifted start position by delta.
        ///
        /// @see shift_start
        FST_NODISCARD inline constexpr range with_shifted_start(value_type delta) const noexcept;

        /// Create a new range with shifted end position by delta.
        ///
        /// @see shift_end
        FST_NODISCARD inline constexpr range with_shifted_end(value_type delta) const noexcept;

        /// Create a new range with given length.
        ///
        /// @see set_length
        FST_NODISCARD inline constexpr range with_length(value_type len) const noexcept;

        /// Create a new range with shifted start position by delta but keeping length.
        ///
        /// @see shift
        FST_NODISCARD inline constexpr range with_shift(value_type delta) const noexcept;

        /// Create a new range with new start position but keeping length.
        ///
        /// @see move_to
        FST_NODISCARD inline constexpr range with_move(value_type s) const noexcept;

        /// Moves to new start position.
        inline constexpr range& set_start(value_type s) noexcept;

        /// Moves to new end position .
        inline constexpr range& set_end(value_type e) noexcept;

        /// Moves to new start position and keep current length.
        inline constexpr range& move_to(value_type s) noexcept;

        /// Moves start position by delta and keep current length.
        inline constexpr range& shift(value_type delta) noexcept;

        /// Moves start position by delta.
        inline constexpr range& shift_start(value_type delta) noexcept;

        /// Moves start position by delta.
        inline constexpr range& shift_end(value_type delta) noexcept;

        /// Changes the length of the range.
        inline constexpr range& set_length(value_type len) noexcept;

        /// Returns the length of the range.
        FST_NODISCARD inline constexpr value_type length() const noexcept;

        /// Returns the middle of the range.
        FST_NODISCARD inline constexpr value_type middle() const noexcept;

        /// Returns true if the start side of range is smaller than the end side.
        FST_NODISCARD inline constexpr bool is_sorted() const noexcept;

        /// Returns true if the range is symmetric (i.e. start == -end).
        FST_NODISCARD inline constexpr bool is_symmetric() const noexcept;

        /// Returns true if the given position lies inside this range.
        FST_NODISCARD inline constexpr bool contains(value_type x) const noexcept;

        /// Same as contains(x).
        FST_NODISCARD inline constexpr bool contains_closed(value_type x) const noexcept;

        /// Returns true if the given position lies inside ]start, end[
        FST_NODISCARD inline constexpr bool contains_opened(value_type x) const noexcept;

        /// Returns true if the given position lies inside ]start, end]
        FST_NODISCARD inline constexpr bool contains_left_opened(value_type x) const noexcept;

        /// Returns true if the given position lies inside [start, end[
        FST_NODISCARD inline constexpr bool contains_right_opened(value_type x) const noexcept;

        /// Returns true if the given range lies entirely inside this range.
        FST_NODISCARD inline constexpr bool contains(const range& r) const noexcept;

        /// Returns the nearest value to the one supplied, which lies within the range.
        FST_NODISCARD inline constexpr value_type clipped_value(value_type x) const noexcept;

        /// Swaps the value of start and end if the range is not sorted.
        ///
        /// @see is_sorted.
        inline constexpr range& sort() noexcept;

        FST_NODISCARD inline constexpr bool operator==(const range& r) const noexcept;
        FST_NODISCARD inline constexpr bool operator!=(const range& r) const noexcept;
        FST_NODISCARD inline constexpr bool operator<(const range& r) const noexcept;
        FST_NODISCARD inline constexpr bool operator<=(const range& r) const noexcept;
        FST_NODISCARD inline constexpr bool operator>(const range& r) const noexcept;
        FST_NODISCARD inline constexpr bool operator>=(const range& r) const noexcept;
    };

    static_assert(__fst::is_trivial<range<int>>::value, "__fst::range must remain a trivial type");
    static_assert(__fst::is_trivial<range<float>>::value, "__fst::range must remain a trivial type");

    template <typename T>
    range(T, T) -> range<T>;

    template <typename T1, typename T2>
    range(T1, T2) -> range<__fst::common_type_t<T1, T2>>;

    ///
    template <typename T>
    struct padding
    {
        using value_type = T;
        value_type top;
        value_type left;
        value_type bottom;
        value_type right;

        padding() noexcept = default;
        padding(const padding&) noexcept = default;
        padding(padding&&) noexcept = default;

        template <typename U>
        inline constexpr padding(const padding<U>& p) noexcept;

        inline constexpr padding(value_type t, value_type l, value_type b, value_type r) noexcept;

        inline constexpr padding(value_type p) noexcept;

        ~padding() noexcept = default;

        padding& operator=(const padding&) noexcept = default;
        padding& operator=(padding&&) noexcept = default;

        template <typename U>
        inline constexpr padding& operator=(const padding<U>& p) noexcept;

        FST_NODISCARD inline __fst::rect<value_type> inside_rect(const __fst::rect<value_type>& rect) const noexcept;

        FST_NODISCARD inline __fst::rect<value_type> outside_rect(const __fst::rect<value_type>& rect) const noexcept;

        FST_NODISCARD inline constexpr bool empty() const noexcept;

        FST_NODISCARD inline constexpr bool operator==(const padding& p) const noexcept;

        FST_NODISCARD inline constexpr bool operator!=(const padding& p) const noexcept;
    };

    template <typename T, __fst::enable_if_t<__fst::is_arithmetic<T>::value, __fst::nullptr_t> = nullptr>
    padding(T) -> padding<T>;

    template <typename T>
    padding(T, T, T, T) -> padding<T>;

    template <typename T1, typename T2, typename T3, typename T4>
    padding(T1, T2, T3, T4) -> padding<__fst::common_type_t<T1, T2, T3, T4>>;

    namespace detail
    {
        template <class RectType, bool hasMember>
        struct is_rect_os_impl
        {
            static constexpr bool value = false;
        };

        template <class RectType>
        struct is_rect_os_impl<RectType, true>
        {
            using value_type = decltype(RectType{}.origin.x);
            static constexpr bool value = fst_offsetof(RectType, origin) == 0 //
                                          && fst_offsetof(RectType, size) == 2 * sizeof(value_type) && fst_offsetof(decltype(RectType{}.origin), y) == sizeof(value_type)
                                          && fst_offsetof(decltype(RectType{}.size), width) == 0 //
                                          && fst_offsetof(decltype(RectType{}.size), height) == sizeof(value_type);
        };

        template <class RectType, bool hasMember>
        struct is_rect_xywh_impl
        {
            static constexpr bool value = false;
        };

        template <class RectType>
        struct is_rect_xywh_impl<RectType, true>
        {
            using value_type = decltype(RectType{}.x);
            static constexpr bool value = fst_offsetof(RectType, x) == 0 //
                                          && fst_offsetof(RectType, y) == sizeof(value_type) //
                                          && fst_offsetof(RectType, width) == sizeof(value_type) * 2 //
                                          && fst_offsetof(RectType, height) == sizeof(value_type) * 3;
        };

        template <class RectType, bool hasMember>
        struct is_rect_XYWH_impl
        {
            static constexpr bool value = false;
        };

        template <class RectType>
        struct is_rect_XYWH_impl<RectType, true>
        {
            using value_type = decltype(RectType{}.X);
            static constexpr bool value = fst_offsetof(RectType, X) == 0 //
                                          && fst_offsetof(RectType, Y) == sizeof(value_type) //
                                          && fst_offsetof(RectType, Width) == sizeof(value_type) * 2 //
                                          && fst_offsetof(RectType, Height) == sizeof(value_type) * 3;
        };

        template <class RectType, bool hasMember>
        struct is_rect_ltrb_impl
        {
            static constexpr bool value = false;
        };

        template <class RectType>
        struct is_rect_ltrb_impl<RectType, true>
        {
            using value_type = decltype(RectType{}.left);

            static constexpr bool value = fst_offsetof(RectType, left) == 0 //
                                          && fst_offsetof(RectType, top) == sizeof(value_type) //
                                          && fst_offsetof(RectType, right) == sizeof(value_type) * 2 //
                                          && fst_offsetof(RectType, bottom) == sizeof(value_type) * 3;
        };

        template <class T>
        struct is_rect_ltrb_impl<__fst::rect<T>, true> : __fst::false_t
        {};

        template <class RectType>
        using is_rect_os = is_rect_os_impl<RectType, //
            has_members<RectType, meta::origin, meta::size>::value>;

        template <class RectType>
        using is_rect_xywh = is_rect_xywh_impl<RectType, //
            has_members<RectType, meta::x, meta::y, meta::width, meta::height>::value && !has_members<RectType, meta::origin, meta::size>::value>;

        template <class RectType>
        using is_rect_XYWH = is_rect_XYWH_impl<RectType, //
            has_members<RectType, meta::X, meta::Y, meta::Width, meta::Height>::value>;

        template <class RectType>
        using is_rect_ltrb = is_rect_ltrb_impl<__fst::remove_cvref_t<RectType>, //
            has_members<RectType, meta::left, meta::top, meta::right, meta::bottom>::value>;

        template <class RectType>
        using enable_if_rect_os = __fst::enable_if_t<!__fst::is_rect<RectType>::value && is_rect_os<RectType>::value, __fst::nullptr_t>;

        template <class RectType>
        using enable_if_rect_xywh = __fst::enable_if_t<!__fst::is_rect<RectType>::value && is_rect_xywh<RectType>::value, __fst::nullptr_t>;

        template <class RectType>
        using enable_if_rect_XYWH = __fst::enable_if_t<!__fst::is_rect<RectType>::value && is_rect_XYWH<RectType>::value, __fst::nullptr_t>;

        template <class RectType>
        using enable_if_rect_ltrb = __fst::enable_if_t<!__fst::is_rect<RectType>::value && is_rect_ltrb<RectType>::value, __fst::nullptr_t>;

        template <class PointType, bool hasMember>
        struct is_point_xy_impl
        {
            static constexpr bool value = false;
        };

        template <class PointType>
        struct is_point_xy_impl<PointType, true>
        {
            using value_type = decltype(PointType{}.x);
            static constexpr bool value = fst_offsetof(PointType, x) == 0 //
                                          && fst_offsetof(PointType, y) == sizeof(value_type);
        };

        template <class PointType, bool hasMember>
        struct is_point_XY_impl
        {
            static constexpr bool value = false;
        };

        template <class PointType>
        struct is_point_XY_impl<PointType, true>
        {
            using value_type = decltype(PointType{}.X);
            static constexpr bool value = fst_offsetof(PointType, X) == 0 //
                                          && fst_offsetof(PointType, Y) == sizeof(value_type);
        };

        template <class PointType>
        using enable_if_point_xy = __fst::enable_if_t< //
            is_point_xy_impl<PointType, has_members<PointType, meta::x, meta::y>::value //
                                            && !is_rect_os<PointType>::value //
                                            && !is_rect_xywh<PointType>::value>::value,
            __fst::nullptr_t>;

        template <class PointType>
        using enable_if_point_XY = __fst::enable_if_t< //
            is_point_XY_impl<PointType, has_members<PointType, meta::X, meta::Y>::value && !is_rect_XYWH<PointType>::value>::value, __fst::nullptr_t>;

        template <class SizeType, bool hasMember>
        struct is_size_wh_impl
        {
            static constexpr bool value = false;
        };

        template <class SizeType>
        struct is_size_wh_impl<SizeType, true>
        {
            using value_type = decltype(SizeType{}.width);
            static constexpr bool value = fst_offsetof(SizeType, width) == 0 //
                                          && fst_offsetof(SizeType, height) == sizeof(value_type);
        };

        template <class SizeType, bool hasMember>
        struct is_size_WH_impl
        {
            static constexpr bool value = false;
        };

        template <class SizeType>
        struct is_size_WH_impl<SizeType, true>
        {
            using value_type = decltype(SizeType{}.Width);
            static constexpr bool value = fst_offsetof(SizeType, Width) == 0 //
                                          && fst_offsetof(SizeType, Height) == sizeof(value_type);
        };

        template <class SizeType>
        using enable_if_size_wh = __fst::enable_if_t< //
            is_size_wh_impl<SizeType, has_members<SizeType, meta::width, meta::height>::value //
                                          && !is_rect_os<SizeType>::value //
                                          && !is_rect_xywh<SizeType>::value>::value,
            __fst::nullptr_t>;

        template <class SizeType>
        using enable_if_size_WH = __fst::enable_if_t< //
            is_size_WH_impl<SizeType, has_members<SizeType, meta::Width, meta::Height>::value //
                                          && !is_rect_XYWH<SizeType>::value>::value,
            __fst::nullptr_t>;
    } // namespace detail.

    ///
    template <typename T>
    struct point
    {
        static_assert(__fst::is_arithmetic<T>::value, "point value_type must be arithmetic");

        using value_type = T;
        value_type x, y;

        point() noexcept = default;
        point(const point&) noexcept = default;
        point(point&&) noexcept = default;

        inline constexpr point(value_type X, value_type Y) noexcept
            : x(X)
            , y(Y)
        {}

        /// Construct a Point from PointType with member x and y.
        template <typename PointType, detail::enable_if_point_xy<PointType> = nullptr>
        inline constexpr point(const PointType& point) noexcept;

        /// Construct a Point from PointType with member X and Y.
        template <typename PointType, detail::enable_if_point_XY<PointType> = nullptr>
        inline constexpr point(const PointType& point) noexcept;

        ~point() noexcept = default;

        point& operator=(const point&) noexcept = default;
        point& operator=(point&&) noexcept = default;

        inline constexpr point& set_x(value_type _x) noexcept;
        inline constexpr point& set_y(value_type _y) noexcept;

        inline constexpr point& add_x(value_type dx) noexcept;
        inline constexpr point& add_y(value_type dy) noexcept;

        FST_NODISCARD inline constexpr point with_x(value_type _x) const noexcept;
        FST_NODISCARD inline constexpr point with_y(value_type _y) const noexcept;

        FST_NODISCARD inline constexpr point with_add_x(value_type dx) const noexcept;
        FST_NODISCARD inline constexpr point with_add_y(value_type dy) const noexcept;

        inline constexpr point& operator+=(value_type v) noexcept;
        inline constexpr point& operator-=(value_type v) noexcept;
        inline constexpr point& operator*=(value_type v) noexcept;
        inline constexpr point& operator/=(value_type v) noexcept;

        inline constexpr point& operator+=(const point& pt) noexcept;
        inline constexpr point& operator-=(const point& pt) noexcept;
        inline constexpr point& operator*=(const point& pt) noexcept;
        inline constexpr point& operator/=(const point& pt) noexcept;

        FST_NODISCARD inline constexpr point operator+(value_type v) const noexcept;
        FST_NODISCARD inline constexpr point operator-(value_type v) const noexcept;
        FST_NODISCARD inline constexpr point operator*(value_type v) const noexcept;
        FST_NODISCARD inline constexpr point operator/(value_type v) const noexcept;

        FST_NODISCARD inline constexpr point operator+(const point& pt) const noexcept;
        FST_NODISCARD inline constexpr point operator-(const point& pt) const noexcept;
        FST_NODISCARD inline constexpr point operator*(const point& pt) const noexcept;
        FST_NODISCARD inline constexpr point operator/(const point& pt) const noexcept;

        FST_NODISCARD inline constexpr bool operator==(const point& pt) const noexcept;
        FST_NODISCARD inline constexpr bool operator!=(const point& pt) const noexcept;

        FST_NODISCARD inline constexpr bool operator<(const point& pt) const noexcept;
        FST_NODISCARD inline constexpr bool operator<=(const point& pt) const noexcept;
        FST_NODISCARD inline constexpr bool operator>(const point& pt) const noexcept;
        FST_NODISCARD inline constexpr bool operator>=(const point& pt) const noexcept;

        FST_NODISCARD inline constexpr point operator-() const noexcept;

        /// Conversion operator to PointType with member x and y.
        template <typename PointType, detail::enable_if_point_xy<PointType> = nullptr>
        FST_NODISCARD inline PointType convert() const;

        /// Conversion operator to PointType with member X and Y.
        template <typename PointType, detail::enable_if_point_XY<PointType> = nullptr>
        FST_NODISCARD inline PointType convert() const;

        /// Conversion operator to PointType with member x and y.
        template <typename PointType, detail::enable_if_point_xy<PointType> = nullptr>
        FST_NODISCARD inline explicit operator PointType() const;

        /// Conversion operator to PointType with member X and Y.
        template <typename PointType, detail::enable_if_point_XY<PointType> = nullptr>
        FST_NODISCARD inline explicit operator PointType() const;

        // template <typename U>
        // inline friend __fst::ostream& operator<<(__fst::ostream& s, const __fst::point<U>& point);
    };

    static_assert(__fst::is_trivial<point<int>>::value, "__fst::point must remain a trivial type");
    static_assert(__fst::is_trivial<point<float>>::value, "__fst::point must remain a trivial type");

    template <typename T>
    point(T, T) -> point<T>;

    template <typename T1, typename T2>
    point(T1, T2) -> point<__fst::common_type_t<T1, T2>>;

    template <typename PointType, detail::enable_if_point_xy<PointType> = nullptr>
    point(const PointType&) -> point<decltype(PointType{}.x)>;

    template <typename PointType, detail::enable_if_point_XY<PointType> = nullptr>
    point(const PointType&) -> point<decltype(PointType{}.X)>;

    ///
    template <typename T>
    struct size
    {
        static_assert(__fst::is_arithmetic<T>::value, "size value_type must be arithmetic");

        using value_type = T;
        value_type width, height;

        size() noexcept = default;
        size(const size&) noexcept = default;
        size(size&&) noexcept = default;

        inline constexpr size(value_type W, value_type H) noexcept;

        ///
        template <typename SizeType, detail::enable_if_size_wh<SizeType> = nullptr>
        inline constexpr size(const SizeType& s) noexcept;

        ///
        template <typename SizeType, detail::enable_if_size_WH<SizeType> = nullptr>
        inline constexpr size(const SizeType& s) noexcept;

        FST_NODISCARD static inline constexpr size zero();
        FST_NODISCARD static inline constexpr size full_scale();

        ~size() noexcept = default;

        size& operator=(const size&) noexcept = default;
        size& operator=(size&&) noexcept = default;

        inline constexpr size& set_width(value_type w) noexcept;
        inline constexpr size& set_height(value_type h) noexcept;
        inline constexpr size& add_width(value_type dw) noexcept;
        inline constexpr size& add_height(value_type dh) noexcept;

        FST_NODISCARD inline constexpr size with_width(value_type w) const noexcept;
        FST_NODISCARD inline constexpr size with_height(value_type h) const noexcept;

        FST_NODISCARD inline constexpr size with_add_width(value_type dw) const noexcept;
        FST_NODISCARD inline constexpr size with_add_height(value_type dh) const noexcept;

        inline constexpr size& operator+=(value_type v) noexcept;
        inline constexpr size& operator-=(value_type v) noexcept;
        inline constexpr size& operator*=(value_type v) noexcept;
        inline constexpr size& operator/=(value_type v) noexcept;

        inline constexpr size& operator+=(const size& s) noexcept;
        inline constexpr size& operator-=(const size& s) noexcept;
        inline constexpr size& operator*=(const size& s) noexcept;
        inline constexpr size& operator/=(const size& s) noexcept;

        FST_NODISCARD inline constexpr size operator+(value_type v) const noexcept;
        FST_NODISCARD inline constexpr size operator-(value_type v) const noexcept;
        FST_NODISCARD inline constexpr size operator*(value_type v) const noexcept;
        FST_NODISCARD inline constexpr size operator/(value_type v) const noexcept;

        FST_NODISCARD inline constexpr size operator+(const size& s) const noexcept;
        FST_NODISCARD inline constexpr size operator-(const size& s) const noexcept;
        FST_NODISCARD inline constexpr size operator*(const size& s) const noexcept;
        FST_NODISCARD inline constexpr size operator/(const size& s) const noexcept;

        FST_NODISCARD inline constexpr bool operator==(const size& s) const noexcept;
        FST_NODISCARD inline constexpr bool operator!=(const size& s) const noexcept;

        FST_NODISCARD inline constexpr bool operator<(const size& s) const noexcept;
        FST_NODISCARD inline constexpr bool operator<=(const size& s) const noexcept;
        FST_NODISCARD inline constexpr bool operator>(const size& s) const noexcept;
        FST_NODISCARD inline constexpr bool operator>=(const size& s) const noexcept;

        FST_NODISCARD inline constexpr size operator-() const noexcept;

        FST_NODISCARD inline constexpr bool empty() const noexcept;

        template <typename SizeType, detail::enable_if_size_wh<SizeType> = nullptr>
        FST_NODISCARD inline SizeType convert() const;

        template <typename SizeType, detail::enable_if_size_WH<SizeType> = nullptr>
        FST_NODISCARD inline SizeType convert() const;

        template <typename SizeType, detail::enable_if_size_wh<SizeType> = nullptr>
        FST_NODISCARD inline explicit operator SizeType() const;

        template <typename SizeType, detail::enable_if_size_WH<SizeType> = nullptr>
        FST_NODISCARD inline explicit operator SizeType() const;

        // template <typename U>
        // inline friend __fst::ostream& operator<<(__fst::ostream& s, const __fst::size<U>& size);
    };

    static_assert(__fst::is_trivial<size<int>>::value, "__fst::size must remain a trivial type");
    static_assert(__fst::is_trivial<size<float>>::value, "__fst::size must remain a trivial type");

    template <typename T>
    size(T, T) -> size<T>;

    template <typename T1, typename T2>
    size(T1, T2) -> size<__fst::common_type_t<T1, T2>>;

    template <typename SizeType, detail::enable_if_size_wh<SizeType> = nullptr>
    size(const SizeType&) -> size<decltype(SizeType{}.width)>;

    template <typename SizeType, detail::enable_if_size_WH<SizeType> = nullptr>
    size(const SizeType&) -> size<decltype(SizeType{}.Width)>;

    ///
    template <typename T>
    struct rect
    {
        static_assert(__fst::is_arithmetic<T>::value, "rect value_type must be arithmetic");

        using value_type = T;
        using point_type = __fst::point<value_type>;
        using size_type = __fst::size<value_type>;

        FST_PRAGMA_PUSH()
        FST_PRAGMA_DISABLE_WARNING_MSVC(4201)
        FST_PRAGMA_DISABLE_WARNING_CLANG("-Wnested-anon-types")
        FST_PRAGMA_DISABLE_WARNING_CLANG("-Wgnu-anonymous-struct")
        FST_PRAGMA_DISABLE_WARNING_GCC("-Wpedantic")

        union
        {
            point_type origin;
            point_type position;
            struct
            {
                value_type x, y;
            };
        };

        union
        {
            size_type size;
            struct
            {
                value_type width, height;
            };
        };
        FST_PRAGMA_POP()

        rect() noexcept = default;
        rect(const rect&) noexcept = default;
        rect(rect&&) noexcept = default;

        inline constexpr rect(const point_type& p, const size_type& s) noexcept
            : origin(p)
            , size(s)
        {}

        inline constexpr rect(value_type x, value_type y, value_type w, value_type h) noexcept;
        inline constexpr rect(value_type x, value_type y, const size_type& s) noexcept;

        template <class U>
        inline constexpr rect(const __fst::rect<U>& rect) noexcept
            : origin{ rect.origin.template convert<value_type>() }
            , size{ rect.size.template convert<value_type>() }
        {}

        template <typename RectType, detail::enable_if_rect_os<RectType> = nullptr>
        inline constexpr rect(const RectType& rect) noexcept;

        template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
        inline constexpr rect(const RectType& rect) noexcept;

        template <typename RectType, detail::enable_if_rect_XYWH<RectType> = nullptr>
        inline constexpr rect(const RectType& rect) noexcept;

        template <typename RectType, detail::enable_if_rect_ltrb<RectType> = nullptr>
        inline constexpr rect(const RectType& rect) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_point(const point_type& topLeft, const point_type& bottomRight) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_bottom_left(value_type x, value_type y, value_type w, value_type h) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_bottom_left(const point_type& p, const size_type& s) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_bottom_right(value_type x, value_type y, value_type w, value_type h) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_bottom_right(const point_type& p, const size_type& s) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_top_left(value_type x, value_type y, value_type w, value_type h) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_top_left(const point_type& p, const size_type& s) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_top_right(value_type x, value_type y, value_type w, value_type h) noexcept;

        FST_NODISCARD static inline constexpr rect create_from_top_right(const point_type& p, const size_type& s) noexcept;

        ~rect() noexcept = default;

        rect& operator=(const rect&) noexcept = default;
        rect& operator=(rect&&) noexcept = default;

        inline constexpr rect& set_x(value_type _x) noexcept;
        inline constexpr rect& set_y(value_type _y) noexcept;
        inline constexpr rect& set_width(value_type w) noexcept;
        inline constexpr rect& set_height(value_type h) noexcept;
        inline constexpr rect& set_position(const point_type& point) noexcept;
        inline constexpr rect& set_size(const size_type& s) noexcept;

        inline constexpr rect& add_x(value_type _x) noexcept;
        inline constexpr rect& add_y(value_type _y) noexcept;
        inline constexpr rect& add_width(value_type w) noexcept;
        inline constexpr rect& add_height(value_type h) noexcept;
        inline constexpr rect& add_point(const point_type& point) noexcept;
        inline constexpr rect& add_size(const size_type& s) noexcept;

        inline constexpr rect& mul_x(value_type _x) noexcept;
        inline constexpr rect& mul_y(value_type _y) noexcept;
        inline constexpr rect& mul_width(value_type w) noexcept;
        inline constexpr rect& mul_height(value_type h) noexcept;

        FST_NODISCARD inline constexpr rect with_x(value_type _x) const noexcept;
        FST_NODISCARD inline constexpr rect with_y(value_type _y) const noexcept;
        FST_NODISCARD inline constexpr rect with_width(value_type w) const noexcept;
        FST_NODISCARD inline constexpr rect with_height(value_type h) const noexcept;
        FST_NODISCARD inline constexpr rect with_position(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_size(const size_type& s) const noexcept;

        FST_NODISCARD inline constexpr rect with_top_left(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_top_right(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_bottom_left(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_bottom_right(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_middle(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_middle_left(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_middle_right(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_middle_top(const point_type& point) const noexcept;
        FST_NODISCARD inline constexpr rect with_middle_bottom(const point_type& point) const noexcept;

        inline constexpr rect& operator+=(const point_type& pos) noexcept;
        inline constexpr rect& operator-=(const point_type& pos) noexcept;

        FST_NODISCARD inline constexpr rect operator+(const point_type& pos) const noexcept;
        FST_NODISCARD inline constexpr rect operator-(const point_type& pos) const noexcept;

        FST_NODISCARD inline constexpr value_type left() const noexcept;
        FST_NODISCARD inline constexpr value_type right() const noexcept;
        FST_NODISCARD inline constexpr value_type top() const noexcept;
        FST_NODISCARD inline constexpr value_type bottom() const noexcept;

        FST_NODISCARD inline constexpr point_type top_left() const noexcept;
        FST_NODISCARD inline constexpr point_type top_right() const noexcept;
        FST_NODISCARD inline constexpr point_type top_right(value_type dx, value_type dy) const noexcept;
        FST_NODISCARD inline constexpr point_type bottom_left() const noexcept;
        FST_NODISCARD inline constexpr point_type bottom_right() const noexcept;
        FST_NODISCARD inline constexpr point_type middle() const noexcept;
        FST_NODISCARD inline constexpr point_type middle_left() const noexcept;
        FST_NODISCARD inline constexpr point_type middle_right() const noexcept;
        FST_NODISCARD inline constexpr point_type middle_top() const noexcept;
        FST_NODISCARD inline constexpr point_type middle_bottom() const noexcept;
        FST_NODISCARD inline constexpr point_type next_left(value_type delta) const noexcept;
        FST_NODISCARD inline constexpr point_type next_left(const point_type& dt) const noexcept;
        FST_NODISCARD inline constexpr point_type next_right(value_type delta) const noexcept;
        FST_NODISCARD inline constexpr point_type next_right(const point_type& dt) const noexcept;
        FST_NODISCARD inline constexpr point_type next_down(value_type delta) const noexcept;
        FST_NODISCARD inline constexpr point_type next_down(const point_type& dt) const noexcept;
        FST_NODISCARD inline constexpr point_type next_up(value_type delta) const noexcept;
        FST_NODISCARD inline constexpr point_type next_up(const point_type& dt) const noexcept;

        FST_NODISCARD inline constexpr bool operator==(const rect& r) const noexcept;
        FST_NODISCARD inline constexpr bool operator!=(const rect& r) const noexcept;

        FST_NODISCARD inline constexpr bool contains(const point_type& pos) const noexcept;

        inline constexpr rect<T>& reduce(const point_type& pt) noexcept;
        FST_NODISCARD inline constexpr rect reduced(const point_type& pt) const noexcept;

        inline constexpr rect<T>& expand(const point_type& pt) noexcept;
        FST_NODISCARD inline constexpr rect expanded(const point_type& pt) const noexcept;

        FST_NODISCARD inline constexpr bool intersects(const rect& r) const noexcept;
        FST_NODISCARD inline constexpr bool intersects(const point_type& p) const noexcept;

        FST_NODISCARD inline constexpr value_type area() const noexcept;

        FST_NODISCARD inline constexpr rect get_union(const rect& rhs) const noexcept;

        inline constexpr rect<T>& merge(const rect& rhs) noexcept;
        FST_NODISCARD inline constexpr rect merged(const rect& rhs) const noexcept;

        FST_NODISCARD inline constexpr rect intersection(const rect& rhs) const noexcept;

        FST_NODISCARD inline constexpr rect get_fitted_rect(const rect& r) const noexcept;

        inline void swap(rect& w) noexcept;

        template <typename U>
        inline void swap(rect<U>& w) noexcept;

        template <typename RectType, detail::enable_if_rect_os<RectType> = nullptr>
        FST_NODISCARD inline RectType convert() const;

        template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
        FST_NODISCARD inline RectType convert() const;

        template <typename RectType, detail::enable_if_rect_XYWH<RectType> = nullptr>
        FST_NODISCARD inline RectType convert() const;

        template <typename RectType, detail::enable_if_rect_ltrb<RectType> = nullptr>
        FST_NODISCARD inline RectType convert() const;

        template <typename RectType, detail::enable_if_rect_os<RectType> = nullptr>
        FST_NODISCARD inline explicit operator RectType() const;

        template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
        FST_NODISCARD inline explicit operator RectType() const;

        template <typename RectType, detail::enable_if_rect_XYWH<RectType> = nullptr>
        FST_NODISCARD inline explicit operator RectType() const;

        template <typename RectType, detail::enable_if_rect_ltrb<RectType> = nullptr>
        FST_NODISCARD inline explicit operator RectType() const;

        // template <typename U>
        // inline friend __fst::ostream& operator<<(__fst::ostream& s, const __fst::rect<U>& rect);
    };

    static_assert(__fst::is_trivial<rect<int>>::value, "__fst::rect must remain a trivial type");
    static_assert(__fst::is_trivial<rect<float>>::value, "__fst::rect must remain a trivial type");

    template <typename T>
    rect(T, T, T, T) -> rect<T>;

    template <typename T1, typename T2, typename T3, typename T4>
    rect(T1, T2, T3, T4) -> rect<__fst::common_type_t<T1, T2, T3, T4>>;

    template <typename RectType, detail::enable_if_rect_os<RectType> = nullptr>
    rect(const RectType&) -> rect<decltype(RectType{}.origin.x)>;

    template <typename RectType, detail::enable_if_rect_xywh<RectType> = nullptr>
    rect(const RectType&) -> rect<decltype(RectType{}.x)>;

    template <typename RectType, detail::enable_if_rect_XYWH<RectType> = nullptr>
    rect(const RectType&) -> rect<decltype(RectType{}.X)>;

    template <typename RectType, detail::enable_if_rect_ltrb<RectType> = nullptr>
    rect(const RectType&) -> rect<decltype(RectType{}.left)>;

    template <typename _Tp>
    class quad
    {
      public:
        using value_type = _Tp;
        static_assert(__fst::is_arithmetic_v<value_type>, "value_type is not arithmetic");

        using point_type = __fst::point<value_type>;

        point_type top_left;
        point_type top_right;
        point_type bottom_right;
        point_type bottom_left;

        quad() noexcept = default;
        quad(const quad&) noexcept = default;
        quad(quad&&) noexcept = default;

        inline constexpr quad(const point_type& tl, const point_type& tr, const point_type& br, const point_type& bl) noexcept;

        inline constexpr quad(const __fst::rect<value_type>& r) noexcept;

        ~quad() noexcept = default;

        quad& operator=(const quad& p) noexcept = default;
        quad& operator=(quad&& p) noexcept = default;

        template <typename U>
        inline constexpr quad& operator=(const quad<U>& q) noexcept;

        FST_NODISCARD inline constexpr bool operator==(const quad& q) const noexcept;

        FST_NODISCARD inline constexpr bool operator!=(const quad& q) const noexcept;

        // friend __fst::ostream& operator<<(__fst::ostream& stream, const quad& p) {
        //   stream << "[{" << p.top_left << "}, {" << p.top_right << "}, {" << p.bottom_right << "}, {" << p.bottom_left
        //          << "}]";
        //   return stream;
        // }
    };

    ///
    ///
    /// [ a  b  tx ]
    /// [ c  d  ty ]
    /// [ 0  0  1  ]
    ///
    template <typename T>
    class transform
    {
      public:
        using value_type = T;
        static_assert(__fst::is_floating_point<T>::value, "__fst::transform value_type must be floating point");

        transform() noexcept = default;
        transform(const transform&) noexcept = default;
        transform(transform&&) noexcept = default;

        inline constexpr transform(value_type _a, value_type _b, value_type _c, value_type _d, value_type _tx, value_type _ty) noexcept;

        FST_NODISCARD static inline constexpr transform identity() noexcept;

        FST_NODISCARD static inline constexpr transform translation(const __fst::point<value_type>& p) noexcept;

        FST_NODISCARD static inline constexpr transform scale(const __fst::size<value_type>& s) noexcept;

        /// return a transform which rotates by `angle' radians.
        FST_NODISCARD static inline transform rotation(value_type angle) noexcept;

        FST_NODISCARD static inline transform rotation(value_type angle, const __fst::point<value_type>& p) noexcept;

        ~transform() noexcept = default;

        transform& operator=(const transform&) noexcept = default;
        transform& operator=(transform&&) noexcept = default;

        inline constexpr transform& translated(const __fst::point<value_type>& p) noexcept;
        inline constexpr transform& scaled(const __fst::size<value_type>& s) noexcept;
        inline transform& rotated(value_type angle) noexcept;

        FST_NODISCARD inline constexpr transform with_translation(const __fst::point<value_type>& p) const noexcept;
        FST_NODISCARD inline constexpr transform with_scale(const __fst::size<value_type>& s) const noexcept;
        FST_NODISCARD inline transform with_rotation(value_type angle) const noexcept;

        FST_NODISCARD inline constexpr transform operator*(const transform& t) const noexcept;
        FST_NODISCARD inline constexpr transform operator+(const __fst::point<value_type>& p) const noexcept;
        FST_NODISCARD inline constexpr transform operator-(const __fst::point<value_type>& p) const noexcept;
        FST_NODISCARD inline constexpr transform operator*(const __fst::size<value_type>& s) const noexcept;

        inline constexpr transform& operator*=(const transform& t) noexcept;
        inline constexpr transform& operator*=(const __fst::size<value_type>& s) noexcept;
        inline constexpr transform& operator+=(const __fst::point<value_type>& p) noexcept;
        inline constexpr transform& operator-=(const __fst::point<value_type>& p) noexcept;

        FST_NODISCARD inline constexpr __fst::point<value_type> apply(const __fst::point<value_type>& p) const noexcept;
        FST_NODISCARD inline constexpr __fst::quad<value_type> apply(const __fst::rect<value_type>& r) const noexcept;
        FST_NODISCARD inline constexpr __fst::quad<value_type> apply(const __fst::quad<value_type>& q) const noexcept;

        template <typename transform_type>
        FST_NODISCARD inline operator transform_type() const;

        value_type a, b, c, d, tx, ty;
    };

    static_assert(__fst::is_trivial<transform<float>>::value, "__fst::transform must remain a trivial type");
    static_assert(__fst::is_trivial<transform<double>>::value, "__fst::transform must remain a trivial type");

    template <typename T>
    FST_NODISCARD inline constexpr __fst::point<T> operator*(const __fst::point<T>& p, const transform<T>& t) noexcept;

    template <typename T>
    FST_NODISCARD inline constexpr __fst::quad<T> operator*(const __fst::rect<T>& p, const transform<T>& t) noexcept;

    template <typename T>
    FST_NODISCARD inline constexpr __fst::quad<T> operator*(const __fst::quad<T>& q, const transform<T>& t) noexcept;

    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //
    //
    //
    //
    // MARK: - IMPLEMENTATION -
    //
    //
    //
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------

    //
    // MARK: - point -
    //

    // template <typename T>
    // inline constexpr point<T>::point(value_type X, value_type Y) noexcept : x(X), y(Y) {}

    /// Construct a Point from PointType with member x and y.
    template <typename T>
    template <typename PointType, detail::enable_if_point_xy<PointType>>
    inline constexpr point<T>::point(const PointType& point) noexcept
        : x(static_cast<value_type>(point.x))
        , y(static_cast<value_type>(point.y))
    {}

    /// Construct a Point from PointType with member X and Y.
    template <typename T>
    template <typename PointType, detail::enable_if_point_XY<PointType>>
    inline constexpr point<T>::point(const PointType& point) noexcept
        : x(static_cast<value_type>(point.X))
        , y(static_cast<value_type>(point.Y))
    {}

    template <typename T>
    inline constexpr point<T> point<T>::operator+(value_type v) const noexcept
    {
        return point(x + v, y + v);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator-(value_type v) const noexcept
    {
        return point(x - v, y - v);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator*(value_type v) const noexcept
    {
        return point(x * v, y * v);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator/(value_type v) const noexcept
    {
        return point(x / v, y / v);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator+(const point& pt) const noexcept
    {
        return point(x + pt.x, y + pt.y);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator-(const point& pt) const noexcept
    {
        return point(x - pt.x, y - pt.y);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator*(const point& pt) const noexcept
    {
        return point(x * pt.x, y * pt.y);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator/(const point& pt) const noexcept
    {
        return point(x / pt.x, y / pt.y);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator+=(value_type v) noexcept
    {
        return *this = (*this + v);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator-=(value_type v) noexcept
    {
        return *this = (*this - v);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator*=(value_type v) noexcept
    {
        return *this = (*this * v);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator/=(value_type v) noexcept
    {
        return *this = (*this / v);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator+=(const point& pt) noexcept
    {
        return *this = (*this + pt);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator-=(const point& pt) noexcept
    {
        return *this = (*this - pt);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator*=(const point& pt) noexcept
    {
        return *this = (*this * pt);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::operator/=(const point& pt) noexcept
    {
        return *this = (*this / pt);
    }

    template <typename T>
    inline constexpr bool point<T>::operator==(const point& pt) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return __fst::fcompare(x, pt.x) && __fst::fcompare(y, pt.y); }
        else { return x == pt.x && y == pt.y; }
    }

    template <typename T>
    inline constexpr bool point<T>::operator!=(const point& pt) const noexcept
    {
        return !operator==(pt);
    }

    template <typename T>
    inline constexpr bool point<T>::operator<(const point& pt) const noexcept
    {
        return (x < pt.x && y < pt.y);
    }
    template <typename T>
    inline constexpr bool point<T>::operator<=(const point& pt) const noexcept
    {
        return (x <= pt.x && y <= pt.y);
    }
    template <typename T>
    inline constexpr bool point<T>::operator>(const point& pt) const noexcept
    {
        return (x > pt.x && y > pt.y);
    }
    template <typename T>
    inline constexpr bool point<T>::operator>=(const point& pt) const noexcept
    {
        return (x >= pt.x && y >= pt.y);
    }

    template <typename T>
    inline constexpr point<T> point<T>::operator-() const noexcept
    {
        return { -x, -y };
    }

    template <typename T>
    inline constexpr point<T>& point<T>::set_x(value_type _x) noexcept
    {
        x = _x;
        return *this;
    }

    template <typename T>
    inline constexpr point<T>& point<T>::set_y(value_type _y) noexcept
    {
        y = _y;
        return *this;
    }

    template <typename T>
    inline constexpr point<T>& point<T>::add_x(value_type dx) noexcept
    {
        return set_x(x + dx);
    }

    template <typename T>
    inline constexpr point<T>& point<T>::add_y(value_type dy) noexcept
    {
        return set_y(y + dy);
    }

    template <typename T>
    inline constexpr point<T> point<T>::with_x(value_type _x) const noexcept
    {
        return point(_x, y);
    }

    template <typename T>
    inline constexpr point<T> point<T>::with_y(value_type _y) const noexcept
    {
        return point(x, _y);
    }

    template <typename T>
    inline constexpr point<T> point<T>::with_add_x(value_type dx) const noexcept
    {
        return point(x + dx, y);
    }

    template <typename T>
    inline constexpr point<T> point<T>::with_add_y(value_type dy) const noexcept
    {
        return point(x, y + dy);
    }

    template <typename T>
    template <typename PointType, detail::enable_if_point_xy<PointType>>
    inline point<T>::operator PointType() const
    {
        using Type = decltype(PointType{}.x);
        return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
    }

    template <typename T>
    template <typename PointType, detail::enable_if_point_XY<PointType>>
    inline point<T>::operator PointType() const
    {
        using Type = decltype(PointType{}.X);
        return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
    }

    template <typename T>
    template <typename PointType, detail::enable_if_point_xy<PointType>>
    inline PointType point<T>::convert() const
    {
        using Type = decltype(PointType{}.x);
        return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
    }

    template <typename T>
    template <typename PointType, detail::enable_if_point_XY<PointType>>
    inline PointType point<T>::convert() const
    {
        using Type = decltype(PointType{}.X);
        return PointType{ static_cast<Type>(x), static_cast<Type>(y) };
    }

    // template <typename T>
    // inline __fst::ostream& operator<<(__fst::ostream& s, const __fst::point<T>& point) {
    // return s << '{' << point.x << ',' << point.y << '}';
    // }

    //
    // MARK: - size -
    //

    template <typename T>
    inline constexpr size<T>::size(value_type W, value_type H) noexcept
        : width(W)
        , height(H)
    {}

    template <typename T>
    template <typename SizeType, detail::enable_if_size_wh<SizeType>>
    inline constexpr size<T>::size(const SizeType& s) noexcept
        : width(static_cast<value_type>(s.width))
        , height(static_cast<value_type>(s.height))
    {}

    template <typename T>
    template <typename SizeType, detail::enable_if_size_WH<SizeType>>
    inline constexpr size<T>::size(const SizeType& s) noexcept
        : width(static_cast<value_type>(s.Width))
        , height(static_cast<value_type>(s.Height))
    {}

    template <typename T>
    inline constexpr size<T> size<T>::full_scale()
    {
        return { __fst::numeric_limits<value_type>::max(), __fst::numeric_limits<value_type>::max() };
    }

    template <typename T>
    inline constexpr size<T> size<T>::zero()
    {
        return { 0, 0 };
    }

    template <typename T>
    inline constexpr size<T> size<T>::operator+(value_type v) const noexcept
    {
        return { width + v, height + v };
    }
    template <typename T>
    inline constexpr size<T> size<T>::operator-(value_type v) const noexcept
    {
        return { width - v, height - v };
    }
    template <typename T>
    inline constexpr size<T> size<T>::operator*(value_type v) const noexcept
    {
        return { width * v, height * v };
    }
    template <typename T>
    inline constexpr size<T> size<T>::operator/(value_type v) const noexcept
    {
        return { width / v, height / v };
    }
    template <typename T>
    inline constexpr size<T> size<T>::operator+(const size& s) const noexcept
    {
        return { width + s.width, height + s.height };
    }
    template <typename T>
    inline constexpr size<T> size<T>::operator-(const size& s) const noexcept
    {
        return { width - s.width, height - s.height };
    }
    template <typename T>
    inline constexpr size<T> size<T>::operator*(const size& s) const noexcept
    {
        return { width * s.width, height * s.height };
    }
    template <typename T>
    inline constexpr size<T> size<T>::operator/(const size& s) const noexcept
    {
        return { width / s.width, height / s.height };
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator+=(value_type v) noexcept
    {
        return *this = (*this + v);
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator-=(value_type v) noexcept
    {
        return *this = (*this - v);
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator*=(value_type v) noexcept
    {
        return *this = (*this * v);
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator/=(value_type v) noexcept
    {
        return *this = (*this / v);
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator+=(const size& s) noexcept
    {
        return *this = (*this + s);
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator-=(const size& s) noexcept
    {
        return *this = (*this - s);
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator*=(const size& s) noexcept
    {
        return *this = (*this * s);
    }
    template <typename T>
    inline constexpr size<T>& size<T>::operator/=(const size& s) noexcept
    {
        return *this = (*this / s);
    }

    template <typename T>
    inline constexpr bool size<T>::operator==(const size& s) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return __fst::fcompare(width, s.width) && __fst::fcompare(height, s.height); }
        else { return (width == s.width && height == s.height); }
    }

    template <typename T>
    inline constexpr bool size<T>::operator!=(const size& s) const noexcept
    {
        return !operator==(s);
    }

    template <typename T>
    inline constexpr bool size<T>::operator<(const size& s) const noexcept
    {
        return (width < s.width && height < s.height);
    }
    template <typename T>
    inline constexpr bool size<T>::operator<=(const size& s) const noexcept
    {
        return (width <= s.width && height <= s.height);
    }
    template <typename T>
    inline constexpr bool size<T>::operator>(const size& s) const noexcept
    {
        return (width > s.width && height > s.height);
    }
    template <typename T>
    inline constexpr bool size<T>::operator>=(const size& s) const noexcept
    {
        return (width >= s.width && height >= s.height);
    }

    //
    template <typename T>
    inline constexpr size<T> size<T>::operator-() const noexcept
    {
        return { -width, -height };
    }

    template <typename T>
    inline constexpr size<T>& size<T>::set_width(value_type w) noexcept
    {
        width = w;
        return *this;
    }

    template <typename T>
    inline constexpr size<T>& size<T>::set_height(value_type h) noexcept
    {
        height = h;
        return *this;
    }

    template <typename T>
    inline constexpr size<T>& size<T>::add_width(value_type dw) noexcept
    {
        return set_width(width + dw);
    }

    template <typename T>
    inline constexpr size<T>& size<T>::add_height(value_type dh) noexcept
    {
        return set_height(height + dh);
    }

    template <typename T>
    inline constexpr size<T> size<T>::with_width(value_type w) const noexcept
    {
        return size(w, height);
    }

    template <typename T>
    inline constexpr size<T> size<T>::with_height(value_type h) const noexcept
    {
        return size(width, h);
    }

    template <typename T>
    inline constexpr size<T> size<T>::with_add_width(value_type dw) const noexcept
    {
        return size(width + dw, height);
    }

    template <typename T>
    inline constexpr size<T> size<T>::with_add_height(value_type dh) const noexcept
    {
        return size(width, height + dh);
    }

    template <typename T>
    FST_NODISCARD inline constexpr bool size<T>::empty() const noexcept
    {
        return width == 0 && height == 0;
    }

    template <typename T>
    template <typename SizeType, detail::enable_if_size_wh<SizeType>>
    inline size<T>::operator SizeType() const
    {
        using Type = decltype(SizeType{}.width);
        return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
    }

    template <typename T>
    template <typename SizeType, detail::enable_if_size_WH<SizeType>>
    inline size<T>::operator SizeType() const
    {
        using Type = decltype(SizeType{}.Width);
        return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
    }

    template <typename T>
    template <typename SizeType, detail::enable_if_size_wh<SizeType>>
    inline SizeType size<T>::convert() const
    {
        using Type = decltype(SizeType{}.width);
        return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
    }

    template <typename T>
    template <typename SizeType, detail::enable_if_size_WH<SizeType>>
    inline SizeType size<T>::convert() const
    {
        using Type = decltype(SizeType{}.Width);
        return SizeType{ static_cast<Type>(width), static_cast<Type>(height) };
    }

    //template <typename T>
    // inline __fst::ostream& operator<<(__fst::ostream& s, const __fst::size<T>& size) {
    // return s << '{' << size.width << ',' << size.height << '}';
    // }

    //
    // MARK: - rect -
    //

    template <typename T>
    inline constexpr rect<T>::rect(value_type x, value_type y, value_type w, value_type h) noexcept
        : origin{ x, y }
        , size{ w, h }
    {}

    template <typename T>
    inline constexpr rect<T>::rect(value_type x, value_type y, const size_type& s) noexcept
        : origin{ x, y }
        , size(s)
    {}

    template <typename T>
    template <typename RectType, detail::enable_if_rect_os<RectType>>
    inline constexpr rect<T>::rect(const RectType& rect) noexcept
        : origin{ static_cast<value_type>(rect.origin.x), static_cast<value_type>(rect.origin.y) }
        , size{ static_cast<value_type>(rect.size.width), static_cast<value_type>(rect.size.height) }
    {}

    template <typename T>
    template <typename RectType, detail::enable_if_rect_xywh<RectType>>
    inline constexpr rect<T>::rect(const RectType& rect) noexcept
        : origin{ static_cast<value_type>(rect.x), static_cast<value_type>(rect.y) }
        , size{ static_cast<value_type>(rect.width), static_cast<value_type>(rect.height) }
    {}

    template <typename T>
    template <typename RectType, detail::enable_if_rect_XYWH<RectType>>
    inline constexpr rect<T>::rect(const RectType& rect) noexcept
        : origin{ static_cast<value_type>(rect.X), static_cast<value_type>(rect.Y) }
        , size{ static_cast<value_type>(rect.Width), static_cast<value_type>(rect.Height) }
    {}

    template <typename T>
    template <typename RectType, detail::enable_if_rect_ltrb<RectType>>
    inline constexpr rect<T>::rect(const RectType& rect) noexcept
        : origin{ static_cast<value_type>(rect.left), static_cast<value_type>(rect.top) }
        , size{ static_cast<value_type>(rect.right - rect.left), static_cast<value_type>(rect.bottom - rect.top) }
    {}

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_point(const point_type& topLeft, const point_type& bottomRight) noexcept
    {
        return rect(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_bottom_left(value_type x, value_type y, value_type w, value_type h) noexcept
    {
        return rect(x, y - h, w, h);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_bottom_left(const point_type& p, const size_type& s) noexcept
    {
        return rect(p.x, p.y - s.height, s.width, s.height);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_bottom_right(value_type x, value_type y, value_type w, value_type h) noexcept
    {
        return rect(x - w, y - h, w, h);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_bottom_right(const point_type& p, const size_type& s) noexcept
    {
        return rect(p.x - s.width, p.y - s.height, s.width, s.height);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_top_left(value_type x, value_type y, value_type w, value_type h) noexcept
    {
        return rect(x, y, w, h);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_top_left(const point_type& p, const size_type& s) noexcept
    {
        return rect(p.x, p.y, s.width, s.height);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_top_right(value_type x, value_type y, value_type w, value_type h) noexcept
    {
        return rect(x - w, y, w, h);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::create_from_top_right(const point_type& p, const size_type& s) noexcept
    {
        return rect(p.x - s.width, p.y, s.width, s.height);
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::set_x(value_type _x) noexcept
    {
        x = _x;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::set_y(value_type _y) noexcept
    {
        y = _y;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::set_width(value_type w) noexcept
    {
        width = w;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::set_height(value_type h) noexcept
    {
        height = h;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::set_position(const point_type& point) noexcept
    {
        origin = point;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::set_size(const size_type& s) noexcept
    {
        size = s;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_top_left(const point_type& point) const noexcept
    {
        return { point, size };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_top_right(const point_type& p) const noexcept
    {
        return { p - point_type{ width, static_cast<value_type>(0) }, size };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_bottom_left(const point_type& p) const noexcept
    {
        return { p - point_type{ static_cast<value_type>(0), height }, size };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_bottom_right(const point_type& p) const noexcept
    {
        return { p - point_type{ width, height }, size };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_middle(const point_type& point) const noexcept
    {
        return { static_cast<value_type>(point.x - width * 0.5), static_cast<value_type>(point.y - height * 0.5), width, height };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_middle_left(const point_type& point) const noexcept
    {
        return { point.x, point.y - height * 0.5, width, height };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_middle_right(const point_type& point) const noexcept
    {
        return { point.x - width, static_cast<value_type>(point.y - height * 0.5), width, height };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_middle_top(const point_type& point) const noexcept
    {
        return { static_cast<value_type>(point.x - width * 0.5), point.y, width, height };
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_middle_bottom(const point_type& point) const noexcept
    {
        return { static_cast<value_type>(point.x - width * 0.5), point.y - height, width, height };
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::add_x(value_type _x) noexcept
    {
        x += _x;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::add_y(value_type _y) noexcept
    {
        y += _y;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::add_width(value_type w) noexcept
    {
        width += w;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::add_height(value_type h) noexcept
    {
        height += h;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::add_point(const point_type& point) noexcept
    {
        x += point.x;
        y += point.y;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::add_size(const size_type& s) noexcept
    {
        width += s.width;
        height += s.height;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::mul_x(value_type _x) noexcept
    {
        x *= _x;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::mul_y(value_type _y) noexcept
    {
        y *= _y;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::mul_width(value_type w) noexcept
    {
        width *= w;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::mul_height(value_type h) noexcept
    {
        height *= h;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_x(value_type _x) const noexcept
    {
        return rect(point_type{ _x, y }, size);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_y(value_type _y) const noexcept
    {
        return rect(point_type{ x, _y }, size);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_width(value_type w) const noexcept
    {
        return rect(origin, size_type{ w, height });
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_height(value_type h) const noexcept
    {
        return rect(origin, size_type{ width, h });
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_position(const point_type& point) const noexcept
    {
        return rect(point, size);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::with_size(const size_type& s) const noexcept
    {
        return rect(origin, s);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::operator+(const point_type& pos) const noexcept
    {
        return rect(x + pos.x, y + pos.y, width, height);
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::operator-(const point_type& pos) const noexcept
    {
        return rect(x - pos.x, y - pos.y, width, height);
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::operator+=(const point_type& pos) noexcept
    {
        x += pos.x;
        y += pos.y;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::operator-=(const point_type& pos) noexcept
    {
        x -= pos.x;
        y -= pos.y;
        return *this;
    }

    template <typename T>
    inline constexpr typename rect<T>::value_type rect<T>::left() const noexcept
    {
        return x;
    }

    template <typename T>
    inline constexpr typename rect<T>::value_type rect<T>::right() const noexcept
    {
        return x + width;
    }

    template <typename T>
    inline constexpr typename rect<T>::value_type rect<T>::top() const noexcept
    {
        return y;
    }

    template <typename T>
    inline constexpr typename rect<T>::value_type rect<T>::bottom() const noexcept
    {
        return y + height;
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::top_left() const noexcept
    {
        return origin;
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::top_right() const noexcept
    {
        return { x + width, y };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::top_right(value_type dx, value_type dy) const noexcept
    {
        return point_type{ x + width + dx, y + dy };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::bottom_left() const noexcept
    {
        return point_type{ x, y + height };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::bottom_right() const noexcept
    {
        return point_type{ x + width, y + height };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::middle() const noexcept
    {

        return point_type{ static_cast<value_type>(x + width * 0.5), static_cast<value_type>(y + height * 0.5) };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::middle_left() const noexcept
    {
        return point_type{ x, static_cast<value_type>(y + height * 0.5) };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::middle_right() const noexcept
    {
        return point_type{ x + width, static_cast<value_type>(y + height * 0.5) };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::middle_top() const noexcept
    {
        return point_type{ static_cast<value_type>(x * 0.5), y };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::middle_bottom() const noexcept
    {
        return point_type{ static_cast<value_type>(x * 0.5), y + height };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_left(value_type delta) const noexcept
    {
        return point_type{ x - delta, y };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_left(const point_type& dt) const noexcept
    {
        return point_type{ x - dt.x, y + dt.y };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_right(value_type delta) const noexcept
    {
        return point_type{ x + width + delta, y };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_right(const point_type& dt) const noexcept
    {
        return point_type{ x + width + dt.x, y + dt.y };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_down(value_type delta) const noexcept
    {
        return point_type{ x, y + height + delta };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_down(const point_type& dt) const noexcept
    {
        return point_type{ x + dt.x, y + height + dt.y };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_up(value_type delta) const noexcept
    {
        return point_type{ x, y - delta };
    }

    template <typename T>
    inline constexpr typename rect<T>::point_type rect<T>::next_up(const point_type& dt) const noexcept
    {
        return point_type{ x + dt.x, y - dt.y };
    }

    template <typename T>
    inline constexpr bool rect<T>::operator==(const rect& r) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>)
        {
            return __fst::fcompare(x, r.x) && __fst::fcompare(y, r.y) && __fst::fcompare(width, r.width) && __fst::fcompare(height, r.height);
        }
        else { return x == r.x && y == r.y && width == r.width && height == r.height; }

        //  return x == r.x && y == r.y && width == r.width && height == r.height;
    }

    template <typename T>
    inline constexpr bool rect<T>::operator!=(const rect& r) const noexcept
    {
        return !operator==(r);
    }

    template <typename T>
    inline constexpr bool rect<T>::contains(const point_type& pos) const noexcept
    {
        return pos.x >= x && pos.x <= x + width && pos.y >= y && pos.y <= y + height;
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::reduce(const point_type& pt) noexcept
    {
        x += pt.x;
        y += pt.y;
        width -= static_cast<value_type>(2) * pt.x;
        height -= static_cast<value_type>(2) * pt.y;
        return *this;
    }

    template <typename T>
    inline constexpr rect<T> rect<T>::reduced(const point_type& pt) const noexcept
    {
        return rect(x + pt.x, y + pt.y, width - static_cast<value_type>(2 * pt.x), height - static_cast<value_type>(2 * pt.y));
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::expand(const point_type& pt) noexcept
    {
        x -= pt.x;
        y -= pt.y;
        width += static_cast<value_type>(2) * pt.x;
        height += static_cast<value_type>(2) * pt.y;
        return *this;
    }

    template <typename T>
    FST_NODISCARD inline constexpr rect<T> rect<T>::expanded(const point_type& pt) const noexcept
    {
        return rect(x - pt.x, y - pt.y, width + static_cast<value_type>(2 * pt.x), height + static_cast<value_type>(2 * pt.y));
    }

    template <typename T>
    inline constexpr bool rect<T>::intersects(const rect& r) const noexcept
    {
        return ((__fst::minimum(right(), r.right()) - __fst::maximum(x, r.x)) > 0) && ((__fst::minimum(bottom(), r.bottom()) - __fst::maximum(y, r.y)) > 0);
    }

    template <typename T>
    inline constexpr bool rect<T>::intersects(const point_type& p) const noexcept
    {
        return ((__fst::minimum(right(), p.x + __fst::numeric_limits<value_type>::epsilon()) - __fst::maximum(x, p.x)) >= 0)
               && ((__fst::minimum(bottom(), p.y + __fst::numeric_limits<value_type>::epsilon()) - __fst::maximum(y, p.y)) >= 0);
    }

    template <typename T>
    inline constexpr typename rect<T>::value_type rect<T>::area() const noexcept
    {
        return size.width * size.height;
    }

    template <typename T>
    FST_NODISCARD inline constexpr rect<T> rect<T>::get_union(const rect& rhs) const noexcept
    {
        value_type nx = __fst::minimum(x, rhs.x);
        value_type ny = __fst::minimum(y, rhs.y);
        return { nx, ny, __fst::maximum(right(), rhs.right()) - nx, __fst::maximum(bottom(), rhs.bottom()) - ny };
    }

    template <typename T>
    inline constexpr rect<T>& rect<T>::merge(const rect& rhs) noexcept
    {
        value_type nx = __fst::minimum(x, rhs.x);
        value_type ny = __fst::minimum(y, rhs.y);
        *this = { nx, ny, __fst::maximum(right(), rhs.right()) - nx, __fst::maximum(bottom(), rhs.bottom()) - ny };
        return *this;
    }

    template <typename T>
    FST_NODISCARD inline constexpr rect<T> rect<T>::merged(const rect& rhs) const noexcept
    {
        value_type nx = __fst::minimum(x, rhs.x);
        value_type ny = __fst::minimum(y, rhs.y);
        return { nx, ny, __fst::maximum(right(), rhs.right()) - nx, __fst::maximum(bottom(), rhs.bottom()) - ny };
    }

    template <typename T>
    FST_NODISCARD inline constexpr rect<T> rect<T>::intersection(const rect& rhs) const noexcept
    {
        value_type nx = __fst::maximum(x, rhs.x);
        value_type nw = __fst::minimum(right(), rhs.right()) - nx;

        if (nw < 0) { return { 0, 0, 0, 0 }; }

        value_type ny = __fst::maximum(y, rhs.y);
        value_type nh = __fst::minimum(bottom(), rhs.bottom()) - ny;

        if (nh < 0) { return { 0, 0, 0, 0 }; }

        return { nx, ny, nw, nh };
    }

    template <typename T>
    FST_NODISCARD inline constexpr rect<T> rect<T>::get_fitted_rect(const rect& r) const noexcept
    {
        if (width < height)
        {
            double hRatio = r.height / static_cast<double>(r.width);
            return r.with_size({ width, static_cast<value_type>(hRatio * width) });
        }
        else
        {
            double wRatio = r.width / static_cast<double>(r.height);
            return r.with_size({ static_cast<value_type>(wRatio * height), height });
        }
    }

    template <typename T>
    inline void rect<T>::swap(rect<T> & r) noexcept
    {
        rect tmp = r;
        r = *this;
        *this = tmp;
    }

    template <typename T>
    template <typename U>
    inline void rect<T>::swap(rect<U> & r) noexcept
    {
        rect tmp = r;
        r = *this;
        *this = tmp;
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_os<RectType>>
    inline rect<T>::operator RectType() const
    {
        using Type = decltype(RectType{}.origin.x);
        return RectType{ { static_cast<Type>(x), static_cast<Type>(y) }, { static_cast<Type>(width), static_cast<Type>(height) } };
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_xywh<RectType>>
    inline rect<T>::operator RectType() const
    {
        using Type = decltype(RectType{}.x);
        return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_XYWH<RectType>>
    inline rect<T>::operator RectType() const
    {
        using Type = decltype(RectType{}.X);
        return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_ltrb<RectType>>
    inline rect<T>::operator RectType() const
    {
        using Type = decltype(RectType{}.left);
        return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(x + width), static_cast<Type>(y + height) };
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_os<RectType>>
    inline RectType rect<T>::convert() const
    {
        using Type = decltype(RectType{}.origin.x);
        return RectType{ { static_cast<Type>(x), static_cast<Type>(y) }, { static_cast<Type>(width), static_cast<Type>(height) } };
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_xywh<RectType>>
    inline RectType rect<T>::convert() const
    {
        using Type = decltype(RectType{}.x);
        return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_XYWH<RectType>>
    inline RectType rect<T>::convert() const
    {
        using Type = decltype(RectType{}.X);
        return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(width), static_cast<Type>(height) };
    }

    template <typename T>
    template <typename RectType, detail::enable_if_rect_ltrb<RectType>>
    inline RectType rect<T>::convert() const
    {
        using Type = decltype(RectType{}.left);
        return RectType{ static_cast<Type>(x), static_cast<Type>(y), static_cast<Type>(x + width), static_cast<Type>(y + height) };
    }

    // template <typename T>
    // inline __fst::ostream& operator<<(__fst::ostream& s, const __fst::rect<T>& rect) {
    // return s << '{' << rect.x << ',' << rect.y << ',' << rect.width << ',' << rect.height << '}';
    // }

    //
    // MARK: - range -
    //

    template <typename T>
    template <typename U>
    inline constexpr range<T>::range(const range<U>& r) noexcept
        : start(static_cast<value_type>(r.start))
        , end(static_cast<value_type>(r.end))
    {}

    template <typename T>
    constexpr range<T>::range(value_type _start, value_type _end) noexcept
        : start(_start)
        , end(_end)
    {}

    template <typename T>
    constexpr range<T> range<T>::with_length(value_type start, value_type len) noexcept
    {
        return { start, start + len };
    }

    template <typename T>
    template <typename U>
    inline constexpr range<T>& range<T>::operator=(const range<U>& r) noexcept
    {
        start = static_cast<value_type>(r.start);
        end = static_cast<value_type>(r.end);
        return *this;
    }

    template <typename T>
    constexpr range<T> range<T>::with_start(value_type s) const noexcept
    {
        return { s, end };
    }

    template <typename T>
    constexpr range<T> range<T>::with_end(value_type e) const noexcept
    {
        return { start, e };
    }

    template <typename T>
    constexpr range<T> range<T>::with_shifted_start(value_type delta) const noexcept
    {
        return { start + delta, end };
    }

    template <typename T>
    constexpr range<T> range<T>::with_shifted_end(value_type delta) const noexcept
    {
        return { start, end + delta };
    }

    template <typename T>
    constexpr range<T> range<T>::with_length(value_type len) const noexcept
    {
        return { start, start + len };
    }

    template <typename T>
    constexpr range<T> range<T>::with_shift(value_type delta) const noexcept
    {
        return { start + delta, end + delta };
    }

    template <typename T>
    constexpr range<T> range<T>::with_move(value_type s) const noexcept
    {
        return { s, s + length() };
    }

    template <typename T>
    inline constexpr range<T>& range<T>::set_start(value_type s) noexcept
    {
        start = s;
        return *this;
    }

    template <typename T>
    inline constexpr range<T>& range<T>::set_end(value_type e) noexcept
    {
        end = e;
        return *this;
    }

    template <typename T>
    inline constexpr range<T>& range<T>::move_to(value_type s) noexcept
    {
        value_type len = length();
        start = s;
        end = s + len;
        return *this;
    }

    template <typename T>
    inline constexpr range<T>& range<T>::shift(value_type delta) noexcept
    {
        start += delta;
        end += delta;
        return *this;
    }

    template <typename T>
    inline constexpr range<T>& range<T>::shift_start(value_type delta) noexcept
    {
        start += delta;
        return *this;
    }

    template <typename T>
    inline constexpr range<T>& range<T>::shift_end(value_type delta) noexcept
    {
        end += delta;
        return *this;
    }

    template <typename T>
    inline constexpr range<T>& range<T>::set_length(value_type len) noexcept
    {
        end = start + len;
        return *this;
    }

    template <typename T>
    constexpr typename range<T>::value_type range<T>::length() const noexcept
    {
        return end - start;
    }

    template <typename T>
    constexpr typename range<T>::value_type range<T>::middle() const noexcept
    {
        return static_cast<T>(start + (end - start) * 0.5);
    }

    template <typename T>
    constexpr bool range<T>::is_sorted() const noexcept
    {
        return start <= end;
    }

    template <typename T>
    constexpr bool range<T>::is_symmetric() const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return start == -end; }
        else { return start == -end; }
    }

    template <typename T>
    constexpr bool range<T>::contains(value_type x) const noexcept
    {
        return x >= start && x <= end;
    }

    template <typename T>
    constexpr bool range<T>::contains_closed(value_type x) const noexcept
    {
        return x >= start && x <= end;
    }

    template <typename T>
    constexpr bool range<T>::contains_opened(value_type x) const noexcept
    {
        return x > start && x < end;
    }

    template <typename T>
    constexpr bool range<T>::contains_left_opened(value_type x) const noexcept
    {
        return x > start && x <= end;
    }

    template <typename T>
    constexpr bool range<T>::contains_right_opened(value_type x) const noexcept
    {
        return x >= start && x < end;
    }

    template <typename T>
    constexpr bool range<T>::contains(const range& r) const noexcept
    {
        return contains(r.start) && contains(r.end);
    }

    template <typename T>
    FST_NODISCARD inline constexpr typename range<T>::value_type range<T>::clipped_value(value_type x) const noexcept
    {
        const value_type t = x < start ? start : x;
        return t > end ? end : t;
    }

    template <typename T>
    constexpr range<T>& range<T>::sort() noexcept
    {
        if (!is_sorted()) { __fst::mem_swap(start, end); }

        return *this;
    }

    template <typename T>
    constexpr bool range<T>::operator==(const range<T>& r) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return start == r.start && end == r.end; }
        else { return start == r.start && end == r.end; }
    }

    template <typename T>
    constexpr bool range<T>::operator!=(const range<T>& r) const noexcept
    {
        return !operator==(r);
    }

    template <typename T>
    constexpr bool range<T>::operator<(const range<T>& r) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return __fst::fcompare(start, r.start) ? length() < r.length() : start < r.start; }
        else { return start == r.start ? length() < r.length() : start < r.start; }
    }

    template <typename T>
    constexpr bool range<T>::operator<=(const range<T>& r) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return __fst::fcompare(start, r.start) ? length() <= r.length() : start <= r.start; }
        else { return start == r.start ? length() <= r.length() : start <= r.start; }
    }

    template <typename T>
    constexpr bool range<T>::operator>(const range<T>& r) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return __fst::fcompare(start, r.start) ? length() > r.length() : start > r.start; }
        else { return start == r.start ? length() > r.length() : start > r.start; }
    }

    template <typename T>
    constexpr bool range<T>::operator>=(const range<T>& r) const noexcept
    {
        if constexpr (__fst::is_floating_point_v<T>) { return __fst::fcompare(start, r.start) ? length() >= r.length() : start >= r.start; }
        else { return start == r.start ? length() >= r.length() : start >= r.start; }
    }

    // template <class T>
    // inline __fst::ostream& oper/ator<<(__fst::ostream& s, const range<T>& r) {
    // return s << '{' << r.start << ',' << r.end << '}';
    // }

    //
    // MARK: - padding -
    //

    template <typename T>
    constexpr padding<T>::padding(value_type t, value_type l, value_type b, value_type r) noexcept
        : top(t)
        , left(l)
        , bottom(b)
        , right(r)
    {}

    template <typename T>
    constexpr padding<T>::padding(value_type p) noexcept
        : top(p)
        , left(p)
        , bottom(p)
        , right(p)
    {}

    template <typename T>
    template <typename U>
    inline constexpr padding<T>::padding(const padding<U>& p) noexcept
        : top(static_cast<value_type>(p.top))
        , left(static_cast<value_type>(p.left))
        , bottom(static_cast<value_type>(p.bottom))
        , right(static_cast<value_type>(p.right))
    {}

    template <typename T>
    template <typename U>
    inline constexpr padding<T>& padding<T>::operator=(const padding<U>& p) noexcept
    {
        top = static_cast<value_type>(p.top);
        left = static_cast<value_type>(p.left);
        bottom = static_cast<value_type>(p.bottom);
        right = static_cast<value_type>(p.right);
        return *this;
    }

    template <typename T>
    __fst::rect<T> padding<T>::inside_rect(const __fst::rect<T>& rect) const noexcept
    {
        return __fst::rect<T>(rect.origin.x + left, rect.origin.y + top, rect.size.width - (left + right), rect.size.height - (top + bottom));
    }

    template <typename T>
    __fst::rect<T> padding<T>::outside_rect(const __fst::rect<T>& rect) const noexcept
    {
        return __fst::rect<T>(rect.origin.x - left, rect.origin.y - top, rect.size.width + left + right, rect.size.height + top + bottom);
    }

    template <typename T>
    constexpr bool padding<T>::empty() const noexcept
    {
        return top == 0 && left == 0 && bottom == 0 && right == 0;
    }

    template <typename T>
    constexpr bool padding<T>::operator==(const padding& p) const noexcept
    {
        return top == p.top && left == p.left && bottom == p.bottom && right == p.right;
    }

    template <typename T>
    constexpr bool padding<T>::operator!=(const padding& p) const noexcept
    {
        return !operator==(p);
    }

    // template <typename T>
    // __fst::ostream& operator<<(__fst::ostream& s, const padding<T>& p) {
    //   return s << '{' << p.top << ',' << p.left << ',' << p.bottom << ',' << p.right << '}';
    // }

    template <typename T>
    inline constexpr quad<T>::quad(const point_type& tl, const point_type& tr, const point_type& br, const point_type& bl) noexcept
        : top_left(tl)
        , top_right(tr)
        , bottom_right(br)
        , bottom_left(bl)

    {}

    template <typename T>
    inline constexpr quad<T>::quad(const __fst::rect<value_type>& r) noexcept
        : top_left(r.top_left())
        , top_right(r.top_right())
        , bottom_right(r.bottom_right())
        , bottom_left(r.bottom_left())
    {}

    template <typename T>
    template <typename U>
    inline constexpr quad<T>& quad<T>::operator=(const quad<U>& q) noexcept
    {
        top_left = q.top_left;
        top_right = q.top_right;
        bottom_right = q.bottom_right;
        bottom_left = q.bottom_left;
        return *this;
    }

    template <typename T>
    FST_NODISCARD inline constexpr bool quad<T>::operator==(const quad& q) const noexcept
    {
        return (top_left == q.top_left && top_right == q.top_right && bottom_right == q.bottom_right && bottom_left == q.bottom_left);
    }

    template <typename T>
    FST_NODISCARD inline constexpr bool quad<T>::operator!=(const quad& q) const noexcept
    {
        return !this->operator==(q);
    }

    template <typename T>
    constexpr transform<T>::transform(value_type _a, value_type _b, value_type _c, value_type _d, value_type _tx, value_type _ty) noexcept
        : a(_a)
        , b(_b)
        , c(_c)
        , d(_d)
        , tx(_tx)
        , ty(_ty)
    {}

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::identity() noexcept
    {
        return {
            static_cast<T>(1), //
            static_cast<T>(0), //
            static_cast<T>(0), //
            static_cast<T>(1), //
            static_cast<T>(0), //
            static_cast<T>(0) //
        };
    }
    /// [ a  b  tx ]
    /// [ c  d  ty ]
    /// [ 0  0  1  ]
    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::translation(const __fst::point<value_type>& p) noexcept
    {
        return {
            static_cast<T>(1), //
            static_cast<T>(0), //
            static_cast<T>(0), //
            static_cast<T>(1), //
            p.x, p.y //
        };
    }

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::scale(const __fst::size<value_type>& s) noexcept
    {
        return {
            s.width, //
            static_cast<T>(0), //
            static_cast<T>(0), //
            s.height, //
            static_cast<T>(0), //
            static_cast<T>(0) //
        };
    }

    template <typename T>
    FST_NODISCARD inline transform<T> transform<T>::rotation(value_type angle) noexcept
    {
        const T ca = __fst::cos(angle);
        const T sa = __fst::sin(angle);

        return {
            ca, //
            -sa, //
            sa, //
            ca, //
            static_cast<T>(0), //
            static_cast<T>(0) //
        };
    }

    template <typename T>
    FST_NODISCARD inline transform<T> transform<T>::rotation(value_type angle, const __fst::point<value_type>& p) noexcept
    {
        return translation(p) * rotation(angle) * translation(-p);
    }

    template <typename T>
    inline constexpr transform<T>& transform<T>::translated(const __fst::point<value_type>& p) noexcept
    {
        return *this += p;
    }

    template <typename T>
    inline constexpr transform<T>& transform<T>::scaled(const __fst::size<value_type>& s) noexcept
    {
        return *this *= s;
    }

    template <typename T>
    inline transform<T>& transform<T>::rotated(value_type angle) noexcept
    {
        return *this *= rotation(angle);
    }

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::with_translation(const __fst::point<value_type>& p) const noexcept
    {
        return *this + p;
    }

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::with_scale(const __fst::size<value_type>& s) const noexcept
    {
        return *this * s;
    }

    template <typename T>
    FST_NODISCARD inline transform<T> transform<T>::with_rotation(value_type angle) const noexcept
    {
        return *this * rotation(angle);
    }

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::operator*(const transform& t) const noexcept
    {
        return {
            a * t.a + b * t.c, //
            a * t.b + b * t.d, //
            c * t.a + d * t.c, //
            c * t.b + d * t.d, //
            tx + a * t.tx + b * t.ty, //
            ty + c * t.tx + d * t.ty //
        };
    }

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::operator+(const __fst::point<value_type>& p) const noexcept
    {
        return {
            a, //
            b, //
            c, //
            d, //
            tx + a * p.x + b * p.y, //
            ty + c * p.x + d * p.y //
        };
    }

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::operator-(const __fst::point<value_type>& p) const noexcept
    {
        return *this + -p;
    }

    template <typename T>
    FST_NODISCARD inline constexpr transform<T> transform<T>::operator*(const __fst::size<value_type>& s) const noexcept
    {
        return {
            a * s.width, //
            b * s.height, //
            c * s.width, //
            d * s.height, //
            tx, //
            ty //
        };
    }

    template <typename T>
    inline constexpr transform<T>& transform<T>::operator*=(const transform& t) noexcept
    {
        return *this = (*this * t);
    }

    template <typename T>
    inline constexpr transform<T>& transform<T>::operator+=(const __fst::point<value_type>& p) noexcept
    {
        return *this = (*this + p);
    }

    template <typename T>
    inline constexpr transform<T>& transform<T>::operator-=(const __fst::point<value_type>& p) noexcept
    {
        return *this = (*this - p);
    }

    template <typename T>
    inline constexpr transform<T>& transform<T>::operator*=(const __fst::size<value_type>& s) noexcept
    {
        return *this = (*this * s);
    }

    template <typename T>
    FST_NODISCARD inline constexpr __fst::point<T> transform<T>::apply(const __fst::point<value_type>& p) const noexcept
    {
        return { a * p.x + c * p.y + tx, b * p.x + d * p.y + ty };
    }

    template <typename T>
    FST_NODISCARD inline constexpr __fst::quad<T> transform<T>::apply(const __fst::rect<value_type>& r) const noexcept
    {
        return __fst::quad<T>(apply(r.position), apply(r.top_right()), apply(r.bottom_right()), apply(r.bottom_left()));
    }

    template <typename T>
    FST_NODISCARD inline constexpr __fst::quad<T> transform<T>::apply(const __fst::quad<value_type>& q) const noexcept
    {
        return __fst::quad<T>(apply(q.top_left), apply(q.top_right), apply(q.bottom_right), apply(q.bottom_left));
    }

    template <typename T>
    template <typename transform_type>
    FST_NODISCARD inline transform<T>::operator transform_type() const
    {
        return transform_type{ a, b, c, d, tx, ty };
    }

    template <typename T>
    FST_NODISCARD inline constexpr __fst::point<T> operator*(const __fst::point<T>& p, const transform<T>& t) noexcept
    {
        return t.apply(p);
    }

    template <typename T>
    FST_NODISCARD inline constexpr __fst::quad<T> operator*(const __fst::rect<T>& r, const transform<T>& t) noexcept
    {
        return t.apply(r);
    }

    template <typename T>
    FST_NODISCARD inline constexpr __fst::quad<T> operator*(const __fst::quad<T>& q, const transform<T>& t) noexcept
    {
        return t.apply(q);
    }

    template <class _CharT, class T>
    inline __fst::output_stream<_CharT>& operator<<(__fst::output_stream<_CharT>& stream, const __fst::point<T>& p) noexcept
    {
        stream << '{' << p.x << ',' << ' ' << p.y << '}';
        return stream;
    }

    template <class _CharT, class T>
    inline __fst::output_stream<_CharT>& operator<<(__fst::output_stream<_CharT>& stream, const __fst::size<T>& s) noexcept
    {
        stream << '{' << s.width << ',' << ' ' << s.height << '}';
        return stream;
    }

    template <class _CharT, class T>
    inline __fst::output_stream<_CharT>& operator<<(__fst::output_stream<_CharT>& stream, const __fst::rect<T>& r) noexcept
    {
        stream << '{' << r.x << ',' << ' ' << r.y << ',' << ' ' << r.width << ',' << ' ' << r.height << '}';
        return stream;
    }
FST_END_NAMESPACE
