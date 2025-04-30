from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def switchBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   switchBlk(pin, pout, params)

    Parameters
    ----------
        pin: connected input ports (3)
        pout: connected output port
        params: block's parameters

        Output switches from input 1 to input 2 if the condition is reached
        (input 3 > or <) than val;
        If pers is 1 the system doesn't switch back again if the condition is
        no more satisfied

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if (nin := size(pin)) != 3:
        raise ValueError("Block should have 3 input ports; received %i." % nin)

    if (nout := size(pout)) != 1:
        raise ValueError("Block should have 1 output port; received %i." % nout)

    return RCPblk("switcher", pin, pout, [0, 0], 1, params)
