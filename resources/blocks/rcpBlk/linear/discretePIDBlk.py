from supsisim.RCPblk import RCPblk, RcpParam


def discretePIDBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   discretePIDBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Last Error", 0, RcpParam.Type.DOUBLE))
    params.append(RcpParam("Integral Sum", 0, RcpParam.Type.DOUBLE))
    return RCPblk("discretePID", pin, pout, [0, 0], 1, params)
