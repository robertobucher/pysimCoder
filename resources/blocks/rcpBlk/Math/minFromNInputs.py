
from supsisim.RCPblk import RCPblk
from numpy import size

def minFromNInputs(pin, pout):
    """

    Call:   minFromNInputs(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('minFromNInputs', pin, pout, [0,0], 0, [], [])
    return blk
