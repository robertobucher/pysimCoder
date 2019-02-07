
from supsisim.RCPblk import RCPblk
from scipy import size

def nullBlk(pin):
    """

    Call:   nullBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('toNull', pin, [], [0,0], 1, [], [])
    return blk
