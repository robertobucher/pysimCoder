from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def lutBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   lutBlk(pin, pout, params)

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
        raise ValueError("Block should 1 input port; received %i." % nin)

    if (nout := size(pout)) != 1:
        raise ValueError("Block should have 1 output port; received %i." % nout)

    params.append(RcpParam("Array Size", size(params[0].value), RcpParam.Type.INT))
    return RCPblk("lut", pin, pout, [0, 0], 1, params)
