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

class int24_t {
public:
    using int_type = int32_t;

private:
    uint8_t _data[3];

    struct int24_t_bit {
        int_type _int24 : 24;
    };

    inline int24_t_bit& to_24_bits() { return *(int24_t_bit*)(void*)(&_data[0]); }

    inline const int24_t_bit& to_24_bits() const { return *(int24_t_bit*)(void*)(&_data[0]); }

public:
    int24_t() noexcept = default;
    int24_t(const int24_t&) noexcept = default;
    int24_t(int24_t&&) noexcept = default;

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t(T integer) noexcept {
        to_24_bits()._int24 = (int_type)integer;
    }

    ~int24_t() noexcept = default;

    int24_t& operator=(const int24_t&) noexcept = default;
    int24_t& operator=(int24_t&&) noexcept = default;

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator=(T integer) noexcept {
        to_24_bits()._int24 = (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator+=(T integer) noexcept {
        to_24_bits()._int24 += (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator-=(T integer) noexcept {
        to_24_bits()._int24 -= (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator*=(T integer) noexcept {
        to_24_bits()._int24 *= (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator/=(T integer) noexcept {
        to_24_bits()._int24 /= (int_type)integer;
        return *this;
    }

    inline int24_t& operator++() noexcept {
        to_24_bits()._int24++;
        return *this;
    }

    inline int24_t operator++(int) noexcept {
        int24_t old = *this;
        operator++();
        return old;
    }

    inline int24_t& operator--() noexcept {
        to_24_bits()._int24--;
        return *this;
    }

    inline int24_t operator--(int) noexcept {
        int24_t old = *this;
        operator--();
        return old;
    }

    inline int24_t& operator|=(const int24_t& rhs) noexcept {
        to_24_bits()._int24 |= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator|=(T integer) noexcept {
        to_24_bits()._int24 |= (int_type)integer;
        return *this;
    }

    inline int24_t& operator&=(const int24_t& rhs) noexcept {
        to_24_bits()._int24 &= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator&=(T integer) noexcept {
        to_24_bits()._int24 &= (int_type)integer;
        return *this;
    }

    inline int24_t& operator^=(const int24_t& rhs) noexcept {
        to_24_bits()._int24 ^= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator^=(T integer) noexcept {
        to_24_bits()._int24 ^= (int_type)integer;
        return *this;
    }

    inline int24_t& operator<<=(const int24_t& rhs) noexcept {
        to_24_bits()._int24 <<= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator<<=(T integer) noexcept {
        to_24_bits()._int24 <<= (int_type)integer;
        return *this;
    }

    inline int24_t& operator>>=(const int24_t& rhs) noexcept {
        to_24_bits()._int24 >>= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline int24_t& operator>>=(T integer) noexcept {
        to_24_bits()._int24 >>= (int_type)integer;
        return *this;
    }

    inline int24_t operator~() const noexcept { return ~(to_24_bits()._int24); }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline T convert() const noexcept {
        return (T)to_24_bits()._int24;
    }

template <class T, __fst::enable_if_t<__fst::is_integral_v<T> && !__fst::is_same_v<__fst::remove_cvref_t<T>, bool>, int> = 0>
    inline operator T() const noexcept {
        return convert<T>();
    }

    inline explicit operator bool() const noexcept { return (bool)(int_type)(to_24_bits()._int24); }

    inline uint8_t operator[](size_t __index) const noexcept {
        fst_assert(__index < 3, "Out of bounds index");
        return _data[__index];
    }

    // friend inline __fst::ostream& operator<<(__fst::ostream& stream, const int24_t& value) {
    //     stream << (int_type)value;
    //     return stream;
    // }
};

static_assert(sizeof(int24_t) == 3, "int24_t size should be 3");

class uint24_t {
public:
    using int_type = uint32_t;

private:
    uint8_t _data[3];

    struct uint24_t_bit {
        int_type _int24 : 24;
    };

    inline uint24_t_bit& to_24_bits() { return *(uint24_t_bit*)(void*)(&_data[0]); }

    inline const uint24_t_bit& to_24_bits() const { return *(uint24_t_bit*)(void*)(&_data[0]); }



public:
    uint24_t() noexcept = default;
    uint24_t(const uint24_t&) noexcept = default;
    uint24_t(uint24_t&&) noexcept = default;

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t(T integer) noexcept {
        to_24_bits()._int24 = (int_type)integer;
    }

    ~uint24_t() noexcept = default;

    uint24_t& operator=(const uint24_t&) noexcept = default;
    uint24_t& operator=(uint24_t&&) noexcept = default;

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator=(T integer) noexcept {
        to_24_bits()._int24 = (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator+=(T integer) noexcept {
        to_24_bits()._int24 += (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator-=(T integer) noexcept {
        to_24_bits()._int24 -= (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator*=(T integer) noexcept {
        to_24_bits()._int24 *= (int_type)integer;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator/=(T integer) noexcept {
        to_24_bits()._int24 /= (int_type)integer;
        return *this;
    }

    inline uint24_t& operator++() noexcept {
        to_24_bits()._int24++;
        return *this;
    }

    inline uint24_t operator++(int) noexcept {
        uint24_t old = *this;
        operator++();
        return old;
    }

    inline uint24_t& operator--() noexcept {
        to_24_bits()._int24--;
        return *this;
    }

    inline uint24_t operator--(int) noexcept {
        uint24_t old = *this;
        operator--();
        return old;
    }

    inline uint24_t& operator|=(const uint24_t& rhs) noexcept {
        to_24_bits()._int24 |= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator|=(T integer) noexcept {
        to_24_bits()._int24 |= (int_type)integer;
        return *this;
    }

    inline uint24_t& operator&=(const uint24_t& rhs) noexcept {
        to_24_bits()._int24 &= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator&=(T integer) noexcept {
        to_24_bits()._int24 &= (int_type)integer;
        return *this;
    }

    inline uint24_t& operator^=(const uint24_t& rhs) noexcept {
        to_24_bits()._int24 ^= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator^=(T integer) noexcept {
        to_24_bits()._int24 ^= (int_type)integer;
        return *this;
    }

    inline uint24_t& operator<<=(const uint24_t& rhs) noexcept {
        to_24_bits()._int24 <<= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator<<=(T integer) noexcept {
        to_24_bits()._int24 <<= (int_type)integer;
        return *this;
    }

    inline uint24_t& operator>>=(const uint24_t& rhs) noexcept {
        to_24_bits()._int24 >>= (int_type)rhs;
        return *this;
    }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline uint24_t& operator>>=(T integer) noexcept {
        to_24_bits()._int24 >>= (int_type)integer;
        return *this;
    }

    inline uint24_t operator~() const noexcept { return ~(to_24_bits()._int24); }

    template <class T, __fst::enable_if_t<__fst::is_integral_v<T>, int> = 0>
    inline T convert() const noexcept {
        return (T)to_24_bits()._int24;
    }

template <class T, __fst::enable_if_t<__fst::is_integral_v<T> && !__fst::is_same_v<T, bool>, int> = 0>
    inline operator T() const noexcept {
        return convert<T>();
    }

    inline explicit operator bool() const noexcept { return (bool)(int_type)to_24_bits()._int24; }

    inline uint8_t operator[](size_t __index) const noexcept {
        fst_assert(__index < 3, "Out of bounds index");
        return _data[__index];
    }

    // friend inline __fst::ostream& operator<<(__fst::ostream& stream, const uint24_t& value) {
    //     stream << (int_type)value;
    //     return stream;
    // }
};

static_assert(sizeof(uint24_t) == 3, "uint24_t size should be 3");

FST_END_NAMESPACE


//
// Operator ==
//
//inline bool operator==(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
//    return __lhs.convert<__fst::int24_t::int_type>() == __rhs.convert<__fst::int24_t::int_type>();
//}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator==(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() == __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator==(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs == T(__rhs);
}

//
// Operator !=
//
inline bool operator!=(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() != __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator!=(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() != __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator!=(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs != T(__rhs);
}

//
// Operator <
//
inline bool operator<(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() < __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() < __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs < T(__rhs);
}

//
// Operator >
//
inline bool operator>(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() > __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() > __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs > T(__rhs);
}

//
// Operator <=
//
inline bool operator<=(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() <= __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<=(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() <= __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<=(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs <= T(__rhs);
}

//
// Operator >=
//
inline bool operator>=(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() >= __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>=(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() >= __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>=(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs >= T(__rhs);
}

//
// Operator +
//
inline __fst::int24_t operator+(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() + __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator+(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() + __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator+(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs + T(__rhs);
}

//
// Operator -
//
inline __fst::int24_t operator-(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() - __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator-(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() - __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator-(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs - T(__rhs);
}

//
// Operator *
//
//inline __fst::int24_t operator*(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
//    return __lhs.convert<__fst::int24_t::int_type>() * __rhs.convert<__fst::int24_t::int_type>();
//}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator*(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() * __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator*(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs * T(__rhs);
}

//
// Operator /
//
inline __fst::int24_t operator/(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() / __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator/(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() / __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator/(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs / T(__rhs);
}

//
// Operator |
//
inline __fst::int24_t operator|(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() | __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator|(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() | __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator|(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs | T(__rhs);
}

//
// Operator &
//
inline __fst::int24_t operator&(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() & __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator&(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() & __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator&(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs & T(__rhs);
}

//
// Operator ^
//
inline __fst::int24_t operator^(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() ^ __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator^(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() ^ __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator^(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs ^ T(__rhs);
}

//
// Operator <<
//
inline __fst::int24_t operator<<(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() << __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator<<(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() << __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator<<(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs << T(__rhs);
}

//
// Operator >>
//
inline __fst::int24_t operator>>(const __fst::int24_t& __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() >> __rhs.convert<__fst::int24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::int24_t operator>>(const __fst::int24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::int24_t::int_type>() >> __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator>>(T __lhs, const __fst::int24_t& __rhs) noexcept {
    return __lhs >> T(__rhs);
}


//
//
//

//
// Operator ==
//
//inline bool operator==(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
//    return __lhs.convert<__fst::uint24_t::int_type>() == __rhs.convert<__fst::uint24_t::int_type>();
//}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator==(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() == __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator==(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs == T(__rhs);
}

//
// Operator !=
//
inline bool operator!=(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() != __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator!=(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() != __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator!=(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs != T(__rhs);
}

//
// Operator <
//
inline bool operator<(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() < __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() < __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs < T(__rhs);
}

//
// Operator >
//
inline bool operator>(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() > __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() > __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs > T(__rhs);
}

//
// Operator <=
//
inline bool operator<=(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() <= __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<=(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() <= __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator<=(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs <= T(__rhs);
}

//
// Operator >=
//
inline bool operator>=(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() >= __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>=(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() >= __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline bool operator>=(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs >= T(__rhs);
}

//
// Operator +
//
inline __fst::uint24_t operator+(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() + __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator+(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() + __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator+(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs + T(__rhs);
}

//
// Operator -
//
inline __fst::uint24_t operator-(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() - __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator-(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() - __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator-(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs - T(__rhs);
}

//
// Operator *
//
inline __fst::uint24_t operator*(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() * __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator*(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() * __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator*(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs * T(__rhs);
}

//
// Operator /
//
inline __fst::uint24_t operator/(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() / __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator/(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() / __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator/(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs / T(__rhs);
}

//
// Operator |
//
inline __fst::uint24_t operator|(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() | __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator|(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() | __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator|(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs | T(__rhs);
}

//
// Operator &
//
inline __fst::uint24_t operator&(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() & __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator&(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() & __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator&(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs & T(__rhs);
}

//
// Operator ^
//
inline __fst::uint24_t operator^(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() ^ __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator^(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() ^ __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator^(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs ^ T(__rhs);
}

//
// Operator <<
//
inline __fst::uint24_t operator<<(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() << __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator<<(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() << __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator<<(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs << T(__rhs);
}

//
// Operator >>
//
inline __fst::uint24_t operator>>(const __fst::uint24_t& __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() >> __rhs.convert<__fst::uint24_t::int_type>();
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline __fst::uint24_t operator>>(const __fst::uint24_t& __lhs, T __rhs) noexcept {
    return __lhs.convert<__fst::uint24_t::int_type>() >> __rhs;
}

template <typename T, typename __fst::enable_if_t<__fst::is_integral_v<T>, T>>
inline T operator>>(T __lhs, const __fst::uint24_t& __rhs) noexcept {
    return __lhs >> T(__rhs);
}
