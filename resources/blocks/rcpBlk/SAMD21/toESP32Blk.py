
from supsisim.RCPblk import RCPblk
from numpy import size

def toESP32Blk(pin):
    """

    Call:   toESP32Blk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('toESP32', pin, [], [0,0], 1, [], [])
    return blk
