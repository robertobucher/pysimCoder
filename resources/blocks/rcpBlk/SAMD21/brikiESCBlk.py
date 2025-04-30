from supsisim.RCPblk import RCPblk, RcpParam


def brikiESCBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   brikiESCBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    PWMlist = [11, 13, 21, 22, 23, 24, 25]

    if params[0].value not in PWMlist:
        raise ValueError(
            "You pass a channel which is not in the ABC Board PIN list: %i"
            % (params[0].value)
        )
    return RCPblk("brikiESC", pin, [], [0, 0], 1, params)
