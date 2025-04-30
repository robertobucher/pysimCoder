from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def maxon_EncBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   maxon_EncBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if size(pout) != 1:
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    params[2].value = 4 * params[2].value
    return RCPblk("maxon_enc", [], pout, [0, 0], 0, params)
