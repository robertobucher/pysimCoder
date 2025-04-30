from supsisim.RCPblk import RCPblk, RcpParam


def pmsm_alignBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   pmsm_alignBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal 1", 0, RcpParam.Type.INT))
    params.append(RcpParam("Internal 2", 0, RcpParam.Type.INT))
    return RCPblk("pmsm_align", pin, pout, [0, 0], 1, params)
