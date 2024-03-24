from supsisim.RCPblk import RCPblk
from numpy import size

def constBlk(pout, val):
    """

    Call:   constBlk(pout, val)

    Parameters
    ----------
       pout: connected output port(s)
       val : Value

    Returns
    -------
    blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('constant',[],pout,[0,0],0,[val],[])
    return blk

