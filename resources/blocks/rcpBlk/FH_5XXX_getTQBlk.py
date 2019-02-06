from supsisim.RCPblk import RCPblk
from scipy import size

def FH_5XXX_getTQBlk(pout, ID):
    """

    Call:   FH_5XXX_getTQBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('FH_5XXX_getTQ', [], pout, [0,0], 0, [], [ID])
    return blk

