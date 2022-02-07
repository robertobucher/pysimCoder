
from supsisim.RCPblk import RCPblk
from scipy import size

def digitalWrite_af(pin, devicePin):
    """

    Call:   digitalWrite_af(pin, devicePin)

    Parameters
    ----------
       pin: connected input port(s)
       devicePin : Device pin

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('digitalWrite_af', pin, [], [0,0], 0, [], [devicePin])
    return blk
