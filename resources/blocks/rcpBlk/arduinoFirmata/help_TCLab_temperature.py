from supsisim.RCPblk import RCPblk


def help_TCLab_temperature(pin, pout):
    """

    Call:   help_TCLab_temperature(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("help_TCLab_temperature", pin, pout, [0, 0], 0)
