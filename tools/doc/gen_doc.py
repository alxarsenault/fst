#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os;
import platform;

if __name__ == "__main__":
    dir_path = os.path.dirname(os.path.realpath(__file__));
    common_file_path = os.path.join(dir_path, "test.h");

    common_file = open(common_file_path, "r+");
    
    blocks = [];
    block = [];

    for line in common_file:
        line = line.lstrip();
        if line.startswith("///"):
            block.append(line.lstrip("///").lstrip());

        elif block:
            blocks.append(block);
            block = [];

    if block:
        blocks.append(block);

    # print(blocks);
    
    for x in blocks:
        res = ''.join(x);
        print(res)

#  

# pandoc -s --from=gfm --to=html5 -c /c/Code/Alex/fst/tools/doc/stylesheet.css -o /c/Code/Alex/fst/tools/doc/example3.html /c/Code/Alex/fst/tools/doc/test.md