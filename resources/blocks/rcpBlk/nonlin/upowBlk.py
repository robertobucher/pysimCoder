from supsisim.RCPblk import RCPblk, RcpParam


def upowBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   upowBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("upow", pin, pout, [0, 0], 0, params)
