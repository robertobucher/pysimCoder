from supsisim.RCPblk import RCPblk
from scipy import size

def FH_3XXX_XBlk(pin, ID):
    """

    Call:   FH_3XXX_XBlk(pin, ID)

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

    blk = RCPblk('FH_3XXX_X',pin,[],[0,0],1,[],[ID])
    return blk

