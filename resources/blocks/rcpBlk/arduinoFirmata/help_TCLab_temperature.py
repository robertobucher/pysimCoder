from supsisim.RCPblk import RCPblk
from numpy import size


def help_TCLab_temperature(pin, pout):
    """

    Call:   help_TCLab_temperature(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('help_TCLab_temperature', pin, pout, [0, 0], 0,
                 [], [])
    return blk
