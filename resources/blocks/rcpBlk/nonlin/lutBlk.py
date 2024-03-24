from supsisim.RCPblk import RCPblk
from numpy import size

def lutBlk(pin, pout, coeff):
    """

    Call:   lutBlk(pin, pout, coeff)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       coeff : Coeff 

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should 1 input port; received %i." % size(pin))

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('lut',pin,pout,[0,0],1,[coeff],[size(coeff)])
    return blk

