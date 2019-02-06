from supsisim.RCPblk import RCPblk
from scipy import size

def FH_CO_MotXBlk(pin, ID, res):
    """

    Call:   FH_CO_MotXBlk(pin, ID, res)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
       res : Resolution

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('FH_canopen_motX',pin,[],[0,0],1,[res],[ID])
    return blk

