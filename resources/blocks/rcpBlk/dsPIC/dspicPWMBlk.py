from supsisim.RCPblk import RCPblk, RcpParam


def dspicPWMBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   brikiPWMBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    p = params[0].value[-1]
    p = p.upper()

    pn = ord(p) - ord("0")

    if pn < 1 or pn > 3:
        raise ValueError(
            "PWM should be between PWM1 and PWM3 not %s"
            % (params[0].value)
        )
    
    params[0].value = pn
    params[0].type = RcpParam.Type.INT

    return RCPblk("dspicPWM", pin, [], [0, 0], 1, params)
