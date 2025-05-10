
from supsisim.RCPblk import RCPblk, RcpParam
from numpy import size

def antideadzoneBlk(pin: list[int], pout: list[int], params: RcpParam) -> RCPblk:
    """

    Call:   antideadzone(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
       blk: RCPblk

    """

    upper = params[0].value
    lower = params[1].value

    if upper < 0:
        raise ValueError("Anti Deadzone: the Upper field must be >= 0!")
    if lower > 0:
        raise ValueError("Anti Deadzone: the Lower field must be <= 0!")

    blk = RCPblk('antideadzone', pin, pout, [0,0], 0, params)
    return blk
