from supsisim.RCPblk import RCPblk, RcpParam


def FH_5XXX_getVBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   FH_5XXX_getVBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("FH_5XXX_getV", [], pout, [0, 0], 0, params)
