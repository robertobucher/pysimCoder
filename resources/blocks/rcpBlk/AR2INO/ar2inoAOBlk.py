
from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size

def ar2inoAOBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   brikiAOBlk(pin, pin)

    Parameters
    ----------
       pin: connected input port(s)
       pin : Pin

    Returns
    -------
       blk: RCPblk

    """
    DAlist = [14]
    if ch not in DAlist:
        raise ValueError("You pass a channel which is correct %i" % (ch))
            
    blk = RCPblk('ar2inoAO', pin, [], [0,0], 1, params)
    return blk
