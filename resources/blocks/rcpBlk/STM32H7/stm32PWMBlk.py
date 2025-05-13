from supsisim.RCPblk import RCPblk, RcpParam


def stm32PWMBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   stm32PWMBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if params[0].value < 1 or params[0].value > 4:
        raise ValueError(
            "Channel should have value 1 2 3  or 4 not %d" % params[0].value
        )

    return RCPblk("stm32PWM", pin, [], [0, 0], 1, params)
