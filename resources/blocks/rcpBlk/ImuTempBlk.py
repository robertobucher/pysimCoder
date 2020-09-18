
from supsisim.RCPblk import RCPblk
from scipy import size

def ImuTempBlk(pout):
    """

    Call:   ImuTemp(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('ImuTemp', [], pout, [0,0], 0, [], [])
    return blk
