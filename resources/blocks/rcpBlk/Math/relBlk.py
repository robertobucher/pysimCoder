from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def relBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   relBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input ports
       pout: connected output port
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pout) != 1:
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    return RCPblk("rel", pin, pout, [0, 0], 1, params)
