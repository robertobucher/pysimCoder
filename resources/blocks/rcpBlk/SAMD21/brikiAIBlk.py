
from supsisim.RCPblk import RCPblk
from numpy import size

def brikiAIBlk(pout, ch, umin, umax):
    """

    Call:   brikiAIBlk(pout, port)

    Parameters
    ----------
       pout: connected output port(s)
       port : Input

    Returns
    -------
       blk: RCPblk

    """
        
    ADlist = [26, 27, 28, 29, 30, 31, 20]
    if ch not in ADlist:
        raise ValueError("You pass a channel which is not in the ABC Board PIN list: %i" % (ch))
    
    blk = RCPblk('brikiAI', [], pout, [0,0], 0, [umin, umax], [ch])
    return blk
