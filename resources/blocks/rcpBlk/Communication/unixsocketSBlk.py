from supsisim.RCPblk import RCPblk, RcpParam


def unixsocketSBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   unixsocketSBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params[0].value = "/tmp/" + params[0].value
    params.append(RcpParam("Socket", 0, RcpParam.Type.INT))
    return RCPblk("unixsockS", [], pout, [0, 0], 0, params)
