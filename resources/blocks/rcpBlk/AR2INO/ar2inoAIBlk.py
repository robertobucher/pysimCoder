
from supsisim.RCPblk import RCPblk
from numpy import size

def ar2inoAIBlk(pout, ch, Umin, Umax):
    """

    Call:   ar2inoAIBlk(pout, pin, Umin, Umax)

    Parameters
    ----------
       pout: connected output port(s)
       pin : Pin
       Umin : Umin [V]
       Umax : Umax [V]

    Returns
    -------
       blk: RCPblk

    """

    if 2 <= ch <= 5:
        raise ValueError("Pin  %i is reserved for encoders!" % (ch))

    blk = RCPblk('ar2inoAI', [], pout, [0,0], 0, [Umin, Umax], [ch])
    return blk
