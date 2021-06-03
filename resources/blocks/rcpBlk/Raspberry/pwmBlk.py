
from supsisim.RCPblk import RCPblk
from scipy import size

def pwmBlk(pin, ch, umin, umax):
    """

    Call:   pwmBlk(pout, ch)

    Parameters
    ----------
       pout: connected output port(s)
       ch : Channel

    Returns
    -------
       blk: RCPblk

    """
    if ch == 0:
        ch = 1
    else:
        ch = 23
        
    blk = RCPblk('pwm', pin, [], [0,0], 1, [umin, umax], [ch])
    return blk
