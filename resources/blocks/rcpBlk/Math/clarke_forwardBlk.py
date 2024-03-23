from supsisim.RCPblk import RCPblk
from numpy import size

def clarke_forwardBlk(pin, pout):
    """

    Call:   clarke_forwardBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        blk  : RCPblk

    """

    if (size(pin) != 3 and size(pin) != 2):
        raise ValueError("Forward Clarke transformation has to have 2 or 3 inputs, current number of inputs is (%i)" % (size(pin)))

    blk = RCPblk('forward_clarke', pin, pout, [0,0], 1, [], [])
    return blk

