
from supsisim.RCPblk import RCPblk
from scipy import size

def setup_af(pout, port):
    """

    Call:   setup_af(pout, port)

    Parameters
    ----------
       pout: connected output port(s)
       port : Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('setup_af', [], pout, [0,0], 0, [], [], port)
    return blk
