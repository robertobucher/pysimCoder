from supsisim.RCPblk import RCPblk
from scipy import size

def switchBlk(pin, pout, cond, val, pers):
    """

    Call:   switchBlk(pin, pout, cond, val, pers)

    Parameters
    ----------
        pin: connected input ports (3)
        pout: connected output port
        cond:  0 >, 1 <
        val:   value to compare
        pers:  switch can change again (0) or is fixed (1)

        Output switches from input 1 to input 2 if the condition is reached
        (input 3 > or <) than val;
        If pers is 1 the system doesn't switch back again if the condition is
        no more satisfied

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 3):
        raise ValueError("Block should have 3 input ports; received %i." % size(pin))

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('switcher',pin,pout,[0,0],1,[val],[cond, pers])
    return blk

