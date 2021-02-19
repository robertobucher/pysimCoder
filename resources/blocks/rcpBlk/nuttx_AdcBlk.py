
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttx_AdcBlk(pout, devname, umin, umax):
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

    blk = RCPblk('nuttx_Adc', [], pout, [0,0], 0, [umin, umax], [0], devname)
    return blk
