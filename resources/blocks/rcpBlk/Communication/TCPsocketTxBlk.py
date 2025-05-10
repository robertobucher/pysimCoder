from supsisim.RCPblk import RCPblk, RcpParam


def TCPsocketTxRxBlk(*args):
    """
    Call:   TCPsocketTxRxBlk(*args)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if len(args) == 3:
        pin, pout, params = args
    elif len(args) == 2:
        pout = []
        pin, params = args

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("TCPsocketTxRx", pin, pout, [0, 0], 1, params)
