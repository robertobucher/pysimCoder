"""
This is a procedural interface to the yttalab library

roberto.bucher@supsi.ch

The following commands are provided:

Design and plot commands
  d2c         - discrete to continous time conversion  
"""
import numpy as np
import scipy as sp
import scipy.linalg as la
from slycot import sb02od
import control as ct

def d2c(sys,method='zoh'):
    """
    Continous to discrete conversion with ZOH method

    Call:
    sysc=d2c(sys,method='log')

    Parameters
    ----------
    sys :   System in statespace or Tf form 
    method: 'zoh' or 'tustin'

    Returns
    -------
    sysc: continous system ss or tf
    

    """
    flag = 0
    if isinstance(sys, ct.TransferFunction):
        sys = ct.tf2ss(sys)
        flag = 1

    a = sys.A
    b = sys.B
    c = sys.C
    d = sys.D
    Ts = sys.dt
    n = np.shape(a)[0]
    nb = np.shape(b)[1]
    nc = np.shape(c)[0]
    tol=1e-12
    
    if method=='zoh':
        if n==1 and a[0,0]==1:
            A = 0
            B = b/sys.dt
            C = c
            D = d
        else:
            tmp1 = np.hstack((a,b))
            tmp2 = np.hstack((np.zeros((nb, n)), np.eye(nb)))
            tmp = np.vstack((tmp1, tmp2))
            s = la.logm(tmp)
            s = s/Ts
            if la.norm(np. imag(s), np.inf) > np.sqrt(sp.finfo(float).eps):
                print('Warning: accuracy may be poor')
            s = np.real(s)
            A = s[0:n,0:n]
            B = s[0:n,n:n+nb]
            C = c
            D = d
    elif method=='foh':
        a = np.asmatrix(a)
        b = np.asmatrix(b)
        c = np.asmatrix(c)
        d = np.asmatrix(d)
        Id = np.asmatrix(np.eye(n))
        A = la.logm(a)/Ts
        A = np.real(np.around(A,12))
        Amat = np.asmatrix(A)
        B = (a-Id)**(-2)*Amat**2*b*Ts
        B = np.real(np.around(B,12))
        Bmat = np.asmatrix(B)
        C = c
        D = d - C*(Amat**(-2)/Ts*(a-Id)-Amat**(-1))*Bmat
        D = np.real(np.around(D,12))
    elif method=='tustin':
        a = np.asmatrix(a)
        b = np.asmatrix(b)
        c = np.asmatrix(c)
        d = np.asmatrix(d)
        poles = la.eigvals(a)
        if any(abs(poles-1)<200*sp.finfo(float).eps):
            print('d2c: some poles very close to one. May get bad results.')
        
        I = np.asmatrix(np.eye(n,n))
        tk = 2 / np.sqrt (Ts)
        A = (2/Ts)*(a-I)*la.inv(a+I)
        iab = la.inv(I+a)*b
        B = tk*iab
        C = tk*(c*la.inv(I+a))
        D = d- (c*iab)
    else:
        print('Method not supported')
        return
    
    sysc=ct.StateSpace(A,B,C,D)
    if flag==1:
        sysc = ct.ss2tf(sysc)
    return sysc

