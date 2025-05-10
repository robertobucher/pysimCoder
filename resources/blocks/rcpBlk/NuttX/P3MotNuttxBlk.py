from supsisim.RCPblk import RCPblk, RcpParam


def P3MotNuttxBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   P3MotNuttxBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params = RcpParam("File descriptor", 0, RcpParam.Type.INT)
    return RCPblk("nuttx_p3m_spi", pin, pout, [0, 0], params)
