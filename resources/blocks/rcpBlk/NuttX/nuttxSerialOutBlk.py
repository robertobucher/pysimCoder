from supsisim.RCPblk import RCPblk, RcpParam


def nuttxSerialOutBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   nuttxSerialOutBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("serialOut", pin, [], [0, 0], 1, params)
