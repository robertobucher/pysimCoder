from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size, unique


def nuttx_ADCBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   nuttx_AdcBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if (nout := size(pout)) != (nch := size(params[1].value)):
        raise ValueError(
            "ADC block: number of outputs (%i) should match number of channels (%i)"
            % (nout, nch)
        )

    if unique(params[1].value).size != nch:
        raise ValueError("ADC block: duplicate channels!")

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    params.append(RcpParam("Number of channels", 0, RcpParam.Type.INT))
    return RCPblk("nuttx_ADC", [], pout, [0, 0], 0, params)
