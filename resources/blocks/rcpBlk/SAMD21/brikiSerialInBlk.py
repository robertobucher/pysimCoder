from supsisim.RCPblk import RCPblk


def brikiSerialInBlk(pout: list[int]) -> RCPblk:
    """
    Call:   brikiSerialInBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("serialIn", [], pout, [0, 0], 0)
