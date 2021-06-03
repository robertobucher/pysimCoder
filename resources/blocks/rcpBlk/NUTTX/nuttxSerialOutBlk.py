
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttxSerialOutBlk(pin, decim):
    """

    Call:   brikiSerialOut(pin, decim)

    Parameters
    ----------
       pin: connected input port(s)
       decim : decimation

    Returns
    -------
       blk: RCPblk

    """

    decim = int(decim)
    
    blk = RCPblk('serialOut', pin, [], [0,0], 1, [], [decim, 0])
    return blk
