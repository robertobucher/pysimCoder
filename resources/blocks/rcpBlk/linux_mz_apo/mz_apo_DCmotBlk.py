from supsisim.RCPblk import RCPblk, RcpParam


def mz_apo_DCmotBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   mz_apo_DCmotBlk(pin, pout, params)

    Parameters
    ----------
       pin : connected input ports
       pout: connected output ports
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(
        RcpParam("Motor position", 0, RcpParam.Type.INT, RcpParam.ShvFlag.VISIBLE)
    )
    return RCPblk("mz_apo_DCmot", pin, pout, [0, 0], 0, params)
