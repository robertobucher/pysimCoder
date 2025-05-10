from supsisim.RCPblk import RCPblk


def divBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   divBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("Div", pin, pout, [0, 0], 1)
