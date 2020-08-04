from sympy import symbols, Matrix, pi, cos, sin
from sympy.physics.mechanics import *
import numpy as np

# Modeling the system with Kane method

# Signals
x, th  = dynamicsymbols('x th')
v, w = dynamicsymbols('x th', 1)
F = dynamicsymbols('F')
d = symbols('d')

# Constants
m, r = symbols('m r')
M = symbols('M')
g, t = symbols('g t')
Ic = symbols('Ic')

# Frames and Coord. system

# Car
Nf = ReferenceFrame('Nf')
C = Point('C')                         
C.set_vel(Nf, v*Nf.x)
Car = Particle('Car',C,M)

# Pendulum
A = Nf.orientnew('A','Axis',[th,Nf.z])
A.set_ang_vel(Nf,w*Nf.z)

P = C.locatenew('P',r*A.x)
P.v2pt_theory(C,Nf,A)
Pa = Particle('Pa', P, m)

I = outer (Nf.z, Nf.z)
Inertia_tuple = (Ic*I, P)
Bp = RigidBody('Bp', P, A, m, Inertia_tuple)

Bp.potential_energy = m*g*r*sin(th)
Car.potential_energy = 0

# Forces and torques
forces = [(C,F*Nf.x-d*v*Nf.x),(P,0*Nf.y)]

# Lagrange operator
L = Lagrangian(Nf, Car, Bp)

# Lagrange model
LM = LagrangesMethod(L, [x, th], forcelist = forces, frame = Nf)
LM.form_lagranges_equations()

# Equilibrium point
eq_pt = [0.0, pi/2,0.0,0.0]
eq_dict = dict(zip([x,th,v,w], eq_pt))

# symbolically linearize about arbitrary equilibrium
MM, linear_state_matrix, linear_input_matrix, inputs = LM.linearize(q_ind = [x, th], qd_ind = [v, w])

f_p_lin = linear_state_matrix.subs(eq_dict)
f_B_lin = linear_input_matrix.subs(eq_dict)

MM = MM.subs(eq_dict)

Atmp = MM.inv() * f_p_lin
Btmp = MM.inv() * f_B_lin

# Real parameters
Mpar = 0.4874257
mpar = 0.19211
rpar = 0.323
dpar = 1.6800357
Th =  0.0294049
gpar = 9.81
Icpar = Th - mpar*rpar**2
kt = 0.0603/1000
rp = 0.0286524
N = 1

# Get numerical matrices
pars = [M , m, r, d, Ic, g]
par_vals = [Mpar, mpar, rpar, dpar, Icpar, gpar]

par_dict = dict(zip(pars, par_vals))

# Matrices with identified values
Atmp = Atmp.subs(par_dict)
Btmp = Btmp.subs(par_dict)

A = np.matrix(Atmp)
B = np.matrix(Btmp)

A = A.astype('float64')
B = kt/rp*B.astype('float64')

C=[[1, 0, 0, 0], [0, 1, 0, 0]]    
D=[[0], [0]]

from supsictrl.ctrl_repl import *
from supsictrl.ctrl_utils import *
import numpy as np
from matplotlib.pyplot import *
from control import *
from control.matlab import *
from control.mateqn import *

# Continous system
# states: x, phi, v, omega
invPend = ss(A,B,C,D)

# discrete-time model
ts = 0.01
sysd = c2d(invPend,ts,'zoh')

# LQR Controller design
##############################################################################
Q = np.diag([400,300,20,10]);
R = [4e-6];                    
[k_lqr, S, E] = dlqr(sysd.A,sysd.B,Q,R)

# Observer design parameters
##############################################################################
# Control design - Reduced order observer
(preg,pvect) = np.linalg.eig(A-B*k_lqr)
rho=max(abs(preg));       # process spectral radius
rhoamp=10;                # amplification of observer poles

angle1 = 15*1j*np.pi/16
poles_c = rhoamp*rho*np.exp([angle1, -angle1])
obs_poles = np.exp(poles_c*ts)

T=[[0,0,1,0],[0,0,0,1]]
obs=red_obs(sysd,T,obs_poles)

# Put Observer and controller together (compact form)
ctr = comp_form(sysd,obs,k_lqr)

###### Swing up
# Motor model
# Motor response for least square identification
from scipy.optimize import leastsq

def residuals(p, y, t):  
    [k,alpha] = p
    g = tf(k,[1,alpha,0])
    Y,T = step(g,t)
    err=y-Y
    return err

# Motore 1
kt = 60.3e-6
rp = 0.75/26.17585
N = 1

x = np.loadtxt('MOT');
t = x[200:264,0] 
y = x[200:264,1] 
t = t - t[0] 
y = y - y[0]
t = np.arange(0,ts*(len(t)),ts)

U0=1500  
p0=[1.,4.]
y1 = y/U0 
plsq = leastsq(residuals, p0, args=(y1, t))

M = kt/plsq[0][0]           # Motor Inertia
d = plsq[0][1]*M                 # Motor friction

g = tf([kt/M],[1,d/M,0])  # Transfer function

Y,T = step(g,t)
plot(T,Y)
plot(t,y1)
#show()

# Plant
AP = [[0,1],[0,-d/M]]
BP = [[0],[kt/M]]
CP = [1,0]
DP = [0];

Mot = ss(AP,BP,CP,DP)
Motd = c2d(Mot, ts, 'zoh')

# Control Design
wn=6
xi=np.sqrt(2)/2

cl_p1=[1,2*xi*wn,wn**2]
cl_p2=[1,wn]
cl_poly=np.polymul(cl_p1,cl_p2)
cl_poles=np.roots(cl_poly);  # Desired continous poles
cl_polesd=np.exp(cl_poles*ts)  # Desired discrete poles

sz1=np.shape(Motd.A);
sz2=np.shape(Motd.B);

# Add discrete integrator for steady state zero error
Phi_f=np.vstack((Motd.A,-Motd.C*ts))
Phi_f=np.hstack((Phi_f,[[0],[0],[1]]))
G_f=np.vstack((Motd.B, np.zeros((1,1))))

kmot=place(Phi_f,G_f,cl_polesd)

#Reduced order observer
p_oc=-10*max(abs(cl_poles))
p_od=np.exp(p_oc*ts);

T=[0,1]
r_obs=red_obs(Motd,T,[p_od])

contr_I=comp_form_i(Motd,r_obs,kmot)

# Anti windup
[gss_in,gss_out]=set_aw(contr_I,[0,0])

def build_step_up(T1, T2, R1, R2):
    Tbase = np.arange(0,10,ts)
    s = np.shape(Tbase)
    vals = np.zeros(s)

    N1 = int(T1/ts)
    N2 = int(T2/ts)
    vals[0:N1]=R1
    vals[N1:N2]=R2

build_step_up(0.5,4,0.4,-0.4)

Amp = 0.59
F = 0.75

