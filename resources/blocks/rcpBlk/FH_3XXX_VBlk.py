from supsisim.RCPblk import RCPblk
from scipy import size

def FH_3XXX_VBlk(pin, candev, ID):
    """

    Call:   FH_3XXX_VBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
  
    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('FH_3XXX_V', pin, [], [0,0], 1, [], [ID], candev)
    return blk

