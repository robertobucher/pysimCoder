from supsisim.RCPblk import RCPblk


def SHVInputBlk(pout):
    """
    Call:   SHVInputBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("shv_input", [], pout, [0, 0], 0, [])
