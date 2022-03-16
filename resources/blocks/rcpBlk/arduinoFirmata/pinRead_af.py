
from supsisim.RCPblk import RCPblk
from scipy import size

def pinRead_af(pout, devicepin, pinmode):
    """

    Call:   pinRead_af(pout, pinmode, devicepin)

    Parameters
    ----------
       pout: connected output port(s)
       devicepin : Device pin
       pinmode : Pin mode

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('pinRead_af', [], pout, [0,0], 0, [], [devicepin, pinmode])
    return blk
