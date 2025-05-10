from supsisim.RCPblk import RCPblk


def nullBlk(pin: list[int]) -> RCPblk:
    """
    Call:   nullBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("toNull", pin, [], [0, 0], 1)
