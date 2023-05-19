#include "fst/stream.h"
#include "fst/os/window.h"

#define dbg fst::debug<fst::info_flags::function>()

class delegate : public fst::os::window::delegate
{
  public:
    virtual ~delegate() noexcept { dbg(); }

    virtual void on_window_create(fst::os::window* win) noexcept override
    {
        _win = win;
        dbg();
    }

    virtual void on_window_show() noexcept override { dbg(89); }

    virtual void on_window_hide() noexcept override { dbg(89); }

    virtual void on_window_close() noexcept override {}

    virtual void on_window_resize(fst::size<int> size) noexcept override { dbg(size); }

    virtual void on_window_move(fst::rect<int> rect) noexcept override { dbg(rect); }

    virtual void on_mouse_left_down(const fst::os::mouse_event& evt) noexcept override
    {
        dbg(evt.position, evt.click_count);
        _win->capture_mouse();
    }

    virtual void on_mouse_left_up(const fst::os::mouse_event& evt) noexcept override
    {
        dbg(evt.position);
        _win->release_mouse();
    }

    virtual void on_mouse_enter(const fst::os::mouse_event& evt) noexcept override { dbg(evt.position); }
    virtual void on_mouse_leave(const fst::os::mouse_event& evt) noexcept override { dbg(evt.position); }
    virtual void on_mouse_move(const fst::os::mouse_event&) noexcept override
    {
        //dbg(evt.position);
    }

    fst::os::window* _win = nullptr;
};

int main(int argc, char* argv[])
{
    //delegate d;
    //fst::os::window::pointer win = fst::os::window::create(fst::optional_ptr<delegate>(&d, false), fst::rect<int>(0, 0, 500, 500));
    fst::os::window::pointer win = fst::os::window::create(fst::optional_ptr<delegate>::make(), fst::rect<int>(0, 0, 500, 500));
    fst::optional_ptr<delegate> p;
    win->show();
    //fst::os::window::handle h = win->get_handle();
    win->set_mouse_tracking(true);

    win->run();
    return 0;
}
