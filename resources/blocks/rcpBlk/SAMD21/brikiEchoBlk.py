
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiEchoBlk(pout, trg, echo):
    """

    Call:   brikiEchoBlk(pout, trg, echo)

    Parameters
    ----------
       pout: connected output port(s)
       trg : Trigger Pin
       echo : Echo Pin

    Returns
    -------
       blk: RCPblk

    """
    CHlist = [11, 13, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31]
    if trg not in CHlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (trg))
    if echo not in CHlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (echo))

    blk = RCPblk('brikiEcho', [], pout, [0,0], 0, [], [trg, echo])
    return blk
