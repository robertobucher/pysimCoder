"""
This is a procedural interface to the ctrl_utils library

roberto.bucher@supsi.ch

The following commands are provided:

Design commands

  full_obs     - full order observer
  red_obs      - reduced order observer
  comp_form    - state feedback controller+observer in compact form
  comp_form_i  - state feedback controller+observer+integ in compact form
  set_aw       - introduce anti-windup into controller
  matext       - Create extended matrices for state feedback with integral part
  init_par     - get xi and wn fron os and Ts
  xi2os        - get os from xi
  os2xi        - get xi from os
  ts2wn        - get wn from xi and ts
  wn2ts        - get ts from xi and wn
"""

import numpy as np
import scipy as sp
import scipy.linalg as la
import scipy.integrate as ig
import control as ct
import matplotlib.pyplot as plt

def set_mydefaults():
    ct.set_defaults('freqplot', dB=True, deg=True, Hz=False, grid=True)
    
def full_obs(sys,poles):
    """
    Full order observer of the system sys

    Call:
    obs = full_obs(sys,poles)

    Parameters
    ----------
    sys : System in State Space form
    poles: desired observer poles

    Returns
    -------
    obs: ss
    Observer

    """
    if isinstance(sys, ct.TransferFunction):
        "System must be in state space form"
        return
    a = np.asmatrix(sys.A)
    b = np.asmatrix(sys.B)
    c = np.asmatrix(sys.C)
    d = np.asmatrix(sys.D)
    L = ct.place(a.T,c.T,poles)
    L = np.asmatrix(L).T
    Ao = a-L*c
    Bo = np.hstack((b-L*d,L))
    n = np.shape(Ao)
    m = np.shape(Bo)
    Co = np.eye(n[0],n[1])
    Do = np.zeros((n[0],m[1]))
    obs = ct.StateSpace(Ao,Bo,Co,Do,sys.dt)
    return obs

def red_obs(sys,T,poles):
    """
    Reduced order observer of the system sys

    Call:
    obs = red_obs(sys,T,poles)

    Parameters
    ----------
    sys : System in State Space form
    T: Complement matrix
    poles: desired observer poles

    Returns
    -------
    obs: ss
    Reduced order Observer

    """
    if isinstance(sys, ct.TransferFunction):
        "System must be in state space form"
        return
    a = np.asmatrix(sys.A)
    b = np.asmatrix(sys.B)
    c = np.asmatrix(sys.C)
    d = np.asmatrix(sys.D)
    T = np.asmatrix(T)
    P = np.asmatrix(np.vstack((c,T)))
    invP = la.inv(P)
    AA = P*a*invP
    ny = np.shape(c)[0]
    nx = np.shape(a)[0]
    nu = np.shape(b)[1]

    A11 = AA[0:ny,0:ny]
    A12 = AA[0:ny,ny:nx]
    A21 = AA[ny:nx,0:ny]
    A22 = AA[ny:nx,ny:nx]

    L1 = ct.place(A22.T,A12.T,poles)
    L1 = np.asmatrix(L1).T

    nn = nx-ny

    tmp1 = np.asmatrix(np.hstack((-L1,np.eye(nn,nn))))
    tmp2 = np.asmatrix(np.vstack((np.zeros((ny,nn)),np.eye(nn,nn))))
    Ar = tmp1*P*a*invP*tmp2
 
    tmp3 = np.vstack((np.eye(ny,ny),L1))
    tmp3 = np.asmatrix(np.hstack((P*b,P*a*invP*tmp3)))
    tmp4 = np.hstack((np.eye(nu,nu),np.zeros((nu,ny))))
    tmp5 = np.hstack((-d,np.eye(ny,ny)))
    tmp4 = np.asmatrix(np.vstack((tmp4,tmp5)))

    Br = tmp1*tmp3*tmp4

    Cr = invP*tmp2

    tmp5 = np.hstack((np.zeros((ny,nu)),np.eye(ny,ny)))
    tmp6 = np.hstack((np.zeros((nn,nu)),L1))
    tmp5 = np.asmatrix(np.vstack((tmp5,tmp6)))
    Dr = invP*tmp5*tmp4
    
    obs = ct.StateSpace(Ar,Br,Cr,Dr,sys.dt)
    return obs

def comp_form(sys,obs,K):
    """
    Compact form Conroller+Observer

    Call:
    contr = comp_form(sys,obs,K)

    Parameters
    ----------
    sys : System in State Space form
    obs : Observer in State Space form
    K: State feedback gains

    Returns
    -------
    contr: ss
    Controller

    """
    nx = np.shape(sys.A)[0]
    ny = np.shape(sys.C)[0]
    nu = np.shape(sys.B)[1]
    no = np.shape(obs.A)[0]

    Bu = np.asmatrix(obs.B[:,0:nu])
    By = np.asmatrix(obs.B[:,nu:])
    Du = np.asmatrix(obs.D[:,0:nu])
    Dy = np.asmatrix(obs.D[:,nu:])

    X = la.inv(np.eye(nu,nu)+K*Du)

    Ac = np.asmatrix(obs.A)-Bu*X*K*np.asmatrix(obs.C);
    Bc = np.hstack((Bu*X,By-Bu*X*K*Dy))
    Cc  =  -X*K*np.asmatrix(obs.C);
    Dc  =  np.hstack((X,-X*K*Dy))
    contr  =  ct.StateSpace(Ac,Bc,Cc,Dc,sys.dt)
    return contr

def comp_form_i(sys,obs,K,Cy = [[1]]):
    """
    Compact form Conroller+Observer+Integral part
 
    Call:
    contr = comp_form_i(sys,obs,K [,Cy])

    Parameters
    ----------
    sys : System in State Space form
    obs : Observer in State Space form
    K: State feedback gains
    Cy: feedback matric to choose the output for integral part

    Returns
    -------
    contr: ss
    Controller

    """
    if sys.isctime():
        contFlag = True
    else:
        contFlag = False      
        Ts  =  sys.dt
    
    ny = np.shape(sys.C)[0]
    nu = np.shape(sys.B)[1]
    nx = np.shape(sys.A)[0]
    no = np.shape(obs.A)[0]
    ni = np.shape(np.asmatrix(Cy))[0]

    B_obsu = np.asmatrix(obs.B[:,0:nu])
    B_obsy = np.asmatrix(obs.B[:,nu:nu+ny])
    D_obsu = np.asmatrix(obs.D[:,0:nu])
    D_obsy = np.asmatrix(obs.D[:,nu:nu+ny])

    k = np.asmatrix(K)
    nk = np.shape(k)[1]
    Ke = k[:,nk-ni:]
    K = k[:,0:nk-ni]
    X  =  la.inv(np.eye(nu,nu)+K*D_obsu);

    a = np.asmatrix(obs.A)
    c = np.asmatrix(obs.C)
    Cy = np.asmatrix(Cy)

    tmp1 = np.hstack((a-B_obsu*X*K*c,-B_obsu*X*Ke))

    if contFlag:
        tmp2 = np.hstack((np.zeros((ni,no)),np.zeros((ni,ni))))
    else:
        tmp2 = np.hstack((np.zeros((ni,no)),np.eye(ni,ni)))   
    A_ctr = np.vstack((tmp1,tmp2))

    tmp1 = np.hstack((np.zeros((no,ni)),-B_obsu*X*K*D_obsy+B_obsy))
    if contFlag:
        tmp2 = np.hstack((np.eye(ni,ni),-Cy))
    else:
        tmp2 = np.hstack((np.eye(ni,ni)*Ts,-Cy*Ts))
    B_ctr = np.vstack((tmp1,tmp2))

    C_ctr = np.hstack((-X*K*c,-X*Ke))
    D_ctr = np.hstack((np.zeros((nu,ni)),-X*K*D_obsy))

    if contFlag:
        contr = ct.StateSpace(A_ctr,B_ctr,C_ctr,D_ctr)
    else:
        contr = ct.StateSpace(A_ctr,B_ctr,C_ctr,D_ctr,sys.dt)
        
    return contr
    
def set_aw(sys,poles):
    """
    Divide in controller in input and feedback part
    for anti-windup

    Usage
    =====
    [sys_in,sys_fbk] = set_aw(sys,poles)

    Inputs
    ------

    sys: controller
    poles : poles for the anti-windup filter

    Outputs
    -------
    sys_in, sys_fbk: controller in input and feedback part
    """
    sys = ct.ss(sys)
    Ts = sys.dt
    den_old = np.poly(la.eigvals(sys.A))
    sys = ct.tf(sys)
    den = np.poly(poles)
    tmp =  ct.tf(den_old,den,sys.dt)
    sys_in = tmp*sys
    sys_in = sys_in.minreal()
    sys_in = ct.ss(sys_in)
    sys_fbk = 1-tmp
    sys_fbk  =  sys_fbk.minreal()
    sys_fbk  =  ct.ss(sys_fbk)
    return sys_in, sys_fbk

def matext(syst):
    """
    Create extended matrices for state feedback with integral part

    Call:
    Aext, Bext = matext(syst)

    Parameters
    ----------
    syst : System in State Space form (continous or discrete)

    Returns
    -------
    Aext, Bext : matrices of the new system with integral part

    """
    n = syst.A.shape[0]
    if syst.isctime():
         Aext=np.vstack((syst.A, -syst.C))
         Aext =np.hstack((Aext, np.zeros((n+1,1)) ))
    else:
        ts = syst.dt
        Aext=np.vstack((syst.A,-syst.C*ts))
        Aext=np.hstack( (Aext, np.zeros((n+1,1))))
        Aext[n, n] = 1     
    Bext=np.vstack((syst.B, -syst.D))
    return Aext, Bext
        
def init_par(os, ts):
    """
    Find xi and wn for given os and ts

    xi, wn = init_par(os,ts)
    """
    xi = -np.log(os/100)/np.sqrt(np.pi**2 + (np.log(os/100))**2)
    wn = -np.log(0.02*np.sqrt(1-xi**2))/(xi*ts)
    return xi, wn

def xi2os(xi):
    """
    Find os from given xi

    os  =  xi2os(xi)
    """
    os = 100*np.exp(-xi*np.pi/np.sqrt(1-xi**2))
    return os

def os2xi(os):
    """
    Find xi from given os

    xi = xi2os(os)
    """
    xi = -np.log(os/100)/np.sqrt(np.pi**2 + (np.log(os/100))**2)
    return xi

def ts2wn(ts, xi):
    """
    Find wn from given ts and xi

    wn = ts2wn(ts, xi)
    """
    wn = -np.log(0.02*np.sqrt(1-xi**2))/(xi*ts)
    return wn

def wn2ts(wn, xi):
    """
    Find ts from given wn and xi

    ts = wn2ts(wn, xi)
    """
    ts = -np.log(0.02*np.sqrt(1-xi**2))/(xi*wn)
    return ts

class StatePrt:
    """
    StatePrt(fun, [xmin, xmax], [ymin, ymax], Points=20)
    The function "fun" is defined as
    def fun(t, x):
        .....
    """
    def __init__(self, fun, xlim, ylim, Points = 20):
        self.fun = fun
        self.ts = (0,500)
        
        x1 = np.linspace(xlim[0], xlim[-1], Points)
        x2 = np.linspace(ylim[0], ylim[-1], Points)
        k1 = x1[1] - x1[0]
        k2  =x2[1] - x2[0]
        k = 3/np.sqrt(k1**2+k2**2)
    
        x1m = np.zeros((Points, Points))
        x2m = np.zeros((Points, Points))
        h = 0.01
        for nx1 in range(0, Points):
            for nx2 in range(0, Points):
                t, x  = ig.odeint(fun, [x1[nx1], x2[nx2]], [0, h], tfirst=True)

                dx1 = x[0] - x1[nx1]
                dx2 = x[1] - x2[nx2]

                l = np.sqrt(dx1**2+dx2**2)*k
                if l>1.e-10:
                    x1m[nx2,nx1] = dx1/l
                    x2m[nx2,nx1] = dx2/l
        fig, self.ax = plt.subplots()
        self.hl, = self.ax.plot([0], [0])
        #self.ax.set_autoscalex_on(False)
        #self.ax.set_autoscaley_on(False)
        q = self.ax.quiver(x1, x2, x1m, x2m)
        cid = self.hl.figure.canvas.mpl_connect('button_press_event', self)
        plt.show()
           
    def __call__(self, event):
        if event.inaxes!=self.ax.axes: return
        x0 = [event.xdata, event.ydata]
        try:
            x = ig.solve_ivp(self.fun, (0,500), x0)
            self.hl.set_data(x.y[0], x.y[1])
            self.hl.figure.canvas.draw()
        except:
            pass
