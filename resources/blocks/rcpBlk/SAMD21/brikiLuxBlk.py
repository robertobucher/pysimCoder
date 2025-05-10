from supsisim.RCPblk import RCPblk


def brikiLuxBlk(pout: list[int]) -> RCPblk:
    """
    Call:   brikiLuxBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("brikiLux", [], pout, [0, 0], 0)
