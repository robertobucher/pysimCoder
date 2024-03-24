from supsisim.RCPblk import RCPblk
from numpy import size

def absBlk(pin, pout):
    """

    Call:   absBlk(pin, pout)

    Parameters
    ----------
        pin : connected input ports
        pout: connected output ports

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != size(pin)):
        raise ValueError("Block should have same input and output port sizes; received %i %i." % (size(pin),size(pout)))
    blk = RCPblk('absV',pin,pout,[0,0],1,[],[])
    return blk

