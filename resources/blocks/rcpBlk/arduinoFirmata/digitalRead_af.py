
from supsisim.RCPblk import RCPblk
from scipy import size

def digitalRead_af(pout, devicePin):
    """

    Call:   digitalRead_af(pin, devicePin)

    Parameters
    ----------
       pout: connected output port(s)
       devicePin : Device pin

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('digitalRead_af', [], pout, [0,0], 0, [], [devicePin])
    return blk
