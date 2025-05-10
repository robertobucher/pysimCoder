from supsisim.RCPblk import RCPblk


def getTimerBlk(pout: list[int]) -> RCPblk:
    """
    Call:   getTimerBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("getTimer", [], pout, [0, 0], 0)
