from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def FH_3XXX_XBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   FH_3XXX_XBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if size(pin) != 1:
        raise ValueError("Block should have 1 input port; received %i." % size(pin))

    return RCPblk("FH_3XXX_X", pin, [], [0, 0], 1, params)
