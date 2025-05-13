from supsisim.RCPblk import RCPblk, RcpParam


def brikiDCMotBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   brikiDCMotBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    CHlist = [11, 13, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31]
    if params[0].value not in CHlist:
        raise ValueError(
            "You pass a channel which is not in the ABC Board PIN list: %i"
            % (params[0].value)
        )
    if params[1].value not in CHlist:
        raise ValueError(
            "You pass a channel which is not in the ABC Board PIN list: %i"
            % (params[1].value)
        )
    if params[2].value not in CHlist:
        raise ValueError(
            "You pass a channel which is not in the ABC Board PIN list: %i"
            % (params[2].value)
        )

    return RCPblk("brikiDCMot", pin, [], [0, 0], 1, [], params)
