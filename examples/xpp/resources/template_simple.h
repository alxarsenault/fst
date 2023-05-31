@import header

#pragma once
#include <iostream>

@import utils/clamp
@import utils/minimum
@import utils/maximum
@import utils/is_one_of

//
// Traits
//

@import traits/add_const
@import traits/is_copy_assignable
@import traits/is_trivially_copy_constructible
@import traits/add_lvalue_reference
@import traits/is_copy_constructible
@import traits/is_trivially_copyable
@import traits/add_rvalue_reference
@import traits/is_default_constructible
@import traits/is_trivially_default_constructible
@import traits/always_false
@import traits/is_empty
@import traits/is_trivially_destructible
@import traits/conditional
@import traits/is_enum
@import traits/is_trivially_move_constructible
@import traits/empty_t
@import traits/is_final
@import traits/is_void
@import traits/integral_constant
@import traits/is_lvalue_reference
@import traits/is_volatile
@import traits/is_abstract
@import traits/is_move_assignable
@import traits/nullptr_t
@import traits/is_array
@import traits/is_move_constructible
@import traits/remove_const
@import traits/is_assignable
@import traits/is_pointer
@import traits/remove_cv
@import traits/is_base_of
@import traits/is_reference
@import traits/remove_cvref
@import traits/is_class
@import traits/is_rvalue_reference
@import traits/remove_pointer
@import traits/is_const
@import traits/is_same
@import traits/remove_reference
@import traits/is_constructible
@import traits/is_standard_layout
@import traits/void_t
@import traits/is_convertible
@import traits/is_trivial

@macro john(33, 56)
@macro peter()
@doc bg
@doc banana


@trait my_john = is_trivial(fst::string, ["fst/string.h"])

@import test_comment
int test(int a)
{
    @trace Tabarnak

    @code
    s = [[if(a < 8) {
        return a;
    }
    ]]
    add_code(s);

    add_code("if(a < 8) return a;\n");
    @endcode
    return 8;
}

@code(banana, jesus, BINGO, a, b)
--print("BINFFOO")
state.banana = 32;
--print(context.path)
--print(context.content)
@endcode

@code
add_code("inline void gg();");
@endcode

@script check
 


