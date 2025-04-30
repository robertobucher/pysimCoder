from supsisim.RCPblk import RCPblk, RcpParam


def stm32AIBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   stm32AIBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if params[0].value > 9 or params[0].value < 0:
        raise ValueError("Channel should be in range 0-7 not  %d" % params[0].value)

    return RCPblk("stm32AI", [], pout, [0, 0], 0, params)
