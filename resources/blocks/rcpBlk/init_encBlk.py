from supsisim.RCPblk import RCPblk
from scipy import size

def init_encBlk(pin, pout, trgtime, defv, offset):
    """

    Call:   init_encBlk(pin, pout, trgtime, defv, offset)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       trgtime : Trigger Time
       defv : Default Output
       offset : Offset

    Returns
    -------
        blk  : RCPblk

    """
    
    if (size(pout) != 1) or (size(pin) != 1):
        raise ValueError("Block should have 1 input and 1 output port; received %i and %i." % (size(pin),size(pout)))

    blk = RCPblk('init_enc',pin,pout,[0,0],1,[trgtime, defv, offset, 0.0],[])
    return blk

