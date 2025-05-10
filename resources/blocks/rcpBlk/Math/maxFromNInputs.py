from supsisim.RCPblk import RCPblk


def maxFromNInputs(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   maxFromNInputs(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("maxFromNInputs", pin, pout, [0, 0], 0)
