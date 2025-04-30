from supsisim.RCPblk import RCPblk, RcpParam


def UDPsocketTxBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   UDPsocketTxBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("UDPsocketTx", pin, [], [0, 0], 1, params)
