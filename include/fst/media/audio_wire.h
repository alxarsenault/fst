//#pragma once
//
//#include "au/common.h"
//#include "au/memory_range.h"
//#include "au/utils.h"
//#include <algorithm>
//
//AU_BEGIN_NAMESPACE
//
//template <class T>
//inline void copy_buffer(T* dst, const T* src, size_t size) noexcept
//{
//    //au_assert(input_size <= size(), "copy() size out of bounds");
//    std::copy_n(src, size, dst);
//}
//
//template <typename T>
//class audio_wire : public _AU::memory_range<T>
//{
//  public:
//    using range_base = _AU::memory_range<T>;
//    using element_type = T;
//    using value_type = std::remove_cv_t<T>;
//    using size_type = typename range_base::size_type;
//    using reference = typename range_base::reference;
//    using const_reference = typename range_base::const_reference;
//    using pointer = typename range_base::pointer;
//    using const_pointer = typename range_base::const_pointer;
//    using iterator = typename range_base::iterator;
//    using const_iterator = typename range_base::const_iterator;
//
//    static_assert(std::is_floating_point_v<value_type>, "au::audio_wire only works with floating point value type.");
//
//  private:
//    using is_writable = std::bool_constant<!std::is_const_v<element_type>>;
//
//    //template <class OtherElementType>
//    //using enable_if_convertible_t
//    //= std::enable_if_t<std::is_convertible_v<OtherElementType(*)[], element_type(*)[]>, std::nullptr_t>;
//
//    //template <class OtherElementType>
//    //using enable_if_const_convertible_t
//    //= std::enable_if_t<std::is_convertible_v<const OtherElementType(*)[], const element_type(*)[]>, std::nullptr_t>;
//
//    //template <class Container>
//    //using enable_if_compatible_t
//    //= std::enable_if_t<detail::is_audio_wire_base_compatible_container<Container, T>::value, std::nullptr_t>;
//
//    template <bool Dummy, class D>
//    using cond = _AU::dependent_type_condition<Dummy, D>;
//
//    template <bool Dummy, class D>
//    using enable_cond = _AU::enable_if_dependant_same_t<Dummy, D>;
//
//    template <bool _Dummy, class _D = cond<_Dummy, is_writable>>
//    using enable_if_writable = enable_cond<_Dummy, _D>;
//
//#define AU_ENABLE_IF_WRITABLE bool _Dummy = true, class = enable_if_writable<_Dummy>
//
//  public:
//    using range_base::range_base;
//
//    inline bool valid() const noexcept { return (bool) !this->empty(); }
//
//    inline constexpr audio_wire subwire(size_type __offset, size_type __count = std::numeric_limits<size_type>::max()) const noexcept
//    {
//        return this->sub_range(__offset, __count);
//    }
//
//    //
//    //
//    //
//
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void clear() noexcept
//    {
//        std::fill(this->begin(), this->end(), (value_type) 0);
//    }
//
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void clear_n(size_type n) noexcept
//    {
//        au_assert(n <= size(), "clear_n() n out of bounds");
//        std::fill_n(this->begin(), n, (value_type) 0);
//    }
//
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void clear_n(size_type offset, size_type n) noexcept
//    {
//        au_assert(n <= size(), "clear_n() n out of bounds");
//        au_assert(offset < size(), "clear_n() offset out of bounds");
//        std::fill_n(this->data() + offset, _AU::minimum(size() - offset, n), (value_type) 0);
//    }
//
//    //
//    //
//    //
//
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void fill(value_type value) noexcept
//    {
//        std::fill(this->begin(), this->end(), value);
//    }
//
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void fill_n(value_type value, size_type n) noexcept
//    {
//        au_assert(n <= size(), "fill_n() n out of bounds");
//        std::fill_n(this->data(), _AU::minimum(size(), n), value);
//    }
//
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void fill_n(value_type value, size_type offset, size_type n) noexcept
//    {
//        au_assert(n <= size(), "fill_n() n out of bounds");
//        au_assert(offset < size(), "fill_n() offset out of bounds");
//        std::fill_n(this->data() + offset, _AU::minimum(size() - offset, n), value);
//    }
//
//    //
//    // Copy.
//    //
//
//    // wire[i] = input[i]
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void copy(audio_wire<const value_type> input) noexcept
//    {
//        au_assert(input.size() <= size(), "copy() size out of bounds");
//        std::copy_n(input.begin(), _AU::minimum(size(), input.size()), this->begin());
//    }
//
//    template <AU_ENABLE_IF_WRITABLE>
//    inline void copy(const value_type* input, size_t input_size) noexcept
//    {
//        au_assert(input_size <= size(), "copy() size out of bounds");
//        std::copy_n(input, _AU::minimum(size(), input_size), this->begin());
//    }
//};
//AU_END_NAMESPACE
