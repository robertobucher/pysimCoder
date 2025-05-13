from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def trigBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   trigBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
        params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if (nin := size(pin)) != 1:
        raise ValueError("Block have 1 input: received %i input ports" % nin)

    if (nout := size(pout)) != 1:
        raise ValueError("Block have 1 output1: received %i output ports" % nout)

    return RCPblk("trigo", pin, pout, [0, 0], 1, params)
