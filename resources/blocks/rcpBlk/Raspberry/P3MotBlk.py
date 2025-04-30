from supsisim.RCPblk import RCPblk


def P3MotBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   P3MotBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("P3M_spi", pin, pout, [0, 0], 0)
