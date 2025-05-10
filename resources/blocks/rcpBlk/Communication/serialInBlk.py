from supsisim.RCPblk import RCPblk, RcpParam


def serialInBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   serialInBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
       Block's reprezentation RCPblk
    """

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("serialIn", [], pout, [0, 0], 0, params)
