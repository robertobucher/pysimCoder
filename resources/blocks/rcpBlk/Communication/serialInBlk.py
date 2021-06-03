
from supsisim.RCPblk import RCPblk
from scipy import size

def serialInBlk(pout, port):
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

    blk = RCPblk('serialIn', [], pout, [0,0], 0, [], [0], port)
    return blk
