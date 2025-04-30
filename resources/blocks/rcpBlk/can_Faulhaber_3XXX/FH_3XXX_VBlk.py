from supsisim.RCPblk import RCPblk, RcpParam


def FH_3XXX_VBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   FH_3XXX_VBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("FH_3XXX_V", pin, [], [0, 0], 1, params)
