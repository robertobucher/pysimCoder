from supsisim.RCPblk import RCPblk
from numpy import size

def getTimerBlk(pout):
    """

    Call:   getTimerBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('getTimer', [], pout, [0,0], 0, [], [])
    return blk

