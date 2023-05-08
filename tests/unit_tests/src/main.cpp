
#include "utest.h"
#include "fst/profiler.h"
//#include "fst/simd.h"
#include "fst/string.h"
CATCH_INTERNAL_START_WARNINGS_SUPPRESSION
CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS
static Catch::LeakDetector leakDetector;
CATCH_INTERNAL_STOP_WARNINGS_SUPPRESSION

TEST_CASE("fst")
{
    REQUIRE(true);
}



int main(int argc, char* argv[])
{

    fst::print_version();

    FST_STRACE("main");

    //fst::simd::print_version();

    //fst::profiler::register_name<fst::default_memory_category>();
    //fst::profiler::register_name<fst::dsp_memory_category>();

    //fst::profiler::trace("Main");

    //fst::profiler::trace("dsakjjldsalkdjsalkjdlksaj", 3243);

    //fst::profiler::trace("Alex", 3243, "DJKIS");

    //fst::profiler::trace("Main") << fst::endl;

    (void) &leakDetector;
    int ret = Catch::Session().run(argc, argv);

    FST_ATTRIBUTE_UNUSED void* ptr = fst::allocate(32);
    fst::profiler::output(fst::cout);

    return ret;
}
