from supsisim.RCPblk import RCPblk
from scipy import size

def nuttx_DACBlk(pin, port, channel):
    """

    Call:   nuttx_DACBlk(pin, port, channel)

    Parameters
    ----------
       pin: connected input port
       port : Port
       channel: Channel

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('nuttx_DAC', pin, [], [0,0], 1, [], [channel], port)
    return blk
