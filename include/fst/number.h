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

    template <class T>
    struct arithmetic_number_constraint
    {
        FST_NODISCARD static constexpr bool static_constraint() noexcept { return __fst::is_arithmetic_v<T>; }
        FST_NODISCARD static constexpr bool constraint(T) noexcept { return true; }
    };

#if __FST_DEBUG_BUILD__
    template <class T, class _Constraint = arithmetic_number_constraint<T>>
    class number
    {
      public:
        using value_type = T;
        using constraint_type = _Constraint;

        static_assert(constraint_type::static_constraint(), "static constraint failed");

        number() noexcept = default;
        number(const number& other) noexcept = default;
        number(number&& other) noexcept = default;

        inline constexpr number(value_type v) noexcept
            : _value(v)
        {
            fst_assert(constraint_type::constraint(_value), "constraint failed");
        }

        template <typename U, class _UConstraint, typename = __fst::enable_if_t<__fst::is_convertible_v<U, value_type>>>
        inline constexpr number(number<U, _UConstraint> other) noexcept
            : _value((value_type) other.value())
        {
            fst_assert(constraint_type::constraint(_value), "constraint failed");
        }

        number& operator=(const number& other) noexcept = default;
        number& operator=(number&& other) noexcept = default;

        inline constexpr number& operator=(value_type v) noexcept
        {
            _value = v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        FST_NODISCARD inline constexpr value_type& value() noexcept
        {
            fst_assert(constraint_type::constraint(_value), "constraint failed");
            return _value;
        }

        FST_NODISCARD inline constexpr value_type value() const noexcept
        {
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return _value;
        }

        FST_NODISCARD inline constexpr operator value_type&() noexcept
        {
            fst_assert(constraint_type::constraint(_value), "constraint failed");
            return _value;
        }

        FST_NODISCARD inline constexpr operator value_type() const noexcept
        {
            fst_assert(constraint_type::constraint(_value), "constraint failed");
            return _value;
        }

        FST_NODISCARD inline explicit constexpr operator bool() const noexcept
        {
            fst_assert(constraint_type::constraint(_value), "constraint failed");
            return _value != 0;
        }

        inline constexpr number& operator++() noexcept
        {
            ++_value;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        inline constexpr number& operator--() noexcept
        {
            --_value;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        inline constexpr number operator++(int) noexcept { return _value++; }
        inline constexpr number operator--(int) noexcept { return _value--; }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator+=(U v) noexcept
        {
            _value += (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator-=(U v) noexcept
        {
            _value -= (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator*=(U v) noexcept
        {
            _value *= (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator/=(U v) noexcept
        {
            _value /= (value_type) v;
            return *this;
        }

        inline constexpr number& operator|=(number rhs) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value |= rhs.value();
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator|=(U v) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value |= (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        inline constexpr number& operator&=(number rhs) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value &= rhs.value();
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator&=(U v) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value &= (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        inline constexpr number& operator^=(number rhs) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value ^= rhs.value();
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator^=(U v) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value ^= (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        inline constexpr number& operator<<=(number rhs) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value <<= rhs.value();
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator<<=(U v) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value <<= (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        inline constexpr number& operator>>=(number rhs) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value >>= rhs.value();
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        template <class U = value_type, __fst::enable_if_t<__fst::is_convertible_v<U, value_type>, int> = 0>
        inline constexpr number& operator>>=(U v) noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");

            _value >>= (value_type) v;
            fst_assert(constraint_type::constraint(_value), "constraint failed");

            return *this;
        }

        inline constexpr number operator~() const noexcept
        {
            static_assert(__fst::is_integral_v<value_type>, "invalid operation");
            return ~_value;
        }

      private:
        value_type _value;
    };

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr bool operator==(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() == __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr bool operator!=(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() != __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr bool operator<(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() < __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr bool operator>(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() > __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr bool operator<=(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() <= __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr bool operator>=(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() >= __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator+(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() + __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator-(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() - __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator*(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() * __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator/(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        return __lhs.value() / __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator|(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");

        return __lhs.value() | __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator&(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() & __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator^(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() ^ __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator<<(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() << __rhs.value();
    }

    template <class T, class _Constraint, class _OtherConstraint>
    inline constexpr __fst::number<T> operator>>(number<T, _Constraint> __lhs, number<T, _OtherConstraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() >> __rhs.value();
    }

    //
    // Operator ==
    //

    template <class T, class _Constraint>
    inline constexpr bool operator==(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() == __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr bool operator==(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs == __rhs.value();
    }

    //
    // Operator !=
    //

    template <class T, class _Constraint>
    inline constexpr bool operator!=(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() != __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr bool operator!=(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs != __rhs.value();
    }

    //
    // Operator <
    //

    template <class T, class _Constraint>
    inline constexpr bool operator<(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() < __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr bool operator<(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs < __rhs.value();
    }

    //
    // Operator >
    //

    template <class T, class _Constraint>
    inline constexpr bool operator>(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() > __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr bool operator>(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs > __rhs.value();
    }

    //
    // Operator <=
    //

    template <class T, class _Constraint>
    inline constexpr bool operator<=(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() <= __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr bool operator<=(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs <= __rhs.value();
    }

    //
    // Operator >=
    //

    template <class T, class _Constraint>
    inline constexpr bool operator>=(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() >= __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr bool operator>=(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs >= __rhs.value();
    }

    //
    // Operator +
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator+(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() + __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator+(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs + __rhs.value();
    }

    //
    // Operator -
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator-(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() - __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator-(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs - __rhs.value();
    }

    //
    // Operator *
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator*(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() * __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator*(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs * __rhs.value();
    }

    //
    // Operator /
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator/(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        return __lhs.value() / __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator/(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        return __lhs / __rhs.value();
    }

    //
    // Operator |
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator|(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() | __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator|(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs | __rhs.value();
    }

    //
    // Operator &
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator&(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() & __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator&(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs & __rhs.value();
    }

    //
    // Operator ^
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator^(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() ^ __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator^(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs ^ __rhs.value();
    }

    //
    // Operator <<
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator<<(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() << __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator<<(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs << __rhs.value();
    }

    //
    // Operator >>
    //

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator>>(__fst::number<T, _Constraint> __lhs, T __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs.value() >> __rhs;
    }

    template <class T, class _Constraint>
    inline constexpr __fst::number<T, _Constraint> operator>>(T __lhs, __fst::number<T, _Constraint> __rhs) noexcept
    {
        static_assert(__fst::is_integral_v<T>, "invalid operation");
        return __lhs >> __rhs.value();
    }

#else
    template <class T, class _Constraint = arithmetic_number_constraint<T>>
    using number = T;
#endif

    ///
    template <class T>
    struct integer_number_constraint
    {
        static constexpr bool static_constraint() noexcept { return __fst::is_integral_v<T>; }
        static constexpr bool constraint(T) noexcept { return true; }
    };

    template <class T>
    using integer_number = number<T, integer_number_constraint<T>>;

    ///
    template <class T>
    struct unsigned_number_constraint
    {
        static constexpr bool static_constraint() noexcept { return __fst::is_unsigned_v<T>; }
        static constexpr bool constraint(T) noexcept { return true; }
    };

    template <class T>
    using unsigned_number = number<T, unsigned_number_constraint<T>>;

    ///
    template <class T>
    struct float_number_constraint
    {
        static constexpr bool static_constraint() noexcept { return __fst::is_floating_point_v<T>; }
        static constexpr bool constraint(T) noexcept { return true; }
    };

    template <class T>
    using float_number = number<T, float_number_constraint<T>>;

    ///
    template <class T>
    struct dsp_float_number_constraint
    {
        static constexpr bool static_constraint() noexcept { return __fst::is_floating_point_v<T>; }
        static constexpr bool constraint(T value) noexcept { return value >= (T) -1 && value <= (T) 1; }
    };

    template <class T>
    using dsp_float_number = number<T, dsp_float_number_constraint<T>>;

    ///
    template <class T>
    struct non_zero_number_constraint
    {
        static constexpr bool static_constraint() noexcept { return __fst::is_arithmetic_v<T>; }
        static constexpr bool constraint(T value) noexcept { return value != (T) 0; }
    };

    template <class T>
    using non_zero_number = number<T, non_zero_number_constraint<T>>;

    ///
    template <class T>
    struct positive_number_constraint
    {
        static constexpr bool static_constraint() noexcept { return __fst::is_arithmetic_v<T>; }
        static constexpr bool constraint(T value) noexcept { return value >= (T) 0; }
    };

    template <class T>
    using positive_number = number<T, positive_number_constraint<T>>;

    ///
    template <class T, T _Min, T _Max>
    struct bounded_number_constraint
    {
        static constexpr bool static_constraint() noexcept { return __fst::is_arithmetic_v<T>; }
        static constexpr bool constraint(T value) noexcept { return value >= _Min && value <= _Max; }
    };

    template <class T, T _Min, T _Max>
    using bounded_number = number<T, bounded_number_constraint<T, _Min, _Max>>;

FST_END_NAMESPACE
