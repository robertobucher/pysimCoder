from supsisim.RCPblk import RCPblk, RcpParam


def hall3ph2secBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   hall3ph2secBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("hall3ph2sec", pin, pout, [0, 0], 1, params)
