from supsisim.RCPblk import RCPblk, RcpParam


def customRead_af(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   customRead_af(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("customRead_af", [], pout, [0, 0], 0, params)
