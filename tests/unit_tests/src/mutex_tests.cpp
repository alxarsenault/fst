#include "utest.h"
#include "fst/mutex.h"
#include "fst/atomic.h"

// TODO: switch to fst::thread when implemented.
#include <thread>

namespace
{
    TEST_CASE("fst::mutex", "[core]")
    {
        TEST_BLOCK("mutex in main thread")
        {
            fst::atomic<bool> did_try = false;
            fst::mutex m;
            m.lock();

            std::thread t(
                [&]()
                {
                    REQUIRE_FALSE(m.try_lock());
                    did_try = true;
                });

            while (!did_try.load()) {}

            t.join();

            m.unlock();

            REQUIRE(did_try);
        }

        TEST_BLOCK("mutex in other thread")
        {
            //fst::atomic<bool> did_try = false;
            //fst::atomic<bool> did_lock = false;
            //fst::mutex m;

            //std::thread t(
            //    [&]()
            //    {
            //        m.lock();
            //        did_lock.store(true);
            //        while (!did_try.load()) {}
            //        m.unlock();
            //    });

            //// Wait for thread to lock the mutex.
            //while (!did_lock.load()) {}

            //// Try to lock it.
            //REQUIRE_FALSE(m.try_lock());

            //// Notify the thread.
            //did_try.store(true);

            //// This should work in almost no time.
            //m.lock();

            //t.join();

            //m.unlock();
        }

        TEST_BLOCK("mutex in other thread with try_lock loop")
        {
            //fst::atomic<bool> did_try = false;
            //fst::atomic<bool> did_lock = false;
            //fst::mutex m;

            //std::thread t(
            //    [&]()
            //    {
            //        m.lock();
            //        did_lock.store(true);
            //        while (!did_try.load()) {}
            //        m.unlock();
            //    });

            //// Wait for thread to lock the mutex.
            //while (!did_lock.load()) {}

            //// Try to lock it.
            //REQUIRE_FALSE(m.try_lock());

            //// Notify the thread.
            //did_try.store(true);

            //// This should work in almost no time.
            //while(!m.try_lock()){}

            //t.join();

            //m.unlock();
        }
    }
} // namespace
