from supsisim.RCPblk import RCPblk

def P3MotBlk(pin, pout):
    """

    Call:   P3MotBlk(pin,pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('P3M_spi', pin, pout, [0,0], 0, [], [])
    return blk
