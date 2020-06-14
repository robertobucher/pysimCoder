
from supsisim.RCPblk import RCPblk
from scipy import size

def serialOutBlk(pin, decim):
    """

    Call:   serialOut(pin, port)

    Parameters
    ----------
       pin: connected input port(s)
       port : Port

    Returns
    -------
       blk: RCPblk

    """

    decim = int(decim)
    
    blk = RCPblk('serialOut', pin, [], [0,0], 1, [], [decim, 0])
    return blk
