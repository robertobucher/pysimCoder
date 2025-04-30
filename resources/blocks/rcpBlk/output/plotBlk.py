from supsisim.RCPblk import RCPblk, RcpParam


def plotBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   plotBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("plot", pin, [], [0, 0], 1, params)
