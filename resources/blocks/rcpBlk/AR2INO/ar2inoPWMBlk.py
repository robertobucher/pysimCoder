
from supsisim.RCPblk import RCPblk
from numpy import size

def ar2inoPWMBlk(pin, ch, Umin, Umax):
    """

    Call:   ar2inoPWMBlk(pin, pin, Umin, Umax)

    Parameters
    ----------
       pin: connected input port(s)
       pin : Pin
       Umin : Umin [V]
       Umax : Umax [V]

    Returns
    -------
       blk: RCPblk

    """

    if 2 <= ch <= 5:
        raise ValueError("Pin  %i is reserved for encoders!" % (ch))

    blk = RCPblk('ar2inoPWM', pin, [], [0,0], 1, [Umin, Umax], [ch])
    return blk
