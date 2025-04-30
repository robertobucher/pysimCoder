from supsisim.RCPblk import RCPblk, RcpParam


def brikiAOBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   brikiAOBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    DAlist = [26]
    if params[0].value not in DAlist:
        raise ValueError(
            "You pass a channel which is not in the ABC Board PIN list: %i"
            % (params[0].value)
        )

    return RCPblk("brikiAO", pin, [], [0, 0], 1, params)
