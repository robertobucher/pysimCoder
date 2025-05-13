from supsisim.RCPblk import RCPblk, RcpParam


def unixsocketCBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   unixsocketCBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params[0].value = "/tmp/" + params[0].value
    params.append(RcpParam("Socket", 0, RcpParam.Type.INT))
    return RCPblk("unixsockC", pin, [], [0, 0], 1, params)
