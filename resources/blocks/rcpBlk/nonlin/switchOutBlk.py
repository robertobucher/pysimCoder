from supsisim.RCPblk import RCPblk, RcpParam


def switchOutBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   switchOutBlk(pin, pout, val)

    Parameters
    ----------
        pin: connected input ports
        pout: connected output ports
        params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("switch_output", pin, pout, [0, 0], 1, params)
