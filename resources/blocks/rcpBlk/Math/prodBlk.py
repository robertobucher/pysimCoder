from supsisim.RCPblk import RCPblk
from numpy import size


def prodBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   prodBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pout) != 1:
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    return RCPblk("prod", pin, pout, [0, 0], 1)
