from supsisim.RCPblk import RCPblk, RcpParam


def can_gen_recvBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   can_gen_recvBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    return RCPblk("can_gen_recv", [], pout, [0, 0], 0, params)
