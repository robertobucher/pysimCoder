
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiDIBlk(pout, ch):
    """

    Call:   brikiDIBlk(pout, ch)

    Parameters
    ----------
       pout: connected output port(s)
       ch : Channel

    Returns
    -------
       blk: RCPblk

    """

    DIlist = [11, 13, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31]

    if ch not in DIlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (ch))
        
    blk = RCPblk('brikiDI', [], pout, [0,0], 0, [], [ch])
    return blk
