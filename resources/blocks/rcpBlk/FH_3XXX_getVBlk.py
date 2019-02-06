from supsisim.RCPblk import RCPblk
from scipy import size

def FH_3XXX_getVBlk(pout, ID):
    """

    Call:   FH_3XXX_getVBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('FH_3XXX_getV', [], pout, [0,0], 0, [], [ID])
    return blk

