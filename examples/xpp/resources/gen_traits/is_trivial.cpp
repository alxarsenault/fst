
#include "fst/stream.h"

using type = TYPE;

int main()
{
    fst::print<fst::stream_detail::space_t>("is_trivial", fst::is_trivial_v<type>);
    return 0;
}
