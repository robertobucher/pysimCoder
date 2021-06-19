from supsisim.RCPblk import RCPblk
from scipy import size

def nuttx_DHTXXBlk(pout, port):
    """

    Call:   nuttx_DHTXXBlk(pout, port)

    Parameters
    ----------
       pout: connected output port(s)
       port : Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('nuttx_DHTXX', [], pout, [0,0], 0, [], [], port)
    return blk
