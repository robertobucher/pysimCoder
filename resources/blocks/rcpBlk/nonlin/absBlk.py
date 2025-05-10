from supsisim.RCPblk import RCPblk
from numpy import size


def absBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   absBlk(pin, pout)

    Parameters
    ----------
        pin : connected input ports
        pout: connected output ports

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if size(pout) != size(pin):
        raise ValueError(
            "Block should have same input and output port sizes; received %i %i."
            % (size(pin), size(pout))
        )
    return RCPblk("absV", pin, pout, [0, 0], 1)
