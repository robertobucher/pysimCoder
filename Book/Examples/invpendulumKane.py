from sympy import symbols, Matrix, pi
from sympy.physics.mechanics import *
import numpy as np

# Modeling the system with Kane method

# Signals
x, th  = dynamicsymbols('x th')
v, w = dynamicsymbols('v w')
F = dynamicsymbols('F')
d = symbols('d')

# Constants
m, r = symbols('m r')
M = symbols('M')
g, t = symbols('g t')
Jc = symbols('Jc')

# Frames and Coord. system

# Car - reference x,y
Nf = ReferenceFrame('Nf')
C = Point('C')                         
C.set_vel(Nf, v*Nf.x)
Car = Particle('Car',C,M)

# Pendulum - reference x1, y1
Npend = Nf.orientnew('Npend','Axis',[th,Nf.z])
Npend.set_ang_vel(Nf,w*Nf.z)

P = C.locatenew('P',r*Npend.x)
P.v2pt_theory(C,Nf,Npend)
Pa = Particle('Pa', P, m)

I = outer (Nf.z, Nf.z)
Inertia_tuple = (Jc*I, P)
Bp = RigidBody('Bp', P, Npend, m, Inertia_tuple)

# Forces and torques
forces = [(C,F*Nf.x-d*v*Nf.x),(P,-m*g*Nf.y)]
frames = [Nf,Npend]
points = [C,P]

kindiffs = [x.diff(t)-v, th.diff(t) - w]
particles = [Car,Bp]

# Model
KM = KanesMethod(Nf,q_ind=[x,th],u_ind=[v,w], kd_eqs=kindiffs)
fr,frstar = KM.kanes_equations(forces,particles)

# Equilibrium point
eq_pt = [0, pi/2,0,0]
eq_dict = dict(zip([x,th,v,w], eq_pt))

# symbolically linearize about arbitrary equilibrium
linear_state_matrix, linear_input_matrix, inputs = KM.linearize()

# sub in the equilibrium point and the parameters
f_A_lin = linear_state_matrix.subs(eq_dict)
f_B_lin = linear_input_matrix.subs(eq_dict)
m_mat = KM.mass_matrix_full.subs(eq_dict)

# compute A and B
Atmp = m_mat.inv() * f_A_lin
Btmp = m_mat.inv() * f_B_lin

# Linear model
print Atmp
print Btmp


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
pars = [M , m, r, d, Jc, g]
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

from supsictrl.yottalab import *
import numpy as np
import scipy as sp
from matplotlib.pyplot import *
from control import *
from control.matlab import *
from control.mateqn import *

# Continous system
sys = ss(A,B,C,D)

# discrete-time model
ts = 0.01
sysd = c2d(sys,ts,'zoh')

# LQR Controller design
##############################################################################
Q = np.diag([300,10,400,20]);
Q = np.diag([400,300,20,10]);
R = [4e-6];                    
[k_lqr, S, E] = dlqr(sysd.A,sysd.B,Q,R)

# Observer design parameters
##############################################################################
# Control design - Reduced order observer
(preg,pvect) = sp.linalg.eig(A-B*k_lqr)
rho=max(abs(preg));       # process spectral radius
rhoamp=10;                # amplification of observer poles

angle1 = 15*1j*np.pi/16
poles_c = rhoamp*rho*sp.exp([angle1, -angle1])
obs_poles = sp.exp(poles_c*ts)

T=[[0,0,1,0],[0,0,0,1]]
obs=red_obs(sysd,T,[obs_poles])

# Put Observer and controller together (compact form)
ctr = comp_form(sysd,obs,k_lqr)





