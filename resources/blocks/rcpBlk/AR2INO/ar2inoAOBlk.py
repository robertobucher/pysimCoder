
from supsisim.RCPblk import RCPblk
from numpy import size

def ar2inoAOBlk(pin, ch, umin, umax):
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
            
    blk = RCPblk('ar2inoAO', pin, [], [0,0], 1, [umin, umax], [ch])
    return blk
