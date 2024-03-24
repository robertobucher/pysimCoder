
from supsisim.RCPblk import RCPblk
from numpy import size

def stm32PWMBlk(pin, ch, umin, umax):
    """

    Call:   stm32PWMBlk(pin, ch, umin, umax)

    Parameters
    ----------
       pin: connected input port(s)
       ch : Channel
       umin : Umin [V]
       umax : Umax [V]

    Returns
    -------
       blk: RCPblk

    """

    if ch<1 or ch>4:
        raise ValueError("Channel should have value 1 2 3  or 4 not %d" % ch)
     
    blk = RCPblk('stm32PWM', pin, [], [0,0], 1, [umin, umax], [ch])
    return blk
