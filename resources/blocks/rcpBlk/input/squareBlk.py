from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def squareBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   squareBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pout) != 1:
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    return RCPblk("squareSignal", [], pout, [0, 0], 0, params)
