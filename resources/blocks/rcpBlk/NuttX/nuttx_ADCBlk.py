import numpy as np
from supsisim.RCPblk import RCPblk
from numpy import size

def nuttx_ADCBlk(pout, devname, ch, swtrig, res, umin, umax):
    """

    Call:   nuttx_AdcBlk(pout, devname)

    Parameters
    ----------
       pout: connected output port(s)
       devname : Port
       ch: array of ADC channels
       swtrig: flag to either SW trigger or not
       res: resolution of the ADC
       umin: minimal value
       umax: maximum value

    Returns
    -------
       blk: RCPblk

    """

    if(size(pout) != size(ch)):
        raise ValueError("ADC block: number of inputs (%i) should match number of channels (%i)" % (size(pout),size(ch)))

    if(np.unique(ch).size != size(ch)):
        raise ValueError("ADC block: duplicate channels!")

    ch.append(res)
    ch.append(0)      # file descriptor
    ch.append(0)      # number of configured chanels get by ioctl command
    ch.append(swtrig) # boolean flag to SW trigger or not

    blk = RCPblk('nuttx_ADC', [], pout, [0,0], 0, [umin, umax], ch, devname)
    return blk
