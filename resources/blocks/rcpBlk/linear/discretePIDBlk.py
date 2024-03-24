from supsisim.RCPblk import RCPblk
from numpy import size

def discretePIDBlk(pin, pout, Kp, Ki, Kd, min_val, max_val):
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

    blk = RCPblk('discretePID', pin, pout, [0,0], 1, [Kp, Ki, Kd, min_val, max_val, 0, 0], [])
    return blk
