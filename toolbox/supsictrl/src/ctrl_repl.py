"""
This is a procedural interface to the yttalab library

roberto.bucher@supsi.ch

The following commands are provided:

Design and plot commands
  place       - Pole placement (replacement for place)
  dlqr        - Discrete linear quadratic regulator
  d2c         - discrete to continous time conversion  
"""
from numpy import hstack, vstack, imag, zeros, eye, mat, sort, \
     array, shape, real, sort, around, inf, sqrt, size, nonzero
from scipy.linalg import inv, eigvals, eig, logm, norm
import scipy as sp
from scipy.signal import place_poles
import numpy as np
from slycot import sb02od
#from control import *
from control import TransferFunction, tf2ss, StateSpace, ss2tf, dare, issys

def d2c(sys,method='zoh'):
    """Continous to discrete conversion with ZOH method

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
    if isinstance(sys, TransferFunction):
        sys=tf2ss(sys)
        flag=1

    a=sys.A
    b=sys.B
    c=sys.C
    d=sys.D
    Ts=sys.dt
    n=shape(a)[0]
    nb=shape(b)[1]
    nc=shape(c)[0]
    tol=1e-12
    
    if method=='zoh':
        if n==1:
            if b[0,0]==1:
                A=0
                B=b/sys.dt
                C=c
                D=d
        else:
            tmp1=hstack((a,b))
            tmp2=hstack((zeros((nb,n)),eye(nb)))
            tmp=vstack((tmp1,tmp2))
            s=logm(tmp)
            s=s/Ts
            if norm(imag(s),inf) > sqrt(sp.finfo(float).eps):
                print('Warning: accuracy may be poor')
            s=real(s)
            A=s[0:n,0:n]
            B=s[0:n,n:n+nb]
            C=c
            D=d
    elif method=='foh':
        a=mat(a)
        b=mat(b)
        c=mat(c)
        d=mat(d)
        Id = mat(eye(n))
        A = logm(a)/Ts
        A = real(around(A,12))
        Amat = mat(A)
        B = (a-Id)**(-2)*Amat**2*b*Ts
        B = real(around(B,12))
        Bmat = mat(B)
        C = c
        D = d - C*(Amat**(-2)/Ts*(a-Id)-Amat**(-1))*Bmat
        D = real(around(D,12))
    elif method=='tustin':
        a=mat(a)
        b=mat(b)
        c=mat(c)
        d=mat(d)
        poles=eigvals(a)
        if any(abs(poles-1)<200*sp.finfo(float).eps):
            print('d2c: some poles very close to one. May get bad results.')
        
        I=mat(eye(n,n))
        tk = 2 / sqrt (Ts)
        A = (2/Ts)*(a-I)*inv(a+I)
        iab = inv(I+a)*b
        B = tk*iab
        C = tk*(c*inv(I+a))
        D = d- (c*iab)
    else:
        print('Method not supported')
        return
    
    sysc=StateSpace(A,B,C,D)
    if flag==1:
        sysc=ss2tf(sysc)
    return sysc

def dlqr(*args, **keywords):
    """Linear quadratic regulator design for discrete systems

    Usage
    =====
    [K, S, E] = dlqr(A, B, Q, R, [N])
    [K, S, E] = dlqr(sys, Q, R, [N])

    The dlqr() function computes the optimal state feedback controller
    that minimizes the quadratic cost

        J = \sum_0^\infty x' Q x + u' R u + 2 x' N u

    Inputs
    ------
    A, B: 2-d arrays with dynamics and input matrices
    sys: linear I/O system 
    Q, R: 2-d array with state and input weight matrices
    N: optional 2-d array with cross weight matrix

    Outputs
    -------
    K: 2-d array with state feedback gains
    S: 2-d array with solution to Riccati equation
    E: 1-d array with eigenvalues of the closed loop system
    """

    # 
    # Process the arguments and figure out what inputs we received
    #
    
    # Get the system description
    if (len(args) < 3):
        raise ControlArgument("not enough input arguments")

    elif (issys(args[0])):
        # We were passed a system as the first argument; extract A and B
        A = array(args[0].A, ndmin=2, dtype=float);
        B = array(args[0].B, ndmin=2, dtype=float);
        index = 1;
        if args[0].dt==None:
            print('dlqr works only for discrete systems!')
            return
    else:
        # Arguments should be A and B matrices
        A = array(args[0], ndmin=2, dtype=float);
        B = array(args[1], ndmin=2, dtype=float);
        index = 2;

    # Get the weighting matrices (converting to matrices, if needed)
    Q = array(args[index], ndmin=2, dtype=float);
    R = array(args[index+1], ndmin=2, dtype=float);
    if (len(args) > index + 2): 
        N = array(args[index+2], ndmin=2, dtype=float);
        Nflag = 1;
    else:
        N = zeros((Q.shape[0], R.shape[1]));
        Nflag = 0;

    # Check dimensions for consistency
    nstates = B.shape[0];
    ninputs = B.shape[1];
    if (A.shape[0] != nstates or A.shape[1] != nstates):
        raise ControlDimension("inconsistent system dimensions")

    elif (Q.shape[0] != nstates or Q.shape[1] != nstates or
          R.shape[0] != ninputs or R.shape[1] != ninputs or
          N.shape[0] != nstates or N.shape[1] != ninputs):
        raise ControlDimension("incorrect weighting matrix dimensions")

    if Nflag==1:
        Ao=A-B*inv(R)*N.T
        Qo=Q-N*inv(R)*N.T
    else:
        Ao=A
        Qo=Q
    
    #Solve the riccati equation
    (X,L,G) = dare(Ao,B,Qo,R)
#    X = bb_dare(Ao,B,Qo,R)

    # Now compute the return value
    Phi=mat(A)
    H=mat(B)
    K=inv(H.T*X*H+R)*(H.T*X*Phi+N.T)
    L=eig(Phi-H*K)
    return K,X,L

def dlqe(A, G, C, Q, R):
    """
    Kalmann filter

    [K,X] = dlqe(A, G, C, Q, R)
    """
    
   # Get the weighting matrices (converting to matrices, if needed)
    Q = np.array(Q, ndmin=2, dtype=float);
    R = np.array(R, ndmin=2, dtype=float);

    A = np.mat(A)
    C = np.mat(C)
    G = np.mat(G)
    
    #Solve the riccati equation
    (X,L,G) = dare(A.T, C.T, G*Q*G.T, R)

    # Now compute the return value
    K = X*C.T /(C*X*C.T+R)
    Z = X - K*C*X
    Z = (Z+Z.T)/2
    return K, X

