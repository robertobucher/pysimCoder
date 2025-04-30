from supsisim.RCPblk import RCPblk, RcpParam


def USB_OTG_InBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   USB_OTG_InBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    outputs = len(pout)
    if len(params[0].value) > outputs:
        N = outputs
    else:
        N = len(params[0].value)

    params[0].value = params[0].value[:N]

    return RCPblk("USB_OTG_In", [], pout, [0, 0], 0, params)
