from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size

def dspicENCBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   nuttxENCBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    p = params[0].value[-1]
    p = p.upper()
    pn = ord(p) - ord("0")


    if pn < 1 or pn > 3:
        raise ValueError(
            "QEI should be between QEI1 and QEI3 not %s"
            % (params[0].value)
        )

    params[0].value = pn
    params[0].type = RcpParam.Type.INT

    return RCPblk("dspicENC", [], pout, [0, 0], 0, params)
