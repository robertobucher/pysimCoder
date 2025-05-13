from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def comediDOBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   comediDOBlk(pin, params)

    Parameters
    ----------
       pin: connected input port(s)
       params: block's parameters

    Returns
    -------
        Block's reprezentation RCPblk
    """

    if size(pin) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pin))

    return RCPblk("comedi_digital_output", pin, [], [0, 0], 1, params)
