from supsisim.RCPblk import RCPblk, RcpParam


def FH_3XXX_getVBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   FH_3XXX_getVBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("FH_3XXX_getV", [], pout, [0, 0], 0, params)
