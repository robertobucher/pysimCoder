
from supsisim.RCPblk import RCPblk
from scipy import size

def divBlk(pin, pout):
    """

    Call:   divBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('Div', pin, pout, [0,0], 1, [], [])
    return blk
