from supsisim.RCPblk import RCPblk, RcpParam


def brikiDOBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   brikiDOBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    DOlist = [9, 11, 13, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31]

    if params[0].value not in DOlist:
        raise ValueError(
            "You pass a channel which is not in the ABC Board PIN list: %i"
            % (params[0].value)
        )

    return RCPblk("brikiDO", pin, [], [0, 0], 1, params)
