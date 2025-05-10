from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size, unique


def nuttx_PWMBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   nuttx_PWMBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if (nin := size(pin)) != (nch := size(params[1].value)):
        raise ValueError(
            "PWM block: number of inputs (%i) should match number of channels (%i)"
            % (nin, nch)
        )

    if unique(params[1].value).size != nch:
        raise ValueError("PWM block: duplicate channels!")

    params.append(RcpParam("File descriptor", 0, RcpParam.Type.INT))
    return RCPblk("nuttx_PWM", pin, [], [0, 0], 1, params)
