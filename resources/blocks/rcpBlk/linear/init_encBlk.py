from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def init_encBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   init_encBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if (size(pout) != 1) or (size(pin) != 1):
        raise ValueError(
            "Block should have 1 input and 1 output port; received %i and %i."
            % (size(pin), size(pout))
        )

    params.append(RcpParam("Internal 1", 0, RcpParam.Type.DOUBLE))
    return RCPblk("init_enc", pin, pout, [0, 0], 1, params)
