from supsisim.RCPblk import RCPblk, RcpParam


def encoderRead_af(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   encoderRead_af(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("encoderRead_af", [], pout, [0, 0], 0, params)
