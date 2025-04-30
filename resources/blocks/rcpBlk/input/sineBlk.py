from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def sineBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   sineBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if (nin := size(pout)) != 1:
        raise ValueError("Block should have 1 output port; received %i." % nin)

    return RCPblk("sinus", [], pout, [0, 0], 0, params)
