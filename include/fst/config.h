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

#ifndef FST_USE_CUSTOM_CONFIG
#include "fst/config/config.h"
#endif // FST_USE_CUSTOM_CONFIG

//#define FST_USE_PROFILER_COLOR 1
//#ifdef FST_AUTO_CONFIG
//
//
//
//#else
//
//    // ---------------------------------------------
//    // Global
//    // ---------------------------------------------
//
//    //
//    #define FST_NAMESPACE fst
//
//    //
//    #define FST_NO_STD
//
//    // ---------------------------------------------
//    // Assert
//    // ---------------------------------------------
//
//    //
//    #define FST_ASSERT_HOOK(Expr, File, Line) FST_UNUSED(Expr)
//
//    //
//    #define FST_ASSERT_BREAK() FST_DEBUGTRAP()
//
//    //
//    #define FST_RELEASE_ASSERT
//
//    //
//    #define FST_ASSERT_COLOR
//
//    //
//    #define FST_ASSERT_NO_MSG
//
//    // ---------------------------------------------
//    // Memory
//    // ---------------------------------------------
//
//    //
//    #define FST_USE_RP_MALLOC
//
//    //
//    #define _ITERATOR_DEBUG_LEVEL 0
//
//    // ---------------------------------------------
//    // Profiler
//    // ---------------------------------------------
//
//    //
//    #define FST_PROFILE
//
//    //
//    #define FST_PROFILER_WARNING_IS_ERROR
//
//    // ---------------------------------------------
//    // Simd
//    // ---------------------------------------------
//
//    //
//    #define FST_SIMD_128
//
////
////#define FST_SIMD_256
//
////
////#define FST_SIMD_512
//
//#endif // FST_AUTO_CONFIG
