from supsisim.RCPblk import RCPblk

def pmsm_alignBlk(pin, pout, hallOffset, IRCperPeriod, angleOffset):
    """

    Call:   pmsm_alignBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('pmsm_align',pin,pout,[0,0],1,[hallOffset, IRCperPeriod, angleOffset],[0, 0])
    return blk
