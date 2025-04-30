from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def comediDIBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   comediDIBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if size(pout) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pout))

    return RCPblk("comedi_digital_input", [], pout, [0, 0], 0, params)
