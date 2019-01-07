from time import time

from numpy import imag, zeros, eye, mat, array, shape, real, exp, sqrt, size, log, pi, arange, linspace, r_, c_
from scipy import poly
from scipy.linalg import inv, eigvals, eig, logm, norm, expm

class TicToc():
    
    def __init__(self):
        self.tic_ = time()

    def tic(self):
        self.tic_ = time()

    def toc(self):
        print(time()-self.tic_)

