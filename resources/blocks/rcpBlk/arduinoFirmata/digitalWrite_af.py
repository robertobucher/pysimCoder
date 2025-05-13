from supsisim.RCPblk import RCPblk, RcpParam


def digitalWrite_af(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   digitalWrite_af(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("digitalWrite_af", pin, [], [0, 0], 0, params)
