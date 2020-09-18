
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiRGBBlk(pin):
    """

    Call:   brikiRGBBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('brikiRGB', pin, [], [0,0], 1, [], [])
    return blk
