from supsisim.RCPblk import RCPblk


def toESP32Blk(pin: list[int]) -> RCPblk:
    """
    Call:   toESP32Blk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("toESP32", pin, [], [0, 0], 1)
