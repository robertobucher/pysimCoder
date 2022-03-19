from supsisim.RCPblk import RCPblk
from scipy import size


def tos1a(pin, pout, port, lamp, vent, temp):
    """

    Call:   tos1a(pin, pout, port, lamp, vent, temp)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       port : Port
       lamp : TimeConstLamp
       vent : TimeConstVent
       temp : TimeConstTemp

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('tos1a', pin, pout, [0, 0], 0, [lamp, vent, temp], [0], port)
    return blk
