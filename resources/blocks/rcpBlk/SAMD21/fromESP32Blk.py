from supsisim.RCPblk import RCPblk


def fromESP32Blk(pout: list[int]) -> RCPblk:
    """
    Call:   fromESP32Blk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("fromESP32", [], pout, [0, 0], 0)
