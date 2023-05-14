

# fst::conditional

```cpp
template <bool _Test, class _T1, class _T2>
struct conditional;
```

<sub><sup>Defined in header <type_traits></sup></sub>

Provides member typedef type, which is defined as `_T1` if `_Test` is `true` at compile time,
or as `_T2` if `_Test` is `false`.

## Member types

| Type      | Definition |
| ---        |         --- |
| type      | `_T1` if `_Test == true`, `_T2` if `_Test == false` |


## Helper type
```cpp
template <bool _Test, class _T1, class _T2>
using conditional_t = typename conditional<_Test, _T1, _T2>::type;
```