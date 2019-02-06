from supsisim.RCPblk import RCPblk
from scipy import size

def printBlk(pin):
    """

    Call:   printBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('print',pin,[],[0,0],1,[],[])
    return blk

