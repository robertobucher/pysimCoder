from supsisim.RCPblk import RCPblk, RcpParam


def mz_apo_ENCBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   mz_apo_ENCBlk(pout, params)

    Parameters
    ----------
       pout: connected output ports
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Value Raw", 0, RcpParam.Type.INT, RcpParam.ShvFlag.VISIBLE))
    params.append(
        RcpParam("Value offset", 0, RcpParam.Type.INT, RcpParam.ShvFlag.VISIBLE)
    )
    return RCPblk("mz_apo_ENC", [], pout, [0, 0], 0, params)
