from supsisim.RCPblk import RCPblk, RcpParam


def ar2inoDIBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   ar2inoDIBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if 2 <= params[0].value <= 5:
        raise ValueError("Pin  %i is reserved for encoders!" % (params[0].value))

    return RCPblk("ar2inoDI", [], pout, [0, 0], 0, params)
