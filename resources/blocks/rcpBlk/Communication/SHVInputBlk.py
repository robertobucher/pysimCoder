from supsisim.RCPblk import RCPblk
from scipy import size

def SHVInputBlk(pout):
    """

    Call:   SHVInputBlk(pout, name)

    Parameters
    ----------
       pout: connected output port(s)
       name: name of the input

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('shv_input', [], pout, [0,0], 0, [], [0])
    return blk

