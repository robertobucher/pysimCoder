from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def FH_5XXX_ADBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   FH_5XXX_ADBlk(pout, params)

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

    return RCPblk("FH_5XXX_AD", [], pout, [0, 0], 0, params)
