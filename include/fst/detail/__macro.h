///
///
///

#ifdef __has_include
#define FST_HAS_INCLUDE(x) __has_include(x)
#else
#define FST_HAS_INCLUDE(x) 0
#endif

#ifdef __has_feature
#define FST_HAS_FEATURE(x) __has_feature(x)
#else
#define FST_HAS_FEATURE(x) 0
#endif

#ifdef __has_extension
#define FST_HAS_EXTENSION(x) __has_extension(x)
#else
#define FST_HAS_EXTENSION(x) 0
#endif

#ifdef __has_attribute
#define FST_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define FST_HAS_ATTRIBUTE(x) 0
#endif

#ifdef __has_builtin
#define FST_HAS_BUILTIN(x) __has_builtin(x)
#else
#define FST_HAS_BUILTIN(x) 0
#endif

///
#define FST_STRINGIFY(s) __FST_STRINGIFY(s)
#define __FST_STRINGIFY(s) #s

///
#define FST_CONCAT(_X, _Y) __FST_CONCAT(_X, _Y)
#define __FST_CONCAT(_X, _Y) _X##_Y

///
#define FST_EXPAND(X) X
#define FST_DEFER(M, ...) M(__VA_ARGS__)

///
#define FST_IS_MACRO_EMPTY(...) FST_EXPAND(__FST_IS_MACRO_EMPTY(__FST_ARGS_DUMMY(__VA_ARGS__)))
#define __FST_ARGS_DUMMY(...) dummy, ##__VA_ARGS__
#define __FST_SELECT_5(_1, _2, _3, _4, _5, num, ...) num
#define __FST_IS_MACRO_EMPTY(...) FST_EXPAND(__FST_SELECT_5(__VA_ARGS__, 0, 0, 0, 0, 1))


#define FST_DECLARE_DEFAULT_CTOR(NAME)                         \
    constexpr NAME() noexcept = default;                       \
    constexpr NAME(const NAME&) noexcept = default;            \
    constexpr NAME(NAME&&) noexcept = default;                 \
    constexpr NAME& operator=(const NAME&) noexcept = default; \
    constexpr NAME& operator=(NAME&&) noexcept = default