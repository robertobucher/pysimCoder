from supsisim.RCPblk import RCPblk


def SHVOutputBlk(pin):
    """
    Call:   SHVOutputBlk(pout)

    Parameters
    ----------
       pout: connected input port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("shv_output", pin, [], [0, 0], 0, [])
