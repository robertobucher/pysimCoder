from supsisim.RCPblk import RCPblk
from scipy import size

def park_inverseBlk(pin, pout):
    """

    Call:   park_inverseBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('inverse_park', pin, pout, [0,0], 1, [], [])
    return blk

