from supsisim.RCPblk import RCPblk, RcpParam


def TCPsocketAsyncBlk(*args):
    """
    Call:   TCPsocketAsyncBlk(*args)

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

    if params[3].value < 1:
        raise ValueError("The downsampling ratio must be greater than zero!")

    params.insert(3, RcpParam("File descriptor", 0, RcpParam.Type.INT))
    params.append(RcpParam("Downsampling history", 0, RcpParam.Type.INT))
    return RCPblk("TCPsocketAsync", pin, pout, [0, 0], 1, params)
