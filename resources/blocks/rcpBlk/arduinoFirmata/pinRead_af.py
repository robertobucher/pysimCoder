from supsisim.RCPblk import RCPblk, RcpParam


def pinRead_af(pout: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   pinRead_af(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("pinRead_af", [], pout, [0, 0], 0, params)
