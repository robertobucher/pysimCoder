
from supsisim.RCPblk import RCPblk
from numpy import size

def ar2inoSetupBlk(channel, baud):
    """

    Call:   ar2inoSetup(channel, baud)

    Parameters
    ----------
       channel : Port
       baud : Baud

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('ar2inoSetup', [], [], [0,0], 0, [], [baud], channel)
    return blk
