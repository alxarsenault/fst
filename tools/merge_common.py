#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os;
import platform;

if __name__ == "__main__":
    dir_path = os.path.dirname(os.path.realpath(__file__));
    project_directory = os.path.join(dir_path, "..");    
    include_directory = os.path.join(project_directory, "include", "fst");
    common_file_path = os.path.join(include_directory, "common.h");

    common_file = open(common_file_path, "r+");
    common_content = common_file.read();

    macro_file = open(os.path.join(include_directory, "detail", "__macro.h"), "r+").read();
    platform_file = open(os.path.join(include_directory, "detail", "__platform.h"), "r+").read();
    arch_file = open(os.path.join(include_directory, "detail", "__arch.h"), "r+").read();
    int_file = open(os.path.join(include_directory, "detail", "__int.h"), "r+").read();
    compiler_file = open(os.path.join(include_directory, "detail", "__compiler.h"), "r+").read();
    platform_macro_file = open(os.path.join(include_directory, "detail", "__platform_macro.h"), "r+").read();
    numeric_limits_file = open(os.path.join(include_directory, "detail", "__numeric_limits.h"), "r+").read();
    flags_file = open(os.path.join(include_directory, "detail", "__flags.h"), "r+").read();
    assert_file = open(os.path.join(include_directory, "detail", "__assert.h"), "r+").read();
    container_file = open(os.path.join(include_directory, "detail", "__container.h"), "r+").read();
    builtin_traits_file = open(os.path.join(include_directory, "detail", "__builtin_traits.h"), "r+").read();
    traits_file = open(os.path.join(include_directory, "detail", "__traits.h"), "r+").read();
    initializer_list_file = open(os.path.join(include_directory, "detail", "__initializer_list.h"), "r+").read();
    
    common_content = common_content.replace('#include "fst/detail/__macro.h"', macro_file);
    common_content = common_content.replace('#include "fst/detail/__platform.h"', platform_file);
    common_content = common_content.replace('#include "fst/detail/__arch.h"', arch_file);
    common_content = common_content.replace('#include "fst/detail/__int.h"', int_file);
    common_content = common_content.replace('#include "fst/detail/__compiler.h"', compiler_file);
    common_content = common_content.replace('#include "fst/detail/__platform_macro.h"', platform_macro_file);
    common_content = common_content.replace('#include "fst/detail/__numeric_limits.h"', numeric_limits_file);
    common_content = common_content.replace('#include "fst/detail/__flags.h"', flags_file);
    common_content = common_content.replace('#include "fst/detail/__assert.h"', assert_file);
    common_content = common_content.replace('#include "fst/detail/__container.h"', container_file);
    common_content = common_content.replace('#include "fst/detail/__builtin_traits.h"', builtin_traits_file);
    common_content = common_content.replace('#include "fst/detail/__traits.h"', traits_file);
    common_content = common_content.replace('#include "fst/detail/__initializer_list.h"', initializer_list_file);

    print(common_content)
