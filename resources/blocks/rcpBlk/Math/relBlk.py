from supsisim.RCPblk import RCPblk
from numpy import size

def relBlk(pin, pout, operator):
    """

    Call:   relBlk(pin, pout)

    Parameters
    ----------
       pin: connected input ports
       pout: connected output port
       operator: math operations to do

    Returns
    -------
        blk  : RCPblk

    """

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('rel',pin,pout,[0,0],1,[],[], operator)
    return blk

