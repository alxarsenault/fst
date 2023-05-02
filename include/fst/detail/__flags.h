
#define FST_DECLARE_ENUM_CLASS_OPERATORS(enum_class)                                           \
    inline enum_class operator|(enum_class lhs, enum_class rhs) noexcept                       \
    {                                                                                          \
        using type = __fst::underlying_type_t<enum_class>;                                      \
        return (enum_class) (static_cast<type>(lhs) | static_cast<type>(rhs));                 \
    }                                                                                          \
                                                                                               \
    inline enum_class operator&(enum_class lhs, enum_class rhs) noexcept                       \
    {                                                                                          \
        using type = __fst::underlying_type_t<enum_class>;                                      \
        return (enum_class) (static_cast<type>(lhs) & static_cast<type>(rhs));                 \
    }                                                                                          \
                                                                                               \
    inline enum_class& operator|=(enum_class& lhs, enum_class rhs) noexcept                    \
    {                                                                                          \
        return lhs = (lhs | rhs);                                                              \
    }                                                                                          \
                                                                                               \
    inline enum_class& operator&=(enum_class& lhs, enum_class rhs) noexcept                    \
    {                                                                                          \
        return lhs = (lhs & rhs);                                                              \
    }                                                                                          \
                                                                                               \
    inline enum_class& operator~(enum_class& lhs) noexcept                                     \
    {                                                                                          \
        using type = __fst::underlying_type_t<enum_class>;                                      \
        return lhs = (enum_class) (~type(lhs));                                                \
    }                                                                                          \
                                                                                               \
    inline bool operator==(enum_class lhs, __fst::underlying_type_t<enum_class> value) noexcept \
    {                                                                                          \
        return __fst::underlying_type_t<enum_class>(lhs) == value;                              \
    }                                                                                          \
                                                                                               \
    inline bool operator!=(enum_class lhs, __fst::underlying_type_t<enum_class> value) noexcept \
    {                                                                                          \
        return __fst::underlying_type_t<enum_class>(lhs) != value;                              \
    }                                                                                          \
                                                                                               \
    inline bool operator<(enum_class lhs, __fst::underlying_type_t<enum_class> value) noexcept  \
    {                                                                                          \
        return __fst::underlying_type_t<enum_class>(lhs) < value;                               \
    }                                                                                          \
                                                                                               \
    inline bool operator<(__fst::underlying_type_t<enum_class> lhs, enum_class rhs) noexcept    \
    {                                                                                          \
        return lhs < __fst::underlying_type_t<enum_class>(rhs);                                 \
    }                                                                                          \
                                                                                               \
    inline bool operator<=(enum_class lhs, __fst::underlying_type_t<enum_class> value) noexcept \
    {                                                                                          \
        return __fst::underlying_type_t<enum_class>(lhs) <= value;                              \
    }                                                                                          \
                                                                                               \
    inline bool operator<=(__fst::underlying_type_t<enum_class> lhs, enum_class rhs) noexcept   \
    {                                                                                          \
        return lhs <= __fst::underlying_type_t<enum_class>(rhs);                                \
    }                                                                                          \
                                                                                               \
    inline bool operator>(enum_class lhs, __fst::underlying_type_t<enum_class> value) noexcept  \
    {                                                                                          \
        return __fst::underlying_type_t<enum_class>(lhs) > value;                               \
    }                                                                                          \
                                                                                               \
    inline bool operator>(__fst::underlying_type_t<enum_class> lhs, enum_class rhs) noexcept    \
    {                                                                                          \
        return lhs > __fst::underlying_type_t<enum_class>(rhs);                                 \
    }                                                                                          \
                                                                                               \
    inline bool operator>=(enum_class lhs, __fst::underlying_type_t<enum_class> value) noexcept \
    {                                                                                          \
        return __fst::underlying_type_t<enum_class>(lhs) >= value;                              \
    }                                                                                          \
                                                                                               \
    inline bool operator>=(__fst::underlying_type_t<enum_class> lhs, enum_class rhs) noexcept   \
    {                                                                                          \
        return lhs >= __fst::underlying_type_t<enum_class>(rhs);                                \
    }                                                                                          \
                                                                                               \
    inline enum_class operator++(enum_class& ec) noexcept                                      \
    {                                                                                          \
        using type = __fst::underlying_type_t<enum_class>;                                      \
        ec = (enum_class) ((type) ec + 1);                                                     \
        return ec;                                                                             \
    }                                                                                          \
                                                                                               \
    inline enum_class operator++(enum_class ec, int) noexcept                                  \
    {                                                                                          \
        using type = __fst::underlying_type_t<enum_class>;                                      \
        ec = (enum_class) ((type) ec + 1);                                                     \
        return (enum_class) ((type) ec - 1);                                                   \
    }
