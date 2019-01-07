from sympy import symbols, Matrix,pi
from sympy.physics.mechanics import *
import numpy as np

# Modeling the system with Kane method

# Signals
ph1, ph2 = dynamicsymbols('ph1 ph2')
w1, w2 = dynamicsymbols('w1 w2')
I1,I2 = dynamicsymbols('I1 I2')

# Constants
c, d, d1, d2 = symbols('c d d1 d2')
M1, M2 = symbols('M1 M2')
t = symbols('t')
kt1, kt2 = symbols('kt1 kt2')

# Reference frame fro Disk 1 and 2
N1 = ReferenceFrame('N1')
N2 = N1.orientnew('N2', 'Axis',[pi, N1.z])

O1 = Point('O')
O1.set_vel(N1,0)

O2 = O1.locatenew('O2',0)
O2.set_vel(N1,0)

# Reference frames for rotating disks
Nr1 = N1.orientnew('Nr1', 'Axis',[ph1, N1.x])
Nr2 = N2.orientnew('Nr2', 'Axis',[ph2, N2.x])

Nr1.set_ang_vel(N1, w1*N1.x)
Nr2.set_ang_vel(N2, w2*N2.x)

# Mechanics
Io1 = outer(Nr1.x, Nr1.x)
Io2 = outer(Nr2.x, Nr2.x)

In1T = (Io1, O1)
In2T = (Io2, O2)

B1 = RigidBody('B1', O1, Nr1, M1, In1T)
B2 = RigidBody('B2', O2, Nr2, M2, In2T)

# Forces and torques
forces = [(Nr1,(kt1*I1-c*ph1-d1*w1-d*w1)*N1.x+(d*w2+c*ph2)*N2.x)]
forces.append((Nr2,(kt2*I2-c*ph2-d2*w2-d*w2)*N2.x+(d*w1+c*ph1)*N1.x))

kindiffs = [(ph1.diff(t)-w1)]
kindiffs.append((ph2.diff(t)-w2))

bodies=[B1,B2]

# Model
KM = KanesMethod(N1,q_ind=[ph1,ph2],u_ind=[w1,w2],kd_eqs=kindiffs)
fr, frstar = KM.kanes_equations(forces,bodies)

print(fr)
print(frstar)

# symbolically linearize about arbitrary equilibrium
M,linear_state_matrix, linear_input_matrix, inputs = KM.linearize(new_method=True)

# sub in the equilibrium point and the parameters
eq_pt = [0, 0,0,0]
eq_dict = dict(zip([ph1,w1,ph2,w2], eq_pt))

f_A_lin = linear_state_matrix.subs(eq_dict)
f_B_lin = linear_input_matrix.subs(eq_dict)
m_mat = KM.mass_matrix_full.subs(eq_dict)

# compute A and B matrices
A = M.inv() * f_A_lin
B = M.inv() * f_B_lin

print(A)
print(B)

# Real plants parameters
# Motore 1
jm1 = 0.0000085  # inerzia [kg*m2]
kt1p = 0.0000382  # constante di coppia [Nm/mA] per la centralina al posto di [Nm]

# Volano motore 1
rho_ac = 7900  # densita alluminio [g/m3]
rv1 = 0.065    # raggio [m] 
hv1 = 0.01     # spessore [m]
mv1 = ((rho_ac*(rv1**2))*np.pi)*hv1   # massa [kg]
jv1 = (mv1*(rv1**2))/2   # inerzia [kg*m2]
J1 = jv1+jm1   # inerzia totale [kg*m2]

# Motore 2
jm2 = 0.000003   # inerzia [kg*m2]
kt2p = 0.0000205  # constante di coppia [Nm/mA] per la centralina al posto di [Nm/A]

# Volano motore 2
rho_ac = 7900   # densita acciao [kg/m3]
rv2 = 0.065     # raggio [m] 
hv2 = 0.01      # spessore [m]
mv2 = ((rho_ac*(rv2**2))*np.pi)*hv2   # massa [kg]
jv2 = (mv2*(rv2**2))/2              # inerzia [kg*m2]
J2 = jv2+jm2                   # inerzia totale [km*m2]

# Molla                     (frequenza d''oscillazione di 2 Hz)
dp = 0.0027836   # attrito molla (smorzamento)
cp = 0.4797954 
d1p = 0.0002953 
d2p = 0.0004001 

# Get numerical matrices
pars = [kt1, kt2, c, d, d1, d2]
par_vals = [kt1p, kt2p, cp ,dp, d1p, d2p]
par_dict = dict(zip(pars, par_vals))

f_A_lin = linear_state_matrix.subs(par_dict).subs(eq_dict)
f_B_lin = linear_input_matrix.subs(par_dict).subs(eq_dict)

from scipy import zeros
import scipy as sp
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from control import *
from control.matlab import *
from supsictrl.ctrl_repl import *
from supsictrl.ctrl_utils import *

A = np.matrix(M.inv() * f_A_lin)
B1 = np.matrix(M.inv() * f_B_lin)

# Plant model
A[2] = A[2]/J1
A[3] = A[3]/J2
B1[2] = B1[2]/J1
B1[3] = B1[3]/J2

print(A)
print(B1)

A = A.astype('float64')
B1 = B1.astype('float64')

B = B1[:,0]
C = [[1,0,0,0],[0,1, 0, 0]] 
C2 = [0,1,0,0]
D = [[0],[0]] 
D2 = [0]

# Sampling time
ts = 10e-3

gss1 = ss(A,B,C,D)
gss = ss(A,B,C2,D2)
gz = c2d(gss,ts,'zoh')

# Control design
wn = 10
xi1 = np.sqrt(2)/2 
xi2 = np.sqrt(3)/2 
xi2 = 0.85

cl_p1 = [1,2*xi1*wn,wn**2]
cl_p2 = [1,2*xi2*wn,wn**2]
cl_p3 = [1,wn]
cl_poly1 = sp.polymul(cl_p1,cl_p2)
cl_poly = sp.polymul(cl_poly1,cl_p3)
cl_poles = sp.roots(cl_poly)     # Desired continous poles
cl_polesd = sp.exp(cl_poles*ts)  # Desired discrete poles

# Add discrete integrator for steady state zero error
Phi_f = np.vstack((gz.A,-gz.C*ts))
Phi_f = np.hstack((Phi_f,[[0],[0],[0],[0],[1]]))
G_f = np.vstack((gz.B,zeros((1,1))))

# Pole placement
k = place(Phi_f,G_f,cl_polesd)

# Observer design - reduced order observer
poli_of = 5*sp.roots(cl_poly1)     # Desired continous poles
poli_o = 5*cl_poles[0:2]
poli_oz = sp.exp(poli_o*ts) 
poli_ozf = sp.exp(poli_of*ts)

disks = ss(A,B,C,D)
disksz = StateSpace(gz.A,gz.B,C,D,ts)
T = [[0,0,1,0],[0,0,0,1]]

# Reduced order observer
#poli_oz = mat(poli_oz).T
r_obs = red_obs(disksz ,T, poli_oz)
f_obs = full_obs(disksz, poli_ozf)

# Controller and observer in the same matrix -  Compact form
contr_I = comp_form_i(disksz,r_obs,k,[0,1])

# Implement anti windup
[gss_in,gss_out] = set_aw(contr_I,[0.1, 0.1, 0.1])

