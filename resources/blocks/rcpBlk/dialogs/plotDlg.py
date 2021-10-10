import numpy as np
import matplotlib.pyplot as plt

def plotDlg(nin, nout, pars, name):
    fn = '/tmp/'+name

    try:
        x = np.loadtxt(fn)
        t = x[:,0]
        y = x[:,1:]
        plt.plot(t,y)
        plt.grid()
        plt.show()
    except:
        pass

