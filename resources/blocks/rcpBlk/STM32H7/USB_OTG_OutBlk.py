from supsisim.RCPblk import RCPblk, RcpParam


def USB_OTG_OutBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   USB_OTG_OutBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    params.append(RcpParam("Internal", 0, RcpParam.Type.INT))
    return RCPblk("USB_OTG_Out", pin, [], [0, 0], 1, params)
