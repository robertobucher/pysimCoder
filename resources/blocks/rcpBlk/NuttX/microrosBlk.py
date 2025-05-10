from supsisim.RCPblk import RCPblk
from numpy import size


def microrosBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   MICROROSBlk(pin, pout, params)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if (nin := size(pin)) > 4:
        raise ValueError("Block have max 4 inputs: received %i input ports" % (nin))
    if (nout := size(pout)) > 4:
        raise ValueError("Block have max 4 outputs: received %i output ports" % (nout))

    return RCPblk("microros", pin, pout, [0, 0], 0)
