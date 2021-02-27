
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttxDOBlk(pin, port, th):
    """

    Call:   nuttxDOBlk(pin, port, th)

    Parameters
    ----------
       pin: connected input port(s)
       port : Port
       th : Threshold

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('nuttxDO', pin, [], [0,0], 1, [th], [0], port)
    return blk
