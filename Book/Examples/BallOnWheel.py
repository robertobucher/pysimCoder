from sympy import symbols, Matrix, pi
from sympy.physics.mechanics import *
import numpy as np

ph0, ph1, ph2 = dynamicsymbols('ph0 ph1 ph2')
w1, w2 = dynamicsymbols('w1 w2')

T = dynamicsymbols('T')

J1, J2 = symbols('J1 J2')
M1, M2 = symbols('M1 M2')
R1, R2 = symbols('R1 R2')
d1     = symbols('d1')
g      = symbols('g')
t      = symbols('t')

N = ReferenceFrame('N')

O = Point('O')
O.set_vel(N,0)

ph0 = (R1*ph1+R2*ph2)/(R1+R2)

N0 = N.orientnew('N0','Axis',[ph0,N.z])
N1 = N.orientnew('N1','Axis',[ph1,N.z])
N2 = N.orientnew('N2','Axis',[ph2,N.z])
N1.set_ang_vel(N,w1*N.z)
N2.set_ang_vel(N,w2*N.z)

CM2 = O.locatenew('CM2',(R1+R2)*N0.y)
CM2.v2pt_theory(O,N,N0)

Iz = outer(N.z,N.z)
In1T = (J1*Iz, O)
In2T = (J2*Iz, CM2)

B1 = RigidBody('B1', O, N1, M1, In1T)
B2 = RigidBody('B2', CM2, N2, M2, In2T)

#forces = [(N1, (T-d1*w1)*N.z), (CM2,-M2*g*N.y)]
forces = [(N1, T*N.z), (CM2,-M2*g*N.y)]

kindiffs = [ph1.diff(t)-w1,ph2.diff(t)-w2]

KM = KanesMethod(N,q_ind=[ph1, ph2],u_ind=[w1, w2],kd_eqs=kindiffs)
fr, frstar = KM.kanes_equations([B1, B2], forces)

from sympy import Dummy, lambdify
from numpy.linalg import solve
from scipy.integrate import odeint
from matplotlib.pyplot import *

# Equilibrium point
eq_pt = [0, 0, 0, 0, 0]
eq_dict = dict(zip([ph1,ph2,w1,w2, T], eq_pt))

# symbolically linearize about arbitrary equilibrium
MM, linear_state_matrix, linear_input_matrix, inputs = KM.linearize(new_method=True)

# sub in the equilibrium point and the parameters
f_A_lin = linear_state_matrix.subs(eq_dict)
f_B_lin = linear_input_matrix.subs(eq_dict)
m_mat = KM.mass_matrix_full.subs(eq_dict)

# compute A and B
Atmp = m_mat.inv() * f_A_lin
Btmp = m_mat.inv() * f_B_lin

R_w_par=0.254;
R_b_par=0.04;
M_b_par=0.1;
J_w_par=2;
J_b_par=3;
g_par=9.81;
d_w_par=R_w_par*2;

pars = [R1, R2, M2, J1, J2, g, d1]
par_vals = [R_w_par, R_b_par, M_b_par, J_w_par, J_b_par, g_par, d_w_par]
par_dict = dict(zip(pars, par_vals))

# Matrices with identified values
Atmp = Atmp.subs(par_dict)
Btmp = Btmp.subs(par_dict)

A = np.matrix(Atmp)
B = np.matrix(Btmp)

A = A.astype('float64')
B = B.astype('float64')
