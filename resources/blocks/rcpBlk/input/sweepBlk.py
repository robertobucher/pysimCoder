from supsisim.RCPblk import RCPblk, RcpParam
from numpy import pi


def sweepBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   sweepBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params[0].value = 2 * pi * params[0].value
    return RCPblk("sweep", [], pout, [0, 0], 0, params)
