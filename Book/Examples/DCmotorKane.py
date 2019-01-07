from sympy import symbols, Matrix,pi
from sympy.physics.mechanics import *
import numpy as np

# Modeling the system with Kane method

# Signals
ph = dynamicsymbols('ph')
w  = dynamicsymbols('w')
I  = dynamicsymbols('I')

# Constants
Dm = symbols('Dm')
M, J = symbols('M J')
t = symbols('t')
kt  = symbols('kt')

# Reference frame for the motor and Last
N = ReferenceFrame('N')

O = Point('O')
O.set_vel(N,0)

# Reference frames for the rotating disk
Nr = N.orientnew('Nr', 'Axis',[ph, N.x])

Nr.set_ang_vel(N, w*N.x)

# Mechanics
Io = J*outer(Nr.x, Nr.x)

InT = (Io, O)

B = RigidBody('B', O, Nr, M, InT)

# Forces and torques
forces = [(Nr,(kt*I-Dm*w)*N.x)]

kindiffs = [(ph.diff(t)-w)]

bodies=[B]

# Model
KM = KanesMethod(N,q_ind=[ph],u_ind=[w],kd_eqs=kindiffs)
fr, frstar = KM.kanes_equations(forces,bodies)

print fr
print frstar

# symbolically linearize about arbitrary equilibrium
linear_state_matrix, linear_input_matrix, inputs = KM.linearize()

# sub in the equilibrium point and the parameters
eq_pt = [0, 0]
eq_dict = dict(zip([ph,w], eq_pt))

f_A_lin = linear_state_matrix.subs(eq_dict)
f_B_lin = linear_input_matrix.subs(eq_dict)
m_mat = KM.mass_matrix_full.subs(eq_dict)

# compute A and B matrices
A = np.matrix(m_mat.inv() * f_A_lin)
B = np.matrix(m_mat.inv() * f_B_lin)

print A
print B

