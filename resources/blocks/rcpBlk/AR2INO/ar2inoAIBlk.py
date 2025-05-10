from supsisim.RCPblk import RCPblk, RcpParam


def ar2inoAIBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   ar2inoAIBlk(pout, params)

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

    return RCPblk("ar2inoAI", [], pout, [0, 0], 0, params)
