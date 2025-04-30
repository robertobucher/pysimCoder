from supsisim.RCPblk import RCPblk


def minFromNInputs(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   minFromNInputs(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("minFromNInputs", pin, pout, [0, 0], 0)
