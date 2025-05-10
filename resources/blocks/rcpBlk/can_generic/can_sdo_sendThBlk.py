from supsisim.RCPblk import RCPblk, RcpParam


def can_sdo_sendThBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   can_sdo_sendThBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("can_sdo_send", pin, [], [0, 0], 1, params)
