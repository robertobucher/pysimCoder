
from supsisim.RCPblk import RCPblk
from scipy import size

def upowBlk(pin, pout, K, exp):
    """

    Call:   upowBlk(pin, pout, K, exp)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       K : K
       exp : v

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('upow', pin, pout, [0,0], 0, [K, exp], [])
    return blk
