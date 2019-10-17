#!/usr/bin/python

from supsisim.pysim import supsisimul
from control import *
import sys
import os

if __name__ == "__main__":
    if len(sys.argv) > 1:
        fname = sys.argv[1]
        if not os.path.isfile(fname):
            fname = 'untitled'
    else:
        fname = 'untitled'
    th = supsisimul(fname, runflag = True)

