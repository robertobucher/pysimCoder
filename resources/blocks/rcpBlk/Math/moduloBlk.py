
from supsisim.RCPblk import RCPblk

def moduloBlk(pin, pout):
    """

    Call:   moduloBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('pysim_modulo', pin, pout, [0,0], 1, [], [])
    return blk
