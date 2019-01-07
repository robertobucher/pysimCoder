#!/usr/bin/python

from supsisim.pysim import supsisimul
from control import *
import sys

if __name__ == "__main__":
    if len(sys.argv) > 1:
        fname = sys.argv[1]
    else:
        fname = 'untitled'
    th = supsisimul(fname, runflag = True)
    th.start()

