"""
This is a procedural interface to the yttalab library

roberto.bucher@supsi.ch

The following commands are provided:

Design and plot commands
  place       - Pole placement (replacement for place)
  dlqr        - Discrete linear quadratic regulator
  d2c         - discrete to continous time conversion  
"""
import numpy as np
import scipy as sp
import scipy.linalg as la
from slycot import sb02od
import control as ct

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
    if isinstance(sys, ct.TransferFunction):
        sys = ct.tf2ss(sys)
        flag = 1

    a = sys.A
    b = sys.B
    c = sys.C
    d = sys.D
    Ts = sys.dt
    n = np.shape(a)[0]
    nb = np.shape(b)[1]
    nc = np.shape(c)[0]
    tol=1e-12
    
    if method=='zoh':
        if n==1 and a[0,0]==1:
            A = 0
            B = b/sys.dt
            C = c
            D = d
        else:
            tmp1 = np.hstack((a,b))
            tmp2 = np.hstack((np.zeros((nb, n)), np.eye(nb)))
            tmp = np.vstack((tmp1, tmp2))
            s = la.logm(tmp)
            s = s/Ts
            if la.norm(np. imag(s), np.inf) > np.sqrt(sp.finfo(float).eps):
                print('Warning: accuracy may be poor')
            s = np.real(s)
            A = s[0:n,0:n]
            B = s[0:n,n:n+nb]
            C = c
            D = d
    elif method=='foh':
        a = np.mat(a)
        b = np.mat(b)
        c = np.mat(c)
        d = np.mat(d)
        Id = np.mat(np.eye(n))
        A = la.logm(a)/Ts
        A = np.real(np.around(A,12))
        Amat = np.mat(A)
        B = (a-Id)**(-2)*Amat**2*b*Ts
        B = np.real(np.around(B,12))
        Bmat = np.mat(B)
        C = c
        D = d - C*(Amat**(-2)/Ts*(a-Id)-Amat**(-1))*Bmat
        D = np.real(np.around(D,12))
    elif method=='tustin':
        a = np.mat(a)
        b = np.mat(b)
        c = np.mat(c)
        d = np.mat(d)
        poles = la.eigvals(a)
        if any(abs(poles-1)<200*sp.finfo(float).eps):
            print('d2c: some poles very close to one. May get bad results.')
        
        I = np.mat(np.eye(n,n))
        tk = 2 / np.sqrt (Ts)
        A = (2/Ts)*(a-I)*la.inv(a+I)
        iab = la.inv(I+a)*b
        B = tk*iab
        C = tk*(c*la.inv(I+a))
        D = d- (c*iab)
    else:
        print('Method not supported')
        return
    
    sysc=ct.StateSpace(A,B,C,D)
    if flag==1:
        sysc = ct.ss2tf(sysc)
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

    elif (ct.issys(args[0])):
        # We were passed a system as the first argument; extract A and B
        A = np.array(args[0].A, ndmin=2, dtype=float);
        B = np.array(args[0].B, ndmin=2, dtype=float);
        index = 1;
        if args[0].dt==None:
            print('dlqr works only for discrete systems!')
            return
    else:
        # Arguments should be A and B matrices
        A = np.array(args[0], ndmin=2, dtype=float);
        B = np.array(args[1], ndmin=2, dtype=float);
        index = 2;

    # Get the weighting matrices (converting to matrices, if needed)
    Q = np.array(args[index], ndmin=2, dtype=float);
    R = np.array(args[index+1], ndmin=2, dtype=float);
    if (len(args) > index + 2): 
        N = np.array(args[index+2], ndmin=2, dtype=float);
        Nflag = 1;
    else:
        N = np.zeros((Q.shape[0], R.shape[1]));
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
        Ao = A-B*la.inv(R)*N.T
        Qo = Q-N*la.inv(R)*N.T
    else:
        Ao = A
        Qo = Q
    
    #Solve the riccati equation
    (X,L,G) = ct.dare(Ao,B,Qo,R)
#    X = bb_dare(Ao,B,Qo,R)

    # Now compute the return value
    Phi = np.mat(A)
    H = np.mat(B)
    K = la.inv(H.T*X*H+R)*(H.T*X*Phi+N.T)
    L = la.eig(Phi-H*K)
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
    (X,L,G) = ct.dare(A.T, C.T, G*Q*G.T, R)

    # Now compute the return value
    K = X*C.T /(C*X*C.T+R)
    Z = X - K*C*X
    Z = (Z+Z.T)/2
    return K, X

def newdlqr(*args, **keywords):
    """lqr(A, B, Q, R[, N])

    Linear quadratic regulator design

    The lqr() function computes the optimal state feedback controller
    that minimizes the quadratic cost

    .. math:: J = \int_0^\infty (x' Q x + u' R u + 2 x' N u) dt

    The function can be called with either 3, 4, or 5 arguments:

    * ``lqr(sys, Q, R)``
    * ``lqr(sys, Q, R, N)``
    * ``lqr(A, B, Q, R)``
    * ``lqr(A, B, Q, R, N)``

    where `sys` is an `LTI` object, and `A`, `B`, `Q`, `R`, and `N` are
    2d arrays or matrices of appropriate dimension.

    Parameters
    ----------
    A, B: 2-d array
        Dynamics and input matrices
    sys: LTI (StateSpace or TransferFunction)
        Linear I/O system
    Q, R: 2-d array
        State and input weight matrices
    N: 2-d array, optional
        Cross weight matrix

    Returns
    -------
    K: 2-d array
        State feedback gains
    S: 2-d array
        Solution to Riccati equation
    E: 1-d array
        Eigenvalues of the closed loop system

    Examples
    --------
    >>> K, S, E = lqr(sys, Q, R, [N])
    >>> K, S, E = lqr(A, B, Q, R, [N])

    """

    # Make sure that SLICOT is installed
    try:
        from slycot import sb02md
        from slycot import sb02mt
    except ImportError:
        raise ControlSlycot("can't find slycot module 'sb02md' or 'sb02nt'")

    #
    # Process the arguments and figure out what inputs we received
    #

    # Get the system description
    if (len(args) < 3):
        raise ControlArgument("not enough input arguments")

    try:
        # If this works, we were (probably) passed a system as the
        # first argument; extract A and B
        A = np.array(args[0].A, ndmin=2, dtype=float);
        B = np.array(args[0].B, ndmin=2, dtype=float);
        index = 1;
    except AttributeError:
        # Arguments should be A and B matrices
        A = np.array(args[0], ndmin=2, dtype=float);
        B = np.array(args[1], ndmin=2, dtype=float);
        index = 2;

    # Get the weighting matrices (converting to matrices, if needed)
    Q = np.array(args[index], ndmin=2, dtype=float);
    R = np.array(args[index+1], ndmin=2, dtype=float);
    if (len(args) > index + 2):
        N = np.array(args[index+2], ndmin=2, dtype=float);
    else:
        N = np.zeros((Q.shape[0], R.shape[1]));

    # Check dimensions for consistency
    nstates = B.shape[0];
    ninputs = B.shape[1];
    if (A.shape[0] != nstates or A.shape[1] != nstates):
        raise ControlDimension("inconsistent system dimensions")

    elif (Q.shape[0] != nstates or Q.shape[1] != nstates or
          R.shape[0] != ninputs or R.shape[1] != ninputs or
          N.shape[0] != nstates or N.shape[1] != ninputs):
        raise ControlDimension("incorrect weighting matrix dimensions")

    # Compute the G matrix required by SB02MD
    A_b,B_b,Q_b,R_b,L_b,ipiv,oufact,G = \
        sb02mt(nstates, ninputs, B, R, A, Q, N, jobl='N');

    # Call the SLICOT function
    X,rcond,w,S,U,A_inv = sb02md(nstates, A_b, G, Q_b, 'D')

    # Now compute the return value
    # We assume that R is positive definite and, hence, invertible
    K = np.linalg.solve(R, np.dot(B.T, X) + N.T);
    S = X;
    E = w[0:nstates];

    return K, S, E

