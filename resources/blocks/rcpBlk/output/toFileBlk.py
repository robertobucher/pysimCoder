from supsisim.RCPblk import RCPblk
from numpy import size

def toFileBlk(pin, fname):
    """

    Call:   toFileBlk(pin, fname)

    Parameters
    ----------
       pin: connected input port(s)
       fname : File name

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('toFile',pin,[],[0,0],1,[],[], fname)
    return blk

