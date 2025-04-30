from supsisim.RCPblk import RCPblk, RcpParam


def FH_5XXX_VBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   FH_5XXX_VBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("FH_5XXX_V", pin, [], [0, 0], 1, params)
