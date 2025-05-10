from supsisim.RCPblk import RCPblk, RcpParam


def analogWrite_af(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   analogWrite_af(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("analogWrite_af", pin, [], [0, 0], 0, params)
