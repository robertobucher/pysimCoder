from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def epos_areadBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   epos_areadBlk(pout, params)

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

    return RCPblk("epos_canopen_aread", [], pout, [0, 0], 0, params)
