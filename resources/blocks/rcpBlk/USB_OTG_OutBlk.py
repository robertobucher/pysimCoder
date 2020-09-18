
from supsisim.RCPblk import RCPblk
from scipy import size

def USB_OTG_OutBlk(pin, decim):
    """

    Call:   USB_OTG_OutBlk(pin, decim)

    Parameters
    ----------
       pin: connected input port(s)
       decim : Decimation

    Returns
    -------
       blk: RCPblk

    """

    decim = int(decim)
    
    blk = RCPblk('USB_OTG_Out', pin, [], [0,0], 1, [], [decim, 0])
    return blk
