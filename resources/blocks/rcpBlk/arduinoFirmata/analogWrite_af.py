
from supsisim.RCPblk import RCPblk
from numpy import size

def analogWrite_af(pin, devicePin,pinMode):
    """

    Call:   analogWrite_af(pin, devicePin)

    Parameters
    ----------
       pin: connected input port(s)
       devicepin : Device pin
       pinmode : Pin mode

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('analogWrite_af', pin, [], [0,0], 0, [], [devicePin, pinMode])
    return blk
