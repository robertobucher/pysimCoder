from supsisim.RCPblk import RCPblk
from numpy import size

def FH_5XXX_ADBlk(pout, candev, ID, ch):
    """

    Call:   FH_5XXX_ADBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       ch: Channel (0 or 1)

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('FH_5XXX_AD',[],pout,[0,0],0,[],[ID, ch], candev)
    return blk

