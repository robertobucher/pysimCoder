from supsisim.RCPblk import RCPblk, RcpParam


def stm32ENCBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   stm32ENCBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("stm32ENC", [], pout, [0, 0], 0, params)
