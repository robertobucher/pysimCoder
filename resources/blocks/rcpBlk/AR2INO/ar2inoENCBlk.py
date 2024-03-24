
from supsisim.RCPblk import RCPblk
from numpy import size

def ar2inoENCBlk(pout, channel, res, reset):
    """

    Call:   ar2inoENCBlk(pout, channel, res, reset)

    Parameters
    ----------
       pout: connected output port(s)
       channel : Encode no
       res : Resolution
       reset : Reset [0=no/1=yes]

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('ar2inoENC', [], pout, [0,0], 0, [], [channel, res, reset])
    return blk
