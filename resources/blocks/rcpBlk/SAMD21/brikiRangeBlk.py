
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiRangeBlk(pout):
    """

    Call:   brikiRangeBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('brikiRange', [], pout, [0,0], 0, [], [])
    return blk
