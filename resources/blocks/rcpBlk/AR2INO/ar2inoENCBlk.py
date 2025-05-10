from supsisim.RCPblk import RCPblk, RcpParam


def ar2inoENCBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   ar2inoENCBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    params.insert(0, RcpParam("Resolution multiplier", 4, RcpParam.Type.INT))
    return RCPblk("ar2inoENC", [], pout, [0, 0], 0, params)
