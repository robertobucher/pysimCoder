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

    PWMlist = [1]

    p = params[1].value[-1]
    p = p.upper()

    pn = ord(p) - ord("A")

    if pn < 0 or pn > 7:
        raise ValueError(
            "Port should be between PORTA and PORTH not %s"
            % (params[1].value)
        )
    
    if params[2].value < 0 or params[2].value > 15:
        raise ValueError(
            "Channel should be between 0 and 15 not %i"
            % (params[2].value))

    params[1].value = pn
    params[1].type = RcpParam.Type.INT

    if params[0].value not in PWMlist:
        raise ValueError(
            "You pass a channel which is not in  PWM list: %i"
            % (params[0].value)
        )

    return RCPblk("dspicPWM", pin, [], [0, 0], 1, params)
