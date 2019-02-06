from supsisim.RCPblk import RCPblk
from scipy import size

def deadzoneBlk(pin, pout, start, end):
    """

    Call:   deadzoneBlk(pin, pout, start, end)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       start : Start
       end : End

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('deadzone', pin, pout, [0,0], 1, [start, end], [])
    return blk

