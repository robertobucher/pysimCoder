
from supsisim.RCPblk import RCPblk
from numpy import size

def shmemInBlk(pout, name):
    """

    Call:   shmemInBlk(pout, name)

    Parameters
    ----------
       pout: connected output port(s)
       name : SHM name

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('shmemIn', [], pout, [0,0], 0, [], [0], name)
    return blk
