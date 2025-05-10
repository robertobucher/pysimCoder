from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def zdelayBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   zdelayBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pin) != 1:
        raise ValueError("Block have 1 input: received %i input ports" % size(pin))

    if size(pout) != 1:
        raise ValueError("Block have 1 output1: received %i output ports" % size(pout))

    return RCPblk("unitDelay", pin, pout, [0, 1], 0, params)
