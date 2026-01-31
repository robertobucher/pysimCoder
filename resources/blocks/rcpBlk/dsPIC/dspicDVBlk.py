from supsisim.RCPblk import RCPblk, RcpParam


def dspicDVBlk(pin: list[int]) -> RCPblk:
    """
    Call:   serialOutBlk(pout, params)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("dspicDV", pin, [], [0, 0], 1)
