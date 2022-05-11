from supsisim.RCPblk import RCPblk

def P3MotNuttxBlk(pin, pout):
    """

    Call:   P3MotNuttxBlk(pin,pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('nuttx_p3m_spi', pin, pout, [0,0], 0, [], [])
    return blk
