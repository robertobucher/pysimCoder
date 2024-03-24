from supsisim.RCPblk import RCPblk
from numpy import size

def epos_areadBlk(pout, candev, ID, ch):
    """

    Call:   epos_areadBlk(pout, ID, ch)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       ch : Channel [0/1]

    Returns
    -------
       blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('epos_canopen_aread',[],pout,[0,0],0,[],[ID, ch], candev)
    return blk

