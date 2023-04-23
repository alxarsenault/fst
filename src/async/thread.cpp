#include "fst/async/thread.h"
#include "fst/utility.h"
FST_BEGIN_SUB_NAMESPACE(async)

    //
    struct thread::native
    {

        native()
        {

            //print("NATIVE");
        }
        ~native()
        {
            //fst::print("~native--dsjdksjdskjdksjdksjdskdsjd");
        }
    };

    //
    thread::thread() noexcept
        : _native(native_pointer::make())
        , _thread_data{ nullptr, nullptr, nullptr }
    {

        //native* ptr = (native*) _FST::default_memory_zone::aligned_allocate(sizeof(native), alignof(native), _FST::async_memory_category::id());
        //fst_assert(ptr, "allocation failed");

        //ptr = fst_placement_new(ptr) native();

        //fst::print("thread create");

        //_native.reset(ptr);
    }

    thread::thread(thread::thread_data tdata) noexcept
        : _native(native_pointer::make())
        , _thread_data(tdata)
    {

        _thread_data.process(this, _thread_data.data);
    }

    thread::~thread() noexcept
    {

        if (_thread_data.release) { _thread_data.release(this, _thread_data.data); }
    }

    thread& thread::operator=(thread&&) noexcept
    {
        return *this;
    }

FST_END_SUB_NAMESPACE
