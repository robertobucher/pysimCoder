from supsisim.RCPblk import RCPblk
from numpy import size

def serialOutBlk(pin, port):
    """

    Call:   serialIn(pout, port)

    Parameters
    ----------
       pout: connected output port(s)
       port : Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('serialOut', pin, [],  [0,0], 1, [], [0], port)
    return blk
