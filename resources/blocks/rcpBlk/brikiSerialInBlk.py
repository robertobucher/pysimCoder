
from supsisim.RCPblk import RCPblk
from scipy import size

def brikiSerialInBlk(pout):
    """

    Call:   brikiSerialIn(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
       blk: RCPblk

    """

    decim = int(decim)
    
    blk = RCPblk('serialIn', [], pout, [0,0], 0, [], [])
    return blk
