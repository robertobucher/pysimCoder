from supsisim.RCPblk import RCPblk
from numpy import size

def prodBlk(pin, pout):
    """

    Call:   prodBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('prod',pin,pout,[0,0],1,[],[])
    return blk

