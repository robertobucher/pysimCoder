
from supsisim.RCPblk import RCPblk
from scipy import size

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

    blk = RCPblk('nuttx_PWM', pin, [], [0,0], 1, [umin, umax, freq], [ch], port)
    return blk
