from supsisim.RCPblk import RCPblk

def mz_apo_3pmdrv1(pin, pout):
    """

    Call:   mz_apo_3pmdrv1(pin,pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('zynq_3pmdrv1', pin, pout, [0,0], 0, [], [])
    return blk
