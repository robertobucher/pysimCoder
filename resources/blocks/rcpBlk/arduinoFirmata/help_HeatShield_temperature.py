from supsisim.RCPblk import RCPblk, RcpParam


def help_HeatShield_temperature(
    pin: list[int], pout: list[int], params: RcpParam
) -> RCPblk:
    """

    Call:   help_HeatShield_temperature(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("help_HeatShield_temperature", pin, pout, [0, 0], 0, params)
