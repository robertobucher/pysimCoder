from supsisim.RCPblk import RCPblk, RcpParam


def stm32DIBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   stm32DIBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    p = params[0].value[-1]
    p = p.upper()

    pn = ord(p) - ord("A")

    if pn < 0 or pn > 6:
        raise ValueError(
            "Port should be between PORTA and PORTG not %s", (params[0].value)
        )

    if params[1].value < 0 or params[1].value > 15:
        raise ValueError("Channel should be between 0 and 15 not %i", (params[1].value))

    params[0].value = pn
    params[1].type = RcpParam.Type.INT
    return RCPblk("stm32DI", [], pout, [0, 0], 0, params)
