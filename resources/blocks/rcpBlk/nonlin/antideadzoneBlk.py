
from supsisim.RCPblk import RCPblk
from numpy import size

def antideadzoneBlk(pin, pout, upper, lower):
    """

    Call:   antideadzone(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       upper : Upper
       lower : Lower

    Returns
    -------
       blk: RCPblk

    """

    if upper < 0:
        raise ValueError("Anti Deadzone: the Upper field must be >= 0!")
    if lower > 0:
        raise ValueError("Anti Deadzone: the Lower field must be <= 0!")

    blk = RCPblk('antideadzone', pin, pout, [0,0], 0, [upper, lower], [])
    return blk
