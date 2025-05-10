from supsisim.RCPblk import RCPblk


def park_forwardBlk(pin: list[int], pout: list[int]) -> RCPblk:
    """
    Call:   park_forwardBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("forward_park", pin, pout, [0, 0], 1)
