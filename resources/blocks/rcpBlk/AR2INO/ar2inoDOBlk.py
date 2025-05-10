from supsisim.RCPblk import RCPblk, RcpParam


def ar2inoDOBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   ar2inoDOBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if 2 <= params[0].value <= 5:
        raise ValueError("Pin  %i is reserved for encoders!" % (params[0].value))

    return RCPblk("ar2inoDO", pin, [], [0, 0], 1, params)
