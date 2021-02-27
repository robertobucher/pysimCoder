
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttxDIBlk(pout, port):
    """

    Call:   nuttxDIBlk(pout, port)

    Parameters
    ----------
       pout: connected output port(s)
       port : Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('nuttxDI', [], pout, [0,0], 0, [], [0], port)
    return blk
