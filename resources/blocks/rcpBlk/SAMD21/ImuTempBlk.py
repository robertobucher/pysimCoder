from supsisim.RCPblk import RCPblk


def ImuTempBlk(pout: list[int]) -> RCPblk:
    """
    Call:   ImuTempBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("ImuTemp", [], pout, [0, 0], 0)
