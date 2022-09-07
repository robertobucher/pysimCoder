from supsisim.RCPblk import RCPblk

def hall3ph2secBlk(pin, pout, Inverted):
    """

    Call:   hall3ph2secBlk(pin, pout, Gains)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       Inverted : 1 if inverted table is used

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('hall3ph2sec',pin,pout,[0,0],1,[],[Inverted])
    return blk

