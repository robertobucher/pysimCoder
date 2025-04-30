from supsisim.RCPblk import RCPblk, RcpParam


def scopeStream(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   scopeStream(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("scope", pin, [], [0, 0], 1, params)
