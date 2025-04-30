from supsisim.RCPblk import RCPblk
from numpy import size


def clarke_forwardBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   clarke_forwardBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output ports

    Returns
    -------
      Block's reprezentation RCPblk
    """

    nin = size(pin)
    if nin != 3 and nin != 2:
        raise ValueError(
            "Forward Clarke transformation has to have 2 or 3 inputs, current number of inputs is (%i)"
            % (nin)
        )

    return RCPblk("forward_clarke", pin, pout, [0, 0], 1)
