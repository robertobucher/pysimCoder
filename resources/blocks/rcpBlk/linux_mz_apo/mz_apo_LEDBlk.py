from supsisim.RCPblk import RCPblk, RcpParam

def mz_apo_LEDBlk(pin: list[int], params: RcpParam) -> RCPblk:
    """
    Call:   mz_apo_LEDBlk(pin, params)

    Parameters
    ----------
       pin : connected input ports
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk('mz_apo_LED', pin, [], [0, 0], 0, params)
