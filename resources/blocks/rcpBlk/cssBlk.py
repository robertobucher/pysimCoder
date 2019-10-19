from supsisim.RCPblk import RCPblk
from control import tf2ss, TransferFunction
from scipy import mat, shape, size, zeros
from numpy import reshape, hstack

def cssBlk(pin,pout,sys,X0=[]):
    """ 

    Continous state space block

    Call: cssBlk(pin,pout, sys,X0)

    Parameters
    ----------
        pin : connected input ports
        pout: connected output ports
        sys: Discrete system in SS form
        X0: Initial conditions

    Returns
    -------
        blk  : RCPblk

    """
    if isinstance(sys, TransferFunction):
        sys=tf2ss(sys)

    nin = size(pin)
    ni = shape(sys.B)[1]
    if (nin != ni):
        raise ValueError("Block have %i inputs: received %i input ports" % (nin,ni))
    
    no = shape(sys.C)[0]
    nout = size(pout)
    if(no != nout):
        raise ValueError("Block have %i outputs: received %i output ports" % (nout,no))
        
    a  = reshape(sys.A,(1,size(sys.A)),'C')
    b  = reshape(sys.B,(1,size(sys.B)),'C')
    c  = reshape(sys.C,(1,size(sys.C)),'C')
    d  = reshape(sys.D,(1,size(sys.D)),'C')
    nx = shape(sys.A)[0]

    if(size(X0) == nx):
        X0 = reshape(X0,(1,size(X0)),'C')
    else:
        X0 = mat(zeros((1,nx)))

    indA = 1
    indB = indA + nx*nx
    indC = indB + nx*ni
    indD = indC + nx*no
    indX = indD + ni*no
    intPar = [nx,ni,no, indA, indB, indC, indD, indX]
    realPar = hstack((mat([0.0]),a,b,c,d,X0))

    if d.any() == True:
        uy = 1
    else:
        uy = 0
    
    blk = RCPblk('css',pin,pout,[nx,0],uy,realPar,intPar)
    return blk

