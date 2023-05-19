///
///
///

FST_BEGIN_NAMESPACE

    ///
    FST_INLINE_VAR constexpr size_t dynamic_size = (__fst::numeric_limits<size_t>::max)();

FST_END_NAMESPACE

#define FST_DECLARE_CONTAINER_TYPES(T_NAME)        \
    using value_type = T_NAME;                     \
    using size_type = size_t;                      \
    using difference_type = ptrdiff_t;             \
    using pointer = T_NAME*;                       \
    using const_pointer = const T_NAME*;           \
    using reference = T_NAME&;                     \
    using const_reference = __fst::cref_t<T_NAME>; \
    using iterator = pointer;                      \
    using const_iterator = const_pointer

#define FST_DECLARE_CONTAINER_DEFAULT_CTOR(NAME)               \
    constexpr NAME() noexcept = default;                       \
    constexpr NAME(const NAME&) noexcept = default;            \
    constexpr NAME(NAME&&) noexcept = default;                 \
    constexpr NAME& operator=(const NAME&) noexcept = default; \
    constexpr NAME& operator=(NAME&&) noexcept = default

//
#define FST_DECLARE_CONTAINER_DATA(DATE_ACCESS)                                                        \
    FST_NODISCARD inline constexpr iterator begin() noexcept                                           \
    {                                                                                                  \
        return iterator(data());                                                                       \
    }                                                                                                  \
    FST_NODISCARD inline constexpr const_iterator begin() const noexcept                               \
    {                                                                                                  \
        return const_iterator(data());                                                                 \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD inline constexpr iterator end() noexcept                                             \
    {                                                                                                  \
        return iterator(data() + this->size());                                                        \
    }                                                                                                  \
    FST_NODISCARD inline constexpr const_iterator end() const noexcept                                 \
    {                                                                                                  \
        return const_iterator(data() + this->size());                                                  \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD inline constexpr const_iterator cbegin() const noexcept                              \
    {                                                                                                  \
        return begin();                                                                                \
    }                                                                                                  \
    FST_NODISCARD inline constexpr const_iterator cend() const noexcept                                \
    {                                                                                                  \
        return end();                                                                                  \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD constexpr reference front() noexcept                                                 \
    {                                                                                                  \
        return data()[0];                                                                              \
    }                                                                                                  \
    FST_NODISCARD constexpr const_reference front() const noexcept                                     \
    {                                                                                                  \
        return data()[0];                                                                              \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD constexpr reference back() noexcept                                                  \
    {                                                                                                  \
        return data()[this->size() - 1];                                                               \
    }                                                                                                  \
    FST_NODISCARD constexpr const_reference back() const noexcept                                      \
    {                                                                                                  \
        return data()[this->size() - 1];                                                               \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD inline constexpr reference operator[](size_type index) noexcept                      \
    {                                                                                                  \
        return data()[index];                                                                          \
    }                                                                                                  \
    FST_NODISCARD inline constexpr const_reference operator[](size_type index) const noexcept          \
    {                                                                                                  \
        return data()[index];                                                                          \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD FST_ALWAYS_INLINE constexpr reference reversed(size_type index) noexcept             \
    {                                                                                                  \
        fst_assert(index < this->size(), "index out of bounds");                                       \
        return data()[this->size() - index - 1];                                                       \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD FST_ALWAYS_INLINE constexpr const_reference reversed(size_type index) const noexcept \
    {                                                                                                  \
        fst_assert(index < this->size(), "index out of bounds");                                       \
        return data()[this->size() - index - 1];                                                       \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD constexpr pointer data() noexcept                                                    \
    {                                                                                                  \
        return (pointer) DATE_ACCESS;                                                                  \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD constexpr const_pointer data() const noexcept                                        \
    {                                                                                                  \
        return (const_pointer) DATE_ACCESS;                                                            \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer data(size_type offset) noexcept                  \
    {                                                                                                  \
        fst_assert(data(), "access nullptr");                                                          \
        fst_assert(offset < this->size(), "offset out of bounds");                                     \
        return data() + offset;                                                                        \
    }                                                                                                  \
                                                                                                       \
    FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer data(size_type offset) const noexcept      \
    {                                                                                                  \
        fst_assert(data(), "access nullptr");                                                          \
        fst_assert(offset < this->size(), "offset out of bounds");                                     \
        return data() + offset;                                                                        \
    }                                                                                                  \
                                                                                                       \
    template <size_t I>                                                                                \
    FST_NODISCARD FST_ALWAYS_INLINE constexpr pointer data() noexcept                                  \
    {                                                                                                  \
        fst_assert(data(), "access nullptr");                                                          \
        fst_assert(I < this->size(), "offset out of bounds");                                          \
        return data() + I;                                                                             \
    }                                                                                                  \
                                                                                                       \
    template <size_t I>                                                                                \
    FST_NODISCARD FST_ALWAYS_INLINE constexpr const_pointer data() const noexcept                      \
    {                                                                                                  \
        fst_assert(data(), "access nullptr");                                                          \
        fst_assert(I < this->size(), "offset out of bounds");                                          \
        return data() + I;                                                                             \
    }
