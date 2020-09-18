
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiESCBlk(pin, ch, umin, umax):
    """

    Call:   brikiESCBlk(pin, ch, umin, umax)

    Parameters
    ----------
       pin: connected input port(s)
       ch : Pin
       umin : Umin [V]
       umax : Umax [V]

    Returns
    -------
       blk: RCPblk

    """

    PWMlist = [11, 13, 21, 22, 23, 24, 25]

    if ch not in PWMlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (ch))
    blk = RCPblk('brikiESC', pin, [], [0,0], 1, [umin, umax], [ch])
    return blk
