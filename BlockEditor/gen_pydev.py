#!/usr/bin/env python3

"""Generate the C skeleton of a pysimCoder device block.

Usage:
    ./pysim-run.sh gen_pydev NAME
"""

import os
import sys

from supsisim.const import path

TEMPLATE = os.path.join(path, 'CodeGen', 'LinuxRT', 'devices', 'template.c.tmp')


def gen_pydev(name):
    """Write NAME.c into the current directory from the device template.

    The placeholder 'xxxx' is replaced by NAME (first occurrence on each
    line, as sed does). Returns the path of the generated file.
    """
    with open(TEMPLATE, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    fname = name + '.c'
    with open(fname, 'w', encoding='utf-8') as f:
        for line in lines:
            f.write(line.replace('xxxx', name, 1))
    return fname


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: gen_pydev NAME')
    gen_pydev(sys.argv[1])
