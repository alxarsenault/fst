#pragma once


/// fst::conditional
///
/// template <bool _Test, class _T1, class _T2>
/// struct conditional;
///
/// @description
/// Provides member typedef type, which is defined as `_T1` if `_Test` is `true` at compile time,
/// or as `_T2` if `_Test` is `false`.
///
/// @type type
/// `_T1` if `_Test == true`, `_T2` if `_Test == false`
///
/// @helper
/// template <bool _Test, class _T1, class _T2>
/// using conditional_t = typename conditional<_Test, _T1, _T2>::type;

class Bingo
{

};

/// k

