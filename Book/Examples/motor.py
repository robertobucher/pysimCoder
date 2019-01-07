from scipy.optimize import leastsq
from scipy.signal import step2
import numpy as np
import scipy as sp
from control import *
from control.matlab import *
from supsictrl.yottalab import *

# Motor response for least square identification
def residuals(p, y, t):  
    [k,alpha] = p
    g = tf(k,[1,alpha,0])
    Y,T = step(g,t)
    err=y-Y
    return err

# Identify motor

x = np.loadtxt('MOT');
t = x[:,0] 
y = x[:,2] 
t = t[100:] 
t = t-t[0] 
y = y[100:] 
y = y-y[0] 
Io = 1000  
p0 = [1,4]
y1 = y/Io

plsq = leastsq(residuals, p0, args=(y1, t))

kt = 0.0000382          # Motor torque constant
Jm=kt/plsq[0][0]        # Motor Inertia
Dm=plsq[0][1]*Jm        # Motor friction

g=tf([kt/Jm],[1,Dm/Jm,0])  # Transfer function

Y,T = step(g,t)
plot(T,Y,t,y1), legend(('Identified transfer function','Collected data'),prop={'size':10},loc=2), xlabel('t'), ylabel('y'), title('Step response'), grid()

# Design Controller Motor 1
a=[[0,1],[0,-Dm/Jm]]
b=[[0],[1]]
c=[[kt/Jm,0]];
d=[0];

sysc=ss(a,b,c,d)                # Continous state space form

Ts=0.01                         # Sampling time
sys = c2d(sysc,Ts,'zoh')        # Get discrete state space form

# Control system design
# State feedback with integral part

wn=6
xi=np.sqrt(2)/2
angle = np.arccos(xi)

cl_poles = -wn*array([1, exp(1j*angle), exp(-1j*angle)] )
cl_polesd=sp.exp(cl_poles*Ts)    # Desired discrete poles
sz1=sp.shape(sys.A);
sz2=sp.shape(sys.B);

# Add discrete integrator for steady state zero error
Phi_f=np.vstack((sys.A,-sys.C*Ts))
Phi_f=np.hstack((Phi_f,[[0],[0],[1]]))
G_f=np.vstack((sys.B,zeros((1,1))))

k=place(Phi_f,G_f,cl_polesd)

#Reduced order observer
p_oc=-10*max(abs(cl_poles))
p_od=sp.exp(p_oc*Ts);

T=[0,1]
r_obs=red_obs(sys,T,[p_od])

contr_I=comp_form_i(sys,r_obs,k,Ts)

# Anti windup
[gss_in, gss_out]=set_aw(contr_I,[0,0])

