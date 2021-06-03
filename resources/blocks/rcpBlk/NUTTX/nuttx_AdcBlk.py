
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttx_AdcBlk(pout, devname, ch, umin, umax):
    """

    Call:   nuttx_AdcBlk(pout, devname)

    Parameters
    ----------
       pout: connected output port(s)
       devname : Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('nuttx_Adc', [], pout, [0,0], 0, [umin, umax], [ch-1], devname)
    return blk
