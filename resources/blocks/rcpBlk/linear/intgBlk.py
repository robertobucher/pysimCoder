from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def intgBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Continous integral block

    Call: intgBlk(pin, pout, params)

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

    params.insert(0, RcpParam("Internal 1", 0, RcpParam.Type.DOUBLE))
    return RCPblk("integral", pin, pout, [1, 0], 0, params)
