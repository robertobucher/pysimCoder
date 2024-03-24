from supsisim.RCPblk import RCPblk
from numpy import size


def customRead_af(pout, value):
    """

    Call:   customRead_af(pout, value)

    Parameters
    ----------
       pout: connected output port(s)
       value : Value

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('customRead_af', [], pout, [0, 0], 0, [], value)
    return blk
