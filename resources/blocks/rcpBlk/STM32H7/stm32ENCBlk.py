
from supsisim.RCPblk import RCPblk
from numpy import size
import numpy as np

def stm32ENCBlk(pout, res):
    """

    Call:   stm32ENCBlk(pout, res)

    Parameters
    ----------
       pout: connected output port(s)
       res : Resolution

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('stm32ENC', [], pout, [0,0], 0, [res], [])
    return blk
