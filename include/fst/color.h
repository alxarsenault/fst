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
#include "fst/math.h"
#include "fst/traits.h"
#include "fst/utility.h"

FST_BEGIN_NAMESPACE

    class color
    {
      public:
        enum class format {
            argb,
            rgba,
            abgr
        };

        template <typename T, __fst::enable_if_t<__fst::is_floating_point<T>::value, int> = 0>
        struct float_rgba
        {
            T r, g, b, a;
        };

        template <typename T, __fst::enable_if_t<__fst::is_floating_point<T>::value, int> = 0>
        struct float_rgb
        {
            T r, g, b;
        };

        template <typename T, __fst::enable_if_t<__fst::is_floating_point<T>::value, int> = 0>
        struct float_grey_alpha
        {
            T grey, alpha;
        };

        static inline constexpr color black() noexcept { return color(0x000000ff); }
        static inline constexpr color white() noexcept { return color(0xffffffff); }

        FST_PRAGMA_PUSH()
        FST_PRAGMA_DISABLE_WARNING_MSVC(26495)
        constexpr color() noexcept = default;
        FST_PRAGMA_POP()

        constexpr color(const color&) noexcept = default;
        constexpr color(color&&) noexcept = default;

        ~color() noexcept = default;

        constexpr color& operator=(const color&) noexcept = default;
        constexpr color& operator=(color&&) noexcept = default;

        FST_ALWAYS_INLINE constexpr color(uint32_t rgba) noexcept
            : _rgba(rgba)
        {}

        FST_ALWAYS_INLINE constexpr color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept
            : _rgba((uint32_t(a) << color_shift_alpha) | (uint32_t(b) << color_shift_blue) | (uint32_t(g) << color_shift_green) | (uint32_t(r) << color_shift_red))
        {}

        FST_ALWAYS_INLINE static constexpr color from_argb(uint32_t argb) noexcept
        {
            const color c(argb);
            return color(c.green(), c.blue(), c.alpha(), c.red());
        }

        template <typename T>
        inline constexpr color(const float_rgba<T>& rgba) noexcept
        {
            uint32_t ur = color_float_component_to_uint32(rgba.r);
            uint32_t ug = color_float_component_to_uint32(rgba.g);
            uint32_t ub = color_float_component_to_uint32(rgba.b);
            uint32_t ua = color_float_component_to_uint32(rgba.a);
            _rgba = (ua << color_shift_alpha) | (ub << color_shift_blue) | (ug << color_shift_green) | (ur << color_shift_red);
        }

        template <typename T>
        inline constexpr color(const float_rgb<T>& rgb) noexcept
        {
            uint32_t ur = color_float_component_to_uint32(rgb.r);
            uint32_t ug = color_float_component_to_uint32(rgb.g);
            uint32_t ub = color_float_component_to_uint32(rgb.b);
            _rgba = (255 << color_shift_alpha) | (ub << color_shift_blue) | (ug << color_shift_green) | (ur << color_shift_red);
        }

        template <typename T>
        inline constexpr color(const float_grey_alpha<T>& ga) noexcept
        {
            uint32_t u = color_float_component_to_uint32(ga.grey);
            uint32_t ua = color_float_component_to_uint32(ga.alpha);
            _rgba = (ua << color_shift_alpha) | (u << color_shift_blue) | (u << color_shift_green) | (u << color_shift_red);
        }

        template <typename T, size_t Size, __fst::enable_if_t<__fst::is_floating_point<T>::value, int> = 0>
        FST_ALWAYS_INLINE constexpr color(const T (&data)[Size]) noexcept
            : color(&data[0], Size)
        {}

        template <typename T, __fst::enable_if_t<__fst::is_floating_point<T>::value, int> = 0>
        inline constexpr color(const T* data, size_t size) noexcept
        {
            switch (size)
            {
            case 2: {
                uint32_t u = color_float_component_to_uint32(data[0]);
                uint32_t a = color_float_component_to_uint32(data[1]);
                _rgba = (a << color_shift_alpha) | (u << color_shift_blue) | (u << color_shift_green) | (u << color_shift_red);
            }
                return;
            case 3: {
                uint32_t ur = color_float_component_to_uint32(data[0]);
                uint32_t ug = color_float_component_to_uint32(data[1]);
                uint32_t ub = color_float_component_to_uint32(data[2]);
                _rgba = (255 << color_shift_alpha) | (ub << color_shift_blue) | (ug << color_shift_green) | (ur << color_shift_red);
            }
                return;
            case 4: {
                uint32_t ur = color_float_component_to_uint32(data[0]);
                uint32_t ug = color_float_component_to_uint32(data[1]);
                uint32_t ub = color_float_component_to_uint32(data[2]);
                uint32_t ua = color_float_component_to_uint32(data[3]);
                _rgba = (ua << color_shift_alpha) | (ub << color_shift_blue) | (ug << color_shift_green) | (ur << color_shift_red);
            }
                return;
            }
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint32_t& rgba() noexcept { return _rgba; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint32_t rgba() const noexcept { return _rgba; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint32_t argb() const noexcept
        {
            using u32 = uint32_t;
            return (u32(blue()) << color_shift_alpha) | (u32(green()) << color_shift_blue) | (u32(red()) << color_shift_green) | (u32(alpha()) << color_shift_red);
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint32_t abgr() const noexcept
        {
            using u32 = uint32_t;
            return (u32(red()) << color_shift_alpha) | (u32(green()) << color_shift_blue) | (u32(blue()) << color_shift_green) | (u32(alpha()) << color_shift_red);
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr float_rgba<float> f_rgba() const noexcept { return { f_red(), f_green(), f_blue(), f_alpha() }; }

        template <typename T, __fst::enable_if_t<__fst::is_floating_point<T>::value, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr float_rgba<T> f_rgba() const noexcept
        {
            return { static_cast<T>(f_red()), static_cast<T>(f_green()), static_cast<T>(f_blue()), static_cast<T>(f_alpha()) };
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint8_t red() const noexcept { return static_cast<uint8_t>((_rgba & color_bits_red) >> color_shift_red); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint8_t green() const noexcept { return static_cast<uint8_t>((_rgba & color_bits_green) >> color_shift_green); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint8_t blue() const noexcept { return static_cast<uint8_t>((_rgba & color_bits_blue) >> color_shift_blue); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint8_t alpha() const noexcept { return static_cast<uint8_t>((_rgba & color_bits_alpha) >> color_shift_alpha); }

        FST_ALWAYS_INLINE constexpr color& red(uint8_t r) noexcept
        {
            _rgba = (_rgba & ~color_bits_red) | (uint32_t(r) << color_shift_red);
            return *this;
        }

        FST_ALWAYS_INLINE constexpr color& green(uint8_t g) noexcept
        {
            _rgba = (_rgba & ~color_bits_green) | (uint32_t(g) << color_shift_green);
            return *this;
        }

        FST_ALWAYS_INLINE constexpr color& blue(uint8_t b) noexcept
        {
            _rgba = (_rgba & ~color_bits_blue) | (uint32_t(b) << color_shift_blue);
            return *this;
        }

        FST_ALWAYS_INLINE constexpr color& alpha(uint8_t a) noexcept
        {
            _rgba = (_rgba & ~color_bits_alpha) | (uint32_t(a) << color_shift_alpha);
            return *this;
        }

        template <format Format, __fst::enable_if_t<Format == format::rgba, int> = 0>
        FST_ALWAYS_INLINE constexpr color& red(uint8_t r) noexcept
        {
            return red(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::argb, int> = 0>
        FST_ALWAYS_INLINE constexpr color& red(uint8_t r) noexcept
        {
            return green(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::abgr, int> = 0>
        FST_ALWAYS_INLINE constexpr color& red(uint8_t r) noexcept
        {
            return alpha(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::rgba, int> = 0>
        FST_ALWAYS_INLINE constexpr color& green(uint8_t r) noexcept
        {
            return green(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::argb, int> = 0>
        FST_ALWAYS_INLINE constexpr color& green(uint8_t r) noexcept
        {
            return blue(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::abgr, int> = 0>
        FST_ALWAYS_INLINE constexpr color& green(uint8_t r) noexcept
        {
            return blue(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::rgba, int> = 0>
        FST_ALWAYS_INLINE constexpr color& blue(uint8_t r) noexcept
        {
            return blue(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::argb, int> = 0>
        FST_ALWAYS_INLINE constexpr color& blue(uint8_t r) noexcept
        {
            return alpha(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::abgr, int> = 0>
        FST_ALWAYS_INLINE constexpr color& blue(uint8_t r) noexcept
        {
            return green(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::rgba, int> = 0>
        FST_ALWAYS_INLINE constexpr color& alpha(uint8_t r) noexcept
        {
            return alpha(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::argb, int> = 0>
        FST_ALWAYS_INLINE constexpr color& alpha(uint8_t r) noexcept
        {
            return red(r);
        }

        template <format Format, __fst::enable_if_t<Format == format::abgr, int> = 0>
        FST_ALWAYS_INLINE constexpr color& alpha(uint8_t r) noexcept
        {
            return red(r);
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && __fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T red() const noexcept
        {
            return static_cast<T>(f_red());
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && !__fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T red() const noexcept
        {
            return static_cast<T>(red());
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && __fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T green() const noexcept
        {
            return static_cast<T>(f_green());
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && !__fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T green() const noexcept
        {
            return static_cast<T>(green());
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && __fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T blue() const noexcept
        {
            return static_cast<T>(f_blue());
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && !__fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T blue() const noexcept
        {
            return static_cast<T>(blue());
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && __fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T alpha() const noexcept
        {
            return static_cast<T>(f_alpha());
        }

        template <class T, __fst::enable_if_t<__fst::is_arithmetic_v<T> && !__fst::is_floating_point_v<T>, int> = 0>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr T alpha() const noexcept
        {
            return static_cast<T>(alpha());
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr float f_red() const noexcept { return red() / 255.0f; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr float f_green() const noexcept { return green() / 255.0f; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr float f_blue() const noexcept { return blue() / 255.0f; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr float f_alpha() const noexcept { return alpha() / 255.0f; }

        template <size_t Index>
        FST_NODISCARD FST_ALWAYS_INLINE constexpr uint8_t get() const noexcept
        {
            static_assert(Index <= 3, "out of bounds");
            if constexpr (Index == 0)
                return red();
            else if constexpr (Index == 1)
                return green();
            else if constexpr (Index == 2)
                return blue();
            else if constexpr (Index == 3)
                return alpha();
        }

        FST_NODISCARD FST_ALWAYS_INLINE uint8_t operator[](size_t index) const noexcept
        {
            fst_assert(index < 4);

            FST_PRAGMA_PUSH()
            FST_PRAGMA_DISABLE_WARNING_MSVC(6385) // READ_OVERRUN
            return components{ _rgba }.data[3 - index];
            FST_PRAGMA_POP()
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_opaque() const noexcept { return alpha() == 255; }
        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool is_transparent() const noexcept { return alpha() != 255; }

        FST_NODISCARD inline constexpr color darker(float amount) const noexcept
        {
            amount = 1.0f - __fst::clamp<float>(amount, 0.0f, 1.0f);
            return color(uint8_t(red() * amount), uint8_t(green() * amount), uint8_t(blue() * amount), alpha());
        }

        FST_NODISCARD inline color brighter(float amount) const noexcept
        {
            const float ratio = 1.0f / (1.0f + __fst::fabs(amount));
            const float mu = 255 * (1.0f - ratio);

            return color(static_cast<uint8_t>(mu + ratio * red()), // r
                static_cast<uint8_t>(mu + ratio * green()), // g
                static_cast<uint8_t>(mu + ratio * blue()), // b
                alpha() // a
            );
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr color with_red(uint8_t red) const noexcept { return color(red, green(), blue(), alpha()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr color with_green(uint8_t green) const noexcept { return color(red(), green, blue(), alpha()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr color with_blue(uint8_t blue) const noexcept { return color(red(), green(), blue, alpha()); }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr color with_alpha(uint8_t alpha) const noexcept { return color(red(), green(), blue(), alpha); }

        /// mu should be between [0, 1]
        FST_NODISCARD inline constexpr color operator*(float mu) const noexcept
        {
            fst_assert(mu >= 0.0f && mu <= 1.0f);
            return color(static_cast<uint8_t>(red() * mu), static_cast<uint8_t>(green() * mu), static_cast<uint8_t>(blue() * mu), static_cast<uint8_t>(alpha() * mu));
        }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool operator==(const color& c) const noexcept { return _rgba == c._rgba; }

        FST_NODISCARD FST_ALWAYS_INLINE constexpr bool operator!=(const color& c) const noexcept { return !operator==(c); }

      private:
        union components
        {
            uint32_t rgba;
            uint8_t data[4];
        };

        uint32_t _rgba;

        template <typename T>
        static inline uint32_t color_float_component_to_uint32(T f) noexcept
        {
            return static_cast<uint32_t>(__fst::floor(f * 255));
        }

        enum color_shift : uint32_t {
            color_shift_red = 24,
            color_shift_green = 16,
            color_shift_blue = 8,
            color_shift_alpha = 0
        };

        enum color_bits : uint32_t {
            color_bits_red = 0xff000000,
            color_bits_green = 0x00ff0000,
            color_bits_blue = 0x0000ff00,
            color_bits_alpha = 0x000000ff
        };
    };

    static_assert(__fst::is_trivial<__fst::color>::value, "fst::color must remain a trivial type");

FST_END_NAMESPACE
