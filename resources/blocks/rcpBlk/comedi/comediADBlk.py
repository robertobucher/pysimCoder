from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def comediADBlk(pout: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   comediADBlk(pout, params)

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

    return RCPblk("comedi_analog_input", [], pout, [0, 0], 0, params)
