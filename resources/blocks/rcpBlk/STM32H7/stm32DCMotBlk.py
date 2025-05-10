from supsisim.RCPblk import RCPblk, RcpParam


def stm32DCMotBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   stm32DCMotBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    p1 = params[1].value[-1]
    p1 = p1.upper()
    pn1 = ord(p1) - ord("A")

    p2 = params[3].value[-1]
    p2 = p2.upper()
    pn2 = ord(p2) - ord("A")

    if pn1 < 0 or pn1 > 6:
        raise ValueError(
            "Port should be between PORTA and PORTG not %s", (params[1].value)
        )

    if params[2].value < 0 or params[2].value > 15:
        raise ValueError("Channel should be between 0 and 15 not %i", (params[2].value))

    if pn2 < 0 or pn2 > 6:
        raise ValueError(
            "Port should be between PORTA and PORTG not %s", (params[3].value)
        )

    if params[4].value < 0 or params[4].value > 15:
        raise ValueError("Channel should be between 0 and 15 not %i", (params[4].value))

    if params[0].value < 1 or params[0].value > 4:
        raise ValueError(
            "Channel should have value 1 2 3  or 4 not %d" % params[0].value
        )

    params[1].value = pn1
    params[1].type = RcpParam.Type.INT
    params[3].value = pn2
    params[3].type = RcpParam.Type.INT
    return RCPblk("stm32DCMot", pin, [], [0, 0], 1, params)
