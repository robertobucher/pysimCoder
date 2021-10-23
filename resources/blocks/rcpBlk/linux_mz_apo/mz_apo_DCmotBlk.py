
from supsisim.RCPblk import RCPblk
from scipy import size

def mz_apo_DCmotBlk(pin, pout, mot_ID):
    """

    Call:   mz_apo_DCmotBlk(pin, pout, mot_ID)

    Parameters
    ----------
       mot_ID: 0 or 1

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('mz_apo_DCmot', pin, pout, [0, 0], 0, [], [mot_ID, 0])
    return blk
