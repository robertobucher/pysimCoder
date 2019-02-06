from supsisim.RCPblk import RCPblk
from scipy import size

def intgBlk(pin,pout,X0=0.0):
    """ 

    Continous integral block

    Call: intgBlk(pin,pout,X0)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       X0 : Initial conditions

    Returns
    -------
        blk  : RCPblk

    """
    
    nin = size(pin)
    if (nin != 1):
        raise ValueError("Block have 1 input: received %i input ports" % nin)

    nout = size(pout)
    if(nout != 1):
        raise ValueError("Block have 1 output1: received %i output ports" % nout)
        
    blk = RCPblk('integral',pin,pout,[1,0],0,[0.0 ,X0],[])
    return blk

