
from supsisim.RCPblk import RCPblk
from scipy import size

def fromESP32Blk(pout):
    """

    Call:   fromESP32Blk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('fromESP32', [], pout, [0,0], 0, [], [])
    return blk
