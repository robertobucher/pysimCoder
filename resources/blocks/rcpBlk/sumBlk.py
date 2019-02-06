from supsisim.RCPblk import RCPblk
from scipy import size

def sumBlk(pin, pout, Gains):
    """

    Call:   sumBlk(pin, pout, Gains)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       Gains : Gains

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != size(Gains)):
        raise ValueError("Number of inputs (%i) should match Gain Size (%i)" % (size(pin),size(Gains)))
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('sum',pin,pout,[0,0],1,Gains,[])
    return blk

