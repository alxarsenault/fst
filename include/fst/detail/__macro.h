
//
// MARK: Config.
//
#ifndef __has_include
#define __has_include(x) 0
#endif

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#ifndef __has_extension
#define __has_extension(x) 0
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
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
