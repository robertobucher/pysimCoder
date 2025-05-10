from supsisim.RCPblk import RCPblk, RcpParam


def GyroBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   GyroBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("ImuGyro", [], pout, [0, 0], 0, params)
