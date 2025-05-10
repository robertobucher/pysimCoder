from supsisim.RCPblk import RCPblk


def brikiRangeBlk(pout: list[int]) -> RCPblk:
    """
    Call:   brikiRangeBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("brikiRange", [], pout, [0, 0], 0)
