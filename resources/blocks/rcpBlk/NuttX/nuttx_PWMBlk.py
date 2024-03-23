import numpy as np
from supsisim.RCPblk import RCPblk
from numpy import size

def nuttx_PWMBlk(pin, port, ch, freq, umin, umax):
    """

    Call:   nuttx_PWMBlk(pin, port, umin, umax)

    Parameters
    ----------
       pin: connected input port(s)
       port : Port
       umin : Umin [V]
       umax : Umax [V]

    Returns
    -------
       blk: RCPblk

    """

    if(size(pin) != size(ch)):
        raise ValueError("Number of inputs (%i) should match number of channels (%i)" % (size(pin),size(ch)))

    if(np.unique(ch).size != size(ch)):
         raise ValueError("PWM block: duplicate channels!")

    ch.append(0)

    blk = RCPblk('nuttx_PWM', pin, [], [0,0], 1, [freq, umin, umax], ch, port)
    return blk
