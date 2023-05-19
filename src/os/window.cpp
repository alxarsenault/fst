#include "fst/os/window.h"
#include "fst/stream.h"

#if __FST_WINDOWS__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>


FST_BEGIN_SUB_NAMESPACE(os)

    namespace
    {

        //template <class DERIVED_TYPE>
        //class BaseWindow
        //{
        //public:
        //
        //
        //    BaseWindow() : m_hwnd(NULL) { }
        //
        //    BOOL Create(
        //        const char* lpWindowName,
        //        DWORD dwStyle,
        //        DWORD dwExStyle = 0,
        //        int x = CW_USEDEFAULT,
        //        int y = CW_USEDEFAULT,
        //        int nWidth = CW_USEDEFAULT,
        //        int nHeight = CW_USEDEFAULT,
        //        HWND hWndParent = 0,
        //        HMENU hMenu = 0
        //        )
        //    {
        //        WNDCLASS wc = {0};
        //
        //        wc.lpfnWndProc   = DERIVED_TYPE::WindowProc;
        //        wc.hInstance     = GetModuleHandle(NULL);
        //        wc.lpszClassName = ClassName();
        //
        //        RegisterClass(&wc);
        //
        //        m_hwnd = CreateWindowExA(
        //            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
        //            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        //            );
        //
        //        return (m_hwnd ? TRUE : FALSE);
        //    }
        //
        //    HWND Window() const { return m_hwnd; }
        //
        //protected:
        //
        //    virtual const char*  ClassName() const = 0;
        //    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
        //
        //    HWND m_hwnd;
        //};
    }

    struct window::native
    {
        native(window* win) noexcept
            : _win(win)
            , _hwnd(nullptr)
        {
            fst::print(FST_PRETTY_FUNCTION);
        }

        ~native() noexcept { fst::print(FST_PRETTY_FUNCTION); }

        inline bool create(const __fst::rect<int>& rect)
        {
            WNDCLASS wc = { 0 };

            wc.lpfnWndProc = &native::WindowProc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = "Window";

            RegisterClass(&wc);

            DWORD dwExStyle = 0;
            DWORD dwStyle = WS_OVERLAPPEDWINDOW;
            _hwnd = CreateWindowExA(dwExStyle, "Window", "Win name", dwStyle, rect.x, rect.y, rect.width, rect.height, nullptr, nullptr, GetModuleHandleA(nullptr), this);

            //if()

            return (bool) _hwnd;
        }

        void show() noexcept { ShowWindow(_hwnd, true); }

        void hide() noexcept { ShowWindow(_hwnd, false); }

        bool capture_mouse() noexcept
        {

            SetCapture(_hwnd);

            return true;
        }

        void release_mouse() noexcept { ReleaseCapture(); }

        void reset_mouse_tracking() noexcept
        {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.hwndTrack = _hwnd;
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.dwHoverTime = HOVER_DEFAULT;
            TrackMouseEvent(&tme);
        }

        void set_mouse_tracking(bool active) noexcept
        {
            if (_wants_mouse_tracking == active) { return; }

            if (_wants_mouse_tracking)
            {
                _wants_mouse_tracking = false;
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.hwndTrack = _hwnd;
                tme.dwFlags = TME_HOVER | TME_LEAVE | TME_CANCEL;
                tme.dwHoverTime = HOVER_DEFAULT;
                TrackMouseEvent(&tme);
                return;
            }

            _wants_mouse_tracking = true;
            reset_mouse_tracking();
        }

        bool has_mouse_tracking() const noexcept { return _wants_mouse_tracking; }

        LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            switch (uMsg)
            {
            case WM_CREATE: {

                if (window::delegate* d = _win->get_delegate()) { d->on_window_create(_win); }

                return 0;
            }

            case WM_DESTROY:
                //DiscardGraphicsResources();
                //SafeRelease(&pFactory);
                PostQuitMessage(0);
                return 0;

            case WM_SHOWWINDOW: //
            {
                if (window::delegate* d = _win->get_delegate())
                {
                    if (IsWindowVisible(_hwnd)) { d->on_window_hide(); }
                    else { d->on_window_show(); }
                }
                return 0;
            }

            case WM_PAINT:
                //OnPaint();
                return 0;

                // Other messages not shown...

            case WM_MOVE: {
                if (window::delegate* d = _win->get_delegate())
                {
                    RECT rc;
                    GetWindowRect(_hwnd, &rc);
                    d->on_window_move(rc);

                    return 0;
                }
            }

            case WM_SIZE: {

                if (window::delegate* d = _win->get_delegate())
                {
                    RECT rc;
                    GetClientRect(_hwnd, &rc);
                    d->on_window_resize(__fst::rect<int>(rc).size);

                    return 0;
                }
            }

            case WM_LBUTTONDOWN: {
                if (window::delegate* d = _win->get_delegate())
                {

                    __fst::point<int> pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    d->on_mouse_left_down(os::mouse_event{ pos, 1 });
                }

                return 0;
            }
            case WM_LBUTTONUP: {
                if (window::delegate* d = _win->get_delegate())
                {

                    __fst::point<int> pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    d->on_mouse_left_up(os::mouse_event{ pos, 0 });
                }

                return 0;
            }
            case WM_LBUTTONDBLCLK: {
                if (window::delegate* d = _win->get_delegate())
                {

                    __fst::point<int> pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    d->on_mouse_left_down(os::mouse_event{ pos, 2 });
                }

                return 0;
            }

            case WM_MOUSEMOVE: {
                bool did_enter = false;
                if (!_is_mouse_hover)
                {
                    did_enter = true;
                    _is_mouse_hover = true;
                    if (_wants_mouse_tracking) { reset_mouse_tracking(); }
                }

                if (window::delegate* d = _win->get_delegate(); d && _wants_mouse_tracking)
                {
                    __fst::point<int> pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                    if (did_enter) { d->on_mouse_enter(os::mouse_event{ pos, 0 }); }
                    d->on_mouse_move(os::mouse_event{ pos, 0 });
                }

                return 0;
            }

            case WM_MOUSELEAVE: {
                _is_mouse_hover = false;

                if (window::delegate* d = _win->get_delegate())
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(_hwnd, &pt);
                    d->on_mouse_leave(os::mouse_event{ pt, 0 });
                }
                return 0;
            }
            }
            return DefWindowProc(_hwnd, uMsg, wParam, lParam);
        }

        void run()
        {
            MSG msg = {};
            while (GetMessage(&msg, nullptr, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        window* _win;
        HWND _hwnd;
        bool _wants_mouse_tracking = false;
        bool _is_mouse_hover = false;

        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };

    LRESULT CALLBACK window::native::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        native* self = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*) lParam;
            self = (native*) pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) self);

            self->_hwnd = hwnd;
        }
        else { self = (native*) GetWindowLongPtr(hwnd, GWLP_USERDATA); }
        if (self) { return self->handle_message(uMsg, wParam, lParam); }
        else { return DefWindowProc(hwnd, uMsg, wParam, lParam); }
    }

    window::window(__fst::optional_ptr<delegate> d) noexcept
        : _delegate(__fst::move(d))
    {
        _native = __fst::make<native, __fst::default_memory_zone, __fst::os_memory_category>(this);
    }

    window::~window() noexcept
    {
        if (_native) { __fst::destroy<__fst::default_memory_zone, __fst::os_memory_category>(_native); }
    }

    void window::show() noexcept
    {
        _native->show();
    }

    void window::hide() noexcept
    {
        _native->hide();
    }

    bool window::capture_mouse() noexcept
    {
        return _native->capture_mouse();
    }

    void window::release_mouse() noexcept
    {
        _native->release_mouse();
    }

    void window::set_mouse_tracking(bool active) noexcept
    {
        _native->set_mouse_tracking(active);
    }

    bool window::has_mouse_tracking() const noexcept
    {
        return _native->has_mouse_tracking();
    }

    window::handle window::get_handle() const noexcept
    {
        return _native->_hwnd;
    }

    void window::run() noexcept
    {
        _native->run();
    }

    window::window(window && w) noexcept
        : _native(w._native)
        , _delegate(__fst::move(w._delegate))
    {
        w._native = nullptr;
    }

    window& window::operator=(window&& w) noexcept
    {
        if (_native) { __fst::destroy<__fst::default_memory_zone, __fst::os_memory_category>(_native); }
        _native = w._native;
        _delegate = __fst::move(w._delegate);
        return *this;
    }

    window::pointer window::create(__fst::optional_ptr<delegate> d, const __fst::rect<int>& rect) noexcept
    {
        window* win = (window*) pointer::memory_zone_type::aligned_allocate(sizeof(window), alignof(window), pointer::memory_category_type::id());
        fst_assert(win, "allocation failed");

        win = fst_placement_new(win) window(__fst::move(d));
        bool status = win->_native->create(rect);
        fst_assert(status);
        return pointer(win);
    }

FST_END_SUB_NAMESPACE

#endif // __FST__WINDOWS__
