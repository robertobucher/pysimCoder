from supsisim.RCPblk import RCPblk
from scipy import size

def trigBlk(pin, pout, tp):
    """

    Call:   trigBlk(pin, pout, tp)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       tp : sin->1 cos->2 tan->3

    Returns
    -------
        blk  : RCPblk

    """
    
    nin = size(pin)
    if (nin != 1):
        raise ValueError("Block have 1 input: received %i input ports" % nin)

    nout = size(pout)
    if(nout != 1):
        raise ValueError("Block have 1 output1: received %i output ports" % nout)
        
    blk = RCPblk('trigo',pin,pout,[0,0],1,[],[tp])
    return blk

