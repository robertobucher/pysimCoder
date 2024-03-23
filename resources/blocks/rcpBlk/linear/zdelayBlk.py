from supsisim.RCPblk import RCPblk
from numpy import size

def zdelayBlk(pin, pout, X0=0.0):
    """

    Call:   zdelayBlk(pin, pout, X0=0.0)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       X0: Initial conditions

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
        
    blk = RCPblk('unitDelay',pin,pout,[0,1],0,[X0],[])
    return blk

