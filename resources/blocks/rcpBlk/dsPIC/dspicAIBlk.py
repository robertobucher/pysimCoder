from supsisim.RCPblk import RCPblk, RcpParam

def dspicAIBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   dspicDIBlk(pout, params)

    Parameters
    ----------
       pout: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """
    p = params[0].value[-1]
    p = p.upper()

    pn = ord(p) - ord("0")

    if pn < 1 or pn > 5:
        raise ValueError(
            "Port should be between ADC1 and ADC5 not %s"
            %(params[0].value)
        )
    
    if params[1].value < 0 or params[1].value > 4:
        raise ValueError(
            "Channel should be between 0 and 15 not %i"
            % (params[1].value))

    params[0].value = pn
    params[0].type = RcpParam.Type.INT
    
    return RCPblk("dspicAI", [], pout, [0, 0], 0, params)
