
from supsisim.RCPblk import RCPblk
from numpy import pi

def sweepBlk(pout, k, typ):
    """

    Call:   sweepBlk(pout, k, typ)

    Parameters
    ----------
       pout: connected output port(s)
       k : Gain
       typ : Type linear(1), quadratic (2) 

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('sweep', [], pout, [0,0], 0, [2*pi*k], [typ])
    return blk
