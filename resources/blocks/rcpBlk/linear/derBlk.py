from supsisim.RCPblk import RCPblk
from numpy import size

def derBlk(pin,pout):
    """ 

    Continous integral block

    Call: derBlk(pin,pout,X0)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

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
        
    blk = RCPblk('der',pin,pout,[1,0],1,[0.0, 0.0],[])
    return blk

