from supsisim.RCPblk import RCPblk


def clarke_inverseBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   clarke_inverseBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("inverse_clarke", pin, pout, [0, 0], 1)
