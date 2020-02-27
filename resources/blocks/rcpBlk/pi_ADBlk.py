
from supsisim.RCPblk import RCPblk
from scipy import size

def pi_ADBlk(pout, dev, ch, vref):
    """

    Call:   pi_ADBlk(pout, dev, ch)

    Parameters
    ----------
       pout: connected output port(s)
       dev : Device
       ch : Channel

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('pi_AD', [], pout, [0,0], 0, [vref], [dev, ch, 0])
    return blk
