from supsisim.RCPblk import RCPblk, RcpParam


def loggerBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   loggerBlk(pin, param)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("logger", pin, [], [0, 0], 1, params)
