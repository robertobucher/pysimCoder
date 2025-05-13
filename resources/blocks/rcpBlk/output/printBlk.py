from supsisim.RCPblk import RCPblk


def printBlk(pin: list[int]) -> RCPblk:
    """
    Call:   printBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("print", pin, [], [0, 0], 1)
