from supsisim.RCPblk import RCPblk


def ImuAccBlk(pout: list[int]) -> RCPblk:
    """
    Call:   ImuAcc(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("ImuAcc", [], pout, [0, 0], 0)
