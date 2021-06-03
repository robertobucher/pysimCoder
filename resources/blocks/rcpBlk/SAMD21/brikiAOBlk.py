
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiAOBlk(pin, ch, umin, umax):
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
    DAlist = [26]
    if ch not in DAlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (ch))
            
    blk = RCPblk('brikiAO', pin, [], [0,0], 1, [umin, umax], [ch])
    return blk
