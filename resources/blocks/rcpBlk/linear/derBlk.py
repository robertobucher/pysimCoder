from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def derBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Continous integral block

    Call: derBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pin) != 1:
        raise ValueError("Block have 1 input: received %i input ports" % size(pin))

    if size(pout) != 1:
        raise ValueError("Block have 1 output1: received %i output ports" % size(pout))

    params = [
        RcpParam("Internal 1", 0, RcpParam.Type.DOUBLE),
        RcpParam("Internal 2", 0, RcpParam.Type.DOUBLE),
        RcpParam("Internal 3", 0, RcpParam.Type.DOUBLE),
    ]
    return RCPblk("der", pin, pout, [1, 0], 1, params)
