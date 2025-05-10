from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def stm32SPIBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   stm32SPIBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if params[0].value == "NSS":
        pn = -1
        params[1].value = 0
    else:
        p = params[0].value[-1]
        p = p.upper()
        pn = ord(p) - ord("A")

    if size(pout) != size(params[2].value):
        raise ValueError("Number of outputs should fit the len of Outputs")

    if pn < -1 or pn > 6:
        raise ValueError(
            "Port should be between PORTA and PORTG not %s", (params[0].value)
        )

    if params[1].value < 0 or params[1].value > 15:
        raise ValueError("CSS should be between 0 and 15 not %i", (params[1].value))

    params[0].value = pn
    params[0].type = RcpParam.Type.INT
    return RCPblk("stm32SPI", pin, pout, [0, 0], 1, params)
