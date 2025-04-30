from supsisim.RCPblk import RCPblk


def moduloBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   moduloBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("pysim_modulo", pin, pout, [0, 0], 1)
