from supsisim.RCPblk import RCPblk
from numpy import size

def saturBlk(pin, pout, satP, satN):
    """

    Call:   saturBlk(pin, pout, satP, satN)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       satP : Upper saturation
       satN : Lower saturation

    Returns
    -------
       blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pin))
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('saturation',pin,pout,[0,0],1,[satP, satN],[])
    return blk

