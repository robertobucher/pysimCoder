from supsisim.RCPblk import RCPblk
from scipy import size

def stepBlk(pout, initTime, iVal, fVal):
    """

    Call:   stepBlk(pout, initTime, Val)

    Parameters
    ----------
       pout: connected output port(s)
       initTime : Step Time
       Val : Step Value

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('step',[],pout,[0,0],0,[initTime, iVal, fVal],[])
    return blk

