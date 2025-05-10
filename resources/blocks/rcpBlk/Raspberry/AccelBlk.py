from supsisim.RCPblk import RCPblk, RcpParam


def AccelBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   AccelBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("ImuAcc", [], pout, [0, 0], 0, params)
