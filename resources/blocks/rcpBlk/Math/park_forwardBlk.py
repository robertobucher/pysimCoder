from supsisim.RCPblk import RCPblk
from numpy import size

def park_forwardBlk(pin, pout):
    """

    Call:   park_forwardBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('forward_park', pin, pout, [0,0], 1, [], [])
    return blk

