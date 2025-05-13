from supsisim.RCPblk import RCPblk


def park_inverseBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   park_inverseBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("inverse_park", pin, pout, [0, 0], 1)
