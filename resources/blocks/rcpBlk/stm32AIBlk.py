
from supsisim.RCPblk import RCPblk
from scipy import size

def stm32AIBlk(pout, ch, umin, umax):
    """

    Call:   stm32AIBlk(pout, ch, umin, umax)

    Parameters
    ----------
       pout: connected output port(s)
       ch : Channel
       umin : Umin [V]
       umax : Umax [V]

    Returns
    -------
       blk: RCPblk

    """

    if ch > 9 or  ch<0:
        raise ValueError("Channel should be in range 0-7 not  %d" % ch)
        
    blk = RCPblk('stm32AI', [], pout, [0,0], 0, [umin, umax], [ch])
    return blk
