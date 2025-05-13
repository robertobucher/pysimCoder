from supsisim.RCPblk import RCPblk, RcpParam


def FH_5XXX_INIT_Blk(params: RcpParam) -> RCPblk:
    """
    Call:   FH_5XXX_INIT_Blk(params)

    Parameters
    ----------
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("FH_5XXX_INIT", [], [], [0, 0], 0, params)
