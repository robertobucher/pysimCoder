
from supsisim.RCPblk import RCPblk
from numpy import size

def maxFromNInputs(pin, pout):
    """

    Call:   maxFromNInputs(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('maxFromNInputs', pin, pout, [0,0], 0, [], [])
    return blk
