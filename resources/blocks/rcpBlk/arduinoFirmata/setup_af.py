from supsisim.RCPblk import RCPblk, RcpParam


def setup_af(params: RcpParam) -> RCPblk:
    """

    Call:   setup_af(params)

    Parameters
    ----------
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("setup_af", [], [], [0, 0], 0, [], params)
