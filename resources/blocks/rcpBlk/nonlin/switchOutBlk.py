from supsisim.RCPblk import RCPblk
from scipy import size

def switchOutBlk(pin, pout, val):
    """

    Call:   switchOutBlk(pin, pout, val)

    Parameters
    ----------
        pin: connected input ports
        pout: connected output ports
        val:   value to compare

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('switch_output',pin,pout,[0,0],1,[val],[])
    return blk
