from supsisim.RCPblk import RCPblk, RcpParam


def pwmBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   pwmBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """
    if params[0].value == 0:
        params[0].value = 1
    else:
        params[0].value = 23

    return RCPblk("pwm", pin, [], [0, 0], 1, params)
