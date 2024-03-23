
from supsisim.RCPblk import RCPblk
from numpy import size

def serialInFloatBlk(pout, port):
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

    blk = RCPblk('serialInFloat', [], pout, [0,0], 0, [], [0], port)
    return blk
