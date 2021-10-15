
from supsisim.RCPblk import RCPblk
from scipy import size

def mz_apo_ENCBlk(pout, channel, init_val):
    """

    Call:   mz_apo_ENCBlk(pout, channel, init_val)

    Parameters
    ----------
       channel: integer 0 to 2
       init_val: integer of initial value

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('mz_apo_ENC', [], pout, [0, 0], 0, [], [channel, init_val, 0, 0])
    return blk
