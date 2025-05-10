from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def comediENCBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   comediENCBlk(pout, params)

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

    if params[1].value > 1:
        raise ValueError("Channel value can be 0 or 1 not %i !" % params[1].value)

    return RCPblk("comedi_encoder", [], pout, [0, 0], 0, params)
