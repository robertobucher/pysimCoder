from supsisim.RCPblk import RCPblk, RcpParam


def stm32AOBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   stm32AOBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if params[0].valu < 0 or params[0].valu > 1:
        raise ValueError("Channel should have value 0 or 1 not %d" % params[0].valu)

    return RCPblk("stm32AO", pin, [], [0, 0], 1, params)
