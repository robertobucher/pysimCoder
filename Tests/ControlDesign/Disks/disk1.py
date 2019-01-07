from scipy.optimize import leastsq
from scipy.signal import step2
import numpy as np
import scipy as sp
from control import *
from control.matlab import *
from supsictrl.ctrl_repl import *
from supsictrl.ctrl_utils import *
from matplotlib.pyplot import plot, show

# Motor response for least square identification
def residuals(p, y, t):  
    [k,alpha] = p
    g = tf(k,[1,alpha,0])
    Y,T = step(g,t)
    err=y-Y
    return err

# Motore 1
jm1 = 0.0000085    # inerzia [kg*m2]
d1 = 0.0032        # attrito
kt1 = 0.0000382    # constante di coppia [Nm/mA] per la centralina al posto di [Nm/A]

# Volano motore 1
rho_ac = 7900      # densita alluminio [kg/m3]
rv1 = 0.065        # raggio [m] 
hv1 = 0.01         # spessore [m]
mv1 = ((rho_ac*(rv1**2))*np.pi)*hv1     # massa [kg]
jv1 = (mv1*(rv1**2))/2     # inerzia [kg*m2]
J1 = jv1+jm1              # inerzia totale [kg*m2]

p=[d1]
D1=d1
Uo=1000 

# Identify motor 1

#x1 = np.loadtxt('/home/bucher/CACSD/pycontrol/Tests/ControlDesign/Disks/MOT1');
x1 = np.loadtxt('MOT1');
t1=x1[:,0] 
y1=x1[:,2] 
t1=t1[100:] 
t1=t1-t1[0] 
y1=y1[100:] 
y1=y1-y1[0] 
U01=1000  
p01=[1.0,4.0]
y11 = y1/U01
t1=linspace(0,4.99,500)
plsq = leastsq(residuals, p01, args=(y11, t1))

Jm1=kt1/plsq[0][0]           # Motor Inertia
Dm1=plsq[0][1]*Jm1           # Motor friction

g1=tf([kt1/Jm1],[1,Dm1/Jm1,0])  # Transfer function

Y1,T1 = step(g1,t1)
plot(T1,Y1)
plot(t1,y11)
show()

# Design Controller Motor 1
a=[[0,1],[0,-Dm1/Jm1]]
b=[[0],[1]]
c=[[kt1/Jm1,0]];
d=[0];

sysc=ss(a,b,c,d)                # Continous state space form

Ts=0.01                         # Sampling time
sys = c2d(sysc,Ts,'zoh')       # Get discrete state space form

# Control system design
# State feedback with integral part

wn=6
xi=np.sqrt(2)/2

cl_p1=[1,2*xi*wn,wn**2]
cl_p2=[1,wn]
cl_poly=sp.polymul(cl_p1,cl_p2)
cl_poles=sp.roots(cl_poly);  # Desired continous poles
cl_polesd=sp.exp(cl_poles*Ts)  # Desired discrete poles

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

contr_I=comp_form_i(sys,r_obs,k)

# Anti windup
[gss_in,gss_out]=set_aw(contr_I,[0,0])

