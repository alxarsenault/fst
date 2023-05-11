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
#include "fst/async/common.h"

FST_BEGIN_SUB_NAMESPACE(async)


class mutex
{
public:
  //typedef asio::detail::scoped_lock<win_mutex> scoped_lock;

  // Constructor.
  //ASIO_DECL win_mutex();

  //// Destructor.
  //~win_mutex()
  //{
  //  ::DeleteCriticalSection(&crit_section_);
  //}

  //// Lock the mutex.
  //void lock()
  //{
  //  ::EnterCriticalSection(&crit_section_);
  //}

  //// Unlock the mutex.
  //void unlock()
  //{
  //  ::LeaveCriticalSection(&crit_section_);
  //}

private:
  // Initialisation must be performed in a separate function to the constructor
  // since the compiler does not support the use of structured exceptions and
  // C++ exceptions in the same function.
  //ASIO_DECL int do_init();

  //::CRITICAL_SECTION crit_section_;
};
FST_END_SUB_NAMESPACE
