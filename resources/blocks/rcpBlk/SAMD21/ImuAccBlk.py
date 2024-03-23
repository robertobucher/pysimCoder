
from supsisim.RCPblk import RCPblk
from numpy import size

def ImuAccBlk(pout):
    """

    Call:   ImuAcc(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('ImuAcc', [], pout, [0,0], 0, [], [])
    return blk
