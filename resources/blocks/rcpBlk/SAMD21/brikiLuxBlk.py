
from supsisim.RCPblk import RCPblk
from numpy import size

def brikiLuxBlk(pout):
    """

    Call:   brikiLuxBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('brikiLux', [], pout, [0,0], 0, [], [])
    return blk
