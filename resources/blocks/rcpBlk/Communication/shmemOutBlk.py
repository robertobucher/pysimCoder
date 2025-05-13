
from supsisim.RCPblk import RCPblk
from numpy import size

def shmemOutBlk(pin, name):
    """

    Call:   shmemOutBlk(pin, name)

    Parameters
    ----------
       pin: connected input port(s)
       name : SHM name

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('shmemOut', pin, [], [0,0], 1, [], [0], name)
    return blk
