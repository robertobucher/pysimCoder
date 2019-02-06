from supsisim.RCPblk import RCPblk
from scipy import size

def FH_5XXX_setTQBlk(pin, ID):
    """

    Call:   FH_5XXX_setTQBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
  
    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('FH_5XXX_setTQ', pin, [], [0,0], 1, [], [ID])
    return blk

