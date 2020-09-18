
from supsisim.RCPblk import RCPblk
from scipy import size

def ImuGyroBlk(pout):
    """

    Call:   ImuGyro(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('ImuGyro', [], pout, [0,0], 0, [], [])
    return blk
