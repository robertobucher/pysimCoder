from supsisim.RCPblk import RCPblk
from scipy import size

def FH_5XXX_VBlk(pin, ID):
    """

    Call:   FH_5XXX_VBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
   
    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('FH_5XXX_V', pin, [], [0,0], 1, [], [ID])
    return blk

