from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def sumBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   sumBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pin) != size(params[0].value):
        raise ValueError(
            "Number of inputs (%i) should match Gain Size (%i)"
            % (size(pin), size(params[0].value))
        )
    if size(pout) != 1:
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    return RCPblk("sum", pin, pout, [0, 0], 1, params)
