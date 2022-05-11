from supsisim.RCPblk import RCPblk
from scipy import size

def SHVOutputBlk(pin):
    """

    Call:   SHVOutputBlk(pout, name)

    Parameters
    ----------
       pout: connected input port(s)
       name: name of the input

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('shv_output', pin, [], [0,0], 0, [], [0])
    return blk

