from supsisim.RCPblk import RCPblk, RcpParam


def ledStream(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   ledStream(pin, param)

    Parameters
    ----------
       pout: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("led", pin, [], [0, 0], 1, params)
