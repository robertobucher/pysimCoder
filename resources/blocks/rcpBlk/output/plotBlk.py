from supsisim.RCPblk import RCPblk
from numpy import size

def plotBlk(pin, fname):
    """

    Call:   plotBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('plot',pin,[],[0,0],1,[],[], fname)
    return blk

