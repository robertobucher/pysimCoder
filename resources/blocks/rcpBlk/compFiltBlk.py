
from supsisim.RCPblk import RCPblk
from scipy import size

def compFiltBlk(pin, pout, alfa, ts):
    """

    Call:   compFiltBlk(pin, pout, alfa, ts)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       alfa : Alpha
       ts : Sampling Time

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('compFilt', pin, pout, [0,1], 0, [alfa, ts, 0.0], [])
    return blk
