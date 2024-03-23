
from supsisim.RCPblk import RCPblk
from numpy import size

def brikiPWMBlk(pin, ch, umin, umax, freq):
    """

    Call:   brikiPWMBlk(pin, ch)

    Parameters
    ----------
       pin: connected input port(s)
       ch : Pin

    Returns
    -------
       blk: RCPblk

    """

    PWMlist = [11, 13, 21, 22, 23, 24, 25]

    if ch not in PWMlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (ch))
    
    blk = RCPblk('brikiPWM', pin, [], [0,0], 1, [umin, umax], [ch, freq])
    return blk
