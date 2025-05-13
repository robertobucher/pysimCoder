from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def saturBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   saturBlk(pin, pout, params)

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
        raise ValueError("Block should have 1 input port; received %i." % nin)
    if (nout := size(pout)) != 1:
        raise ValueError("Block should have 1 output port; received %i." % nout)
    return RCPblk("saturation", pin, pout, [0, 0], 1, params)
