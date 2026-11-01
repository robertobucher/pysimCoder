import os
import numpy as np
import matplotlib.pyplot as plt
from supsisim.const import TMPDIR

def plotDlg(nin, nout, pars, name):
    fn = os.path.join(TMPDIR, name)

    try:
        x = np.loadtxt(fn)
        t = x[:,0]
        y = x[:,1:]
        plt.plot(t,y)
        plt.grid()
        plt.show()
    except:
        pass

