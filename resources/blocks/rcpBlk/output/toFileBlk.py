from supsisim.RCPblk import RCPblk, RcpParam


def toFileBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   toFileBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("toFile", pin, [], [0, 0], 1, params)
