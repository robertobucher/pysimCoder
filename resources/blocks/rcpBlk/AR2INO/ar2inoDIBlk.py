
from supsisim.RCPblk import RCPblk
from numpy import size

def ar2inoDIBlk(pout, ch):
    """

    Call:   ar2inoDIBlk(pout, pin)

    Parameters
    ----------
       pout: connected output port(s)
       pin : Channel

    Returns
    -------
       blk: RCPblk

    """

    if 2 <= ch <= 5:
        raise ValueError("Pin  %i is reserved for encoders!" % (ch))

    blk = RCPblk('ar2inoDI', [], pout, [0,0], 0, [], [ch])
    return blk
