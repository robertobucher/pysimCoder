from supsisim.RCPblk import RCPblk, RcpParam


def nuttx_DACBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   nuttx_DACBlk(pin, param)

    Parameters
    ----------
       pin: connected input port
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("nuttx_DAC", pin, [], [0, 0], 1, params)
