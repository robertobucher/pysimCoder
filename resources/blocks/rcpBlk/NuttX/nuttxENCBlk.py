
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttxENCBlk(pout, port, res, reset):
    """

    Call:   nuttxENCBlk(pout, port, res, reset)

    Parameters
    ----------
       pout: connected output port(s)
       port : Port
       res : Resolution
       reset: initial counter reset

    Returns
    -------
       blk: RCPblk

    """

    if (size(pout) != 1 and size(pout) != 3):
       raise ValueError("NuttxENC: Number of outputs (%i) should be 1 or 3" % (size(pout)))

    blk = RCPblk('nuttxENC', [], pout, [0,0], 0, [4*res], [reset, 0], port)
    return blk
