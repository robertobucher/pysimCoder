from supsisim.RCPblk import RCPblk, RcpParam


def deadzoneBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   deadzoneBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("deadzone", pin, pout, [0, 0], 1, params)
