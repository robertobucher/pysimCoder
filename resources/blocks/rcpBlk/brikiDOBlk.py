
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiDOBlk(pin, ch, thr):
    """

    Call:   brikiDOBlk(pin, ch, thr)

    Parameters
    ----------
       pin: connected input port(s)
       ch : Channel
       thr : Threshold

    Returns
    -------
       blk: RCPblk

    """

    DOlist = [9, 11, 13, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31]

    if ch not in DOlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (ch))
    
    blk = RCPblk('brikiDO', pin, [], [0,0], 1, [thr], [ch])
    return blk
