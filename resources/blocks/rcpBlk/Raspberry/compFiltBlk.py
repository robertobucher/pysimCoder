from supsisim.RCPblk import RCPblk, RcpParam


def compFiltBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   compFiltBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal 1", 0, RcpParam.Type.DOUBLE))
    params.append(RcpParam("Internal 2", 0, RcpParam.Type.DOUBLE))
    return RCPblk("compFilt", pin, pout, [0, 1], 0, params)
