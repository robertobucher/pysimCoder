from supsisim.RCPblk import RCPblk
from control import tf2ss, TransferFunction
from scipy import shape, size, mat
from numpy import reshape

def matmultBlk(pin, pout, Gains):
    """

    Matrix multiplication of the input signals
    
    Call: matmultBlk(pin,pout, Gains)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       Gains : Gains

    Returns
    -------
        blk  : RCPblk

    """
    
    Gains = mat(Gains)
    n,m = shape(Gains)
    if(size(pin) != m):
        raise ValueError("Block should have %i input port; received %i." % (m,size(pin)))
    if(size(pout) != n):
        raise ValueError("Block should have %i output port; received %i." % (n,size(pout)))
    realPar  = reshape(Gains,(1,size(Gains)),'C')
    blk = RCPblk('mxmult',pin,pout,[0,0],1,realPar,[n,m])
    return blk

