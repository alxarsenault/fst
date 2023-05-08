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

#include "fst/os/common.h"
#include "fst/pointer.h"
#include "fst/geometry.h"

FST_BEGIN_SUB_NAMESPACE(os)

    class window;

    struct mouse_event
    {
        __fst::point<int> position;
        int click_count;
    };

    class window
    {
      public:
        using handle = void*;
        using pointer = __fst::unique_ptr<__fst::os::window, __fst::default_memory_zone, __fst::os_memory_category>;
        struct native;

        class delegate
        {
          public:
            virtual ~delegate() noexcept = default;

            virtual void on_window_create(window*) noexcept {}

            ///
            virtual void on_window_show() noexcept {}

            ///
            virtual void on_window_hide() noexcept {}

            ///
            virtual void on_window_close() noexcept {}

            ///
            virtual void on_window_move(__fst::rect<int>) noexcept {}

            ///
            virtual void on_window_resize(__fst::size<int>) noexcept {}

            virtual void on_mouse_enter(const mouse_event&) noexcept {}
            virtual void on_mouse_leave(const mouse_event&) noexcept {}
            virtual void on_mouse_move(const mouse_event&) noexcept {}

            virtual void on_mouse_left_down(const mouse_event&) noexcept {}
            virtual void on_mouse_left_up(const mouse_event&) noexcept {}
        };

        static pointer create(__fst::optional_ptr<delegate> d, const __fst::rect<int>& rect) noexcept;

        window(const window&) = delete;
        window(window&&) noexcept;

        virtual ~window() noexcept;
        
        window& operator=(const window&) = delete;
        window& operator=(window&&) noexcept;

        void show() noexcept;
        void hide() noexcept;

        bool capture_mouse() noexcept;
        void release_mouse() noexcept;

        void set_mouse_tracking(bool active) noexcept;
        bool has_mouse_tracking() const noexcept;

        handle get_handle() const noexcept;
        void run() noexcept;

        FST_NODISCARD inline delegate* get_delegate() noexcept { return _delegate.get(); }
        FST_NODISCARD inline const delegate* get_delegate() const noexcept { return _delegate.get(); }

      private:
        window(__fst::optional_ptr<delegate> d) noexcept;

        native* _native;
        __fst::optional_ptr<delegate> _delegate;
    };
FST_END_SUB_NAMESPACE
