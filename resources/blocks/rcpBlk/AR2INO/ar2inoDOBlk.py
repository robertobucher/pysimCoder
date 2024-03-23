
from supsisim.RCPblk import RCPblk
from numpy import size

def ar2inoDOBlk(pin, ch, th):
    """

    Call:   ar2inoDOBlk(pin, pin, th)

    Parameters
    ----------
       pin: connected input port(s)
       pin : Channel
       th : Threshold

    Returns
    -------
       blk: RCPblk

    """

    if 2 <= ch <= 5:
        raise ValueError("Pin  %i is reserved for encoders!" % (ch))

    blk = RCPblk('ar2inoDO', pin, [], [0,0], 1, [th], [ch])
    return blk
