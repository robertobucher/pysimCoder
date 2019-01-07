#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import sys

def dataplt(fn):
    try:
        x = np.loadtxt(fn)
        t = x[:,0]
        y = x[:,1:]
        plt.plot(t,y)
        plt.grid()
        plt.show()
    except:
        pass


if __name__ == "__main__":
    if len(sys.argv) > 1:
        fname = sys.argv[1]
        dataplt(fname)
