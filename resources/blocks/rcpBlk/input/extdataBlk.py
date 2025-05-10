from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def extdataBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   extdataBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pout) != params[0].value:
        raise ValueError(
            "Block should have %i output port; received %i."
            % (params[0].value, size(pout))
        )

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("extdata", [], pout, [0, 0], 0, params)
