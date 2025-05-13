from supsisim.RCPblk import RCPblk, RcpParam


def tos1a(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   tos1a(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("tos1a", pin, pout, [0, 0], 0, params)
