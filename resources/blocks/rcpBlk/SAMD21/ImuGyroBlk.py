from supsisim.RCPblk import RCPblk


def ImuGyroBlk(pout: list[int]) -> RCPblk:
    """
    Call:   ImuGyro(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("ImuGyro", [], pout, [0, 0], 0)
