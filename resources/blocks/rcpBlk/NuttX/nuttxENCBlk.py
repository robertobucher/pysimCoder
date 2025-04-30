from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def nuttxENCBlk(pout: list[int], params: RcpParam) -> RCPblk:
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

    if (nout := size(pout)) != 1 and nout != 3:
        raise ValueError("NuttxENC: Number of outputs (%i) should be 1 or 3" % nout)

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("nuttxENC", [], pout, [0, 0], 0, params)
