from supsisim.RCPblk import RCPblk, RcpParam


def genericBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call: genericBlk(pin, pout, params)

    Parameters
    ----------
        pin: connected input ports
        pout: connected output ports
        params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    fname = params[-1].value
    nx = params[0].value
    uy = params[1].value
    params.pop(-1)
    params.pop(0)
    params.pop(1)

    return RCPblk(fname, pin, pout, nx, uy, params)
