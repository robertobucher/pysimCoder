from supsisim.RCPblk import RCPblk, RcpParam


def can_sdo_recvBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   can_sdo_recvBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    params[1].value = params[1] + 0x580
    return RCPblk("can_sdo_recv", [], pout, [0, 0], 0, params)
