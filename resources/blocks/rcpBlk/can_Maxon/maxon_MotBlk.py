from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size


def maxon_MotBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Maxon driver for torque control

    Call: maxon_MotBlk(pin, params)

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

    return RCPblk("maxon_mot", pin, [], [0, 0], 1, params)
