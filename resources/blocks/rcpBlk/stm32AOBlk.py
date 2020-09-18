
from supsisim.RCPblk import RCPblk
from scipy import size

def stm32AOBlk(pin, ch, umin, umax):
    """

    Call:   stm32AOBlk(pin, umin, umax)

    Parameters
    ----------
       pin: connected input port(s)
       umin : Umin [V]
       umax : Umax [V]

    Returns
    -------
       blk: RCPblk

    """

    if ch<0 or ch>1:
        raise ValueError("Channel should have value 0 or 1 not %d" % ch)
    
    blk = RCPblk('stm32AO', pin, [], [0,0], 1, [umin, umax], [ch])
    return blk
