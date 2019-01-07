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

# Motore 2
jm2 = 0.000003     # inerzia [kg*m2]
d2 = 0.0032        # attrito
kt2 = 0.0000205    # constante di coppia [Nm/mA] per la centralina al posto di [Nm/A]

# Volano motore 2
rho_ac = 7900      # densita acciao [kg/m3]
rv2 = 0.065        # raggio [m] 
hv2 = 0.01         # spessore [m]
mv2 = ((rho_ac*(rv2**2))*np.pi)*hv2   # massa [kg]
jv2 = (mv2*(rv2**2))/2              # inerzia [kg*m2]
J2 = jv2+jm2                       # inerzia totale [km*m2]

p=[d2]
D1=d2
Uo=1000 

# Identify motor 2

x2 = np.loadtxt('/home/bucher/CACSD/pycontrol/Tests/ControlDesign/Disks/MOT2');
t2=x2[:,0] 
y2=x2[:,2] 
t2=t2[100:] 
t2=t2-t2[0] 
y2=y2[100:] 
y2=y2-y2[0] 
U02=600  
p02=[1.,4.]
y22 = y2/U02
t2=linspace(0,4.99,500)
plsq = leastsq(residuals, p02, args=(y22, t2))

Jm2=kt2/plsq[0][0]           # Motor Inertia
Dm2=plsq[0][1]*Jm2           # Motor friction

g2=tf([kt2/Jm2],[1,Dm2/Jm2,0])  # Transfer function

Y2,T2 = step(g2,t2)
plot(T2,Y2)
plot(t2,y22)
show()

# Design Controller Motor 2
a=[[0,1],[0,-Dm2/Jm2]]
b=[[0],[1]]
c=[[kt2/Jm2,0]];
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

