from supsisim.RCPblk import RCPblk
from numpy import size


def setup_af(port, baud):
    """

    Call:   setup_af(pout, port)

    Parameters
    ----------
       pout: connected output port(s)
       port : Port
       baud : Baud

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('setup_af', [], [], [0, 0], 0, [], baud, port)
    return blk
