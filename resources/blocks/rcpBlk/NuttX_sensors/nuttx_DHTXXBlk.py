from supsisim.RCPblk import RCPblk, RcpParam


def nuttx_DHTXXBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   nuttx_DHTXXBlk(pout, port)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("nuttx_DHTXX", [], pout, [0, 0], 0, params)
