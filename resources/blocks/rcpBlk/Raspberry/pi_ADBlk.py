from supsisim.RCPblk import RCPblk, RcpParam


def pi_ADBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   pi_ADBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("pi_AD", [], pout, [0, 0], 0, params)
