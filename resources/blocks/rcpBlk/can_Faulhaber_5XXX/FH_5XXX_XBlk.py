from supsisim.RCPblk import RCPblk
from numpy import size

def FH_5XXX_XBlk(pin, candev, ID):
    """

    Call:   FH_5XXX_XBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
 
    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('FH_5XXX_X',pin,[],[0,0],1,[],[ID], candev)
    return blk

