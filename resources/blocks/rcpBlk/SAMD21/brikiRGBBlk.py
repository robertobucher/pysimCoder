from supsisim.RCPblk import RCPblk


def brikiRGBBlk(pin: list[int]) -> RCPblk:
    """
    Call:   brikiRGBBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("brikiRGB", pin, [], [0, 0], 1)
